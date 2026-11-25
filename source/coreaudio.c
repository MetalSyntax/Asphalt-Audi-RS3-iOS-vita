/**
 * @file coreaudio.c
 * @brief iOS AudioUnit RemoteIO and AudioSession emulation on top of sceAudioOut.
 * @details Handles guest audio output (such as embedded FMOD Ex or custom mixers):
 *          creates a RemoteIO unit, configures stream format and render callbacks,
 *          and streams audio on a dedicated high-priority sceAudioOut thread.
 *          Calls the guest callback via call_guest6.
 */
#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#include <stdlib.h>
#include <string.h>

#include "hle.h"
#include "imports.h"
#include "log.h"
#include "macho.h"

#define kAudioUnitProperty_StreamFormat 8
#define kAudioUnitProperty_SetRenderCallback 23
#define kAudioUnitProperty_SampleRate 2
#define kAudioUnitProperty_ElementCount 11
#define kAudioUnitScope_Input 1
#define kAudioUnitScope_Output 2
#define kAudioFormatFlagIsFloat (1 << 0)
#define kAudioFormatFlagIsNonInterleaved (1 << 5)
#define kAudioFormatFlagsFixed824 (24 << 7) // kLinearPCMFormatFlagsSampleFractionShift
#define FOURCC(a, b, c, d) (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))

#define GRAIN 512

typedef struct {
    double mSampleRate;
    uint32_t mFormatID, mFormatFlags, mBytesPerPacket, mFramesPerPacket, mBytesPerFrame,
        mChannelsPerFrame, mBitsPerChannel, mReserved;
} asbd;

typedef struct {
    uint32_t mNumberBuffers;
    struct {
        uint32_t mNumberChannels, mDataByteSize;
        void *mData;
    } mBuffers[2];
} buffer_list;

#define MAX_BUS 16

typedef struct audio_unit audio_unit;
struct audio_unit {
    asbd fmt;
    void *proc, *refcon;
    SceUID thread;
    volatile int running;
    // MultiChannelMixer (AUGraph): buses de entrada con callback propio, mezclados en el hilo del RemoteIO
    int is_mixer, nbus;
    struct {
        asbd fmt;
        void *proc, *refcon;
        float vol;
        int on;
    } bus[MAX_BUS];
    float out_vol;
    audio_unit *src; // RemoteIO: mezclador conectado a su entrada (AUGraphConnectNodeInput)
};

static int ok(void) { return 0; }

static int au_FindNext(void *prev, const void *desc) { return prev ? 0 : 1; } // single audio component

static int au_InstanceNew(void *comp, audio_unit **out) {
    audio_unit *u = calloc(1, sizeof(*u));
    // RemoteIO default: 44.1 kHz, s16 interleaved stereo.
    u->fmt = (asbd){44100, FOURCC('l', 'p', 'c', 'm'), 0xC, 4, 1, 4, 2, 16, 0};
    *out = u;
    return 0;
}

static int au_SetProperty(audio_unit *u, uint32_t id, uint32_t scope, uint32_t elem, const void *data, uint32_t size) {
    if (u->is_mixer) {
        if (id == kAudioUnitProperty_StreamFormat && scope == kAudioUnitScope_Input && elem < MAX_BUS)
            memcpy(&u->bus[elem].fmt, data, sizeof(asbd));
        else if (id == kAudioUnitProperty_ElementCount && scope == kAudioUnitScope_Input)
            u->nbus = *(const uint32_t *)data < MAX_BUS ? *(const uint32_t *)data : MAX_BUS;
        else if (id == kAudioUnitProperty_SampleRate)
            u->fmt.mSampleRate = *(const double *)data;
        else if (id == kAudioUnitProperty_SetRenderCallback && elem < MAX_BUS) {
            u->bus[elem].proc = ((void *const *)data)[0];
            u->bus[elem].refcon = ((void *const *)data)[1];
        }
        return 0;
    }
    if (id == kAudioUnitProperty_StreamFormat) {
        memcpy(&u->fmt, data, sizeof(asbd));
        l_info("AudioUnit: %d Hz, %u channels, %u bits, flags 0x%X", (int)u->fmt.mSampleRate,
               (unsigned)u->fmt.mChannelsPerFrame, (unsigned)u->fmt.mBitsPerChannel, (unsigned)u->fmt.mFormatFlags);
    } else if (id == kAudioUnitProperty_SetRenderCallback) {
        u->proc = ((void *const *)data)[0];
        u->refcon = ((void *const *)data)[1];
    }
    return 0;
}

static int au_GetProperty(audio_unit *u, uint32_t id, uint32_t scope, uint32_t elem, void *data, uint32_t *size) {
    if (id == kAudioUnitProperty_StreamFormat) {
        memcpy(data, u->is_mixer && scope == kAudioUnitScope_Input && elem < MAX_BUS ? &u->bus[elem].fmt : &u->fmt,
               sizeof(asbd));
        *size = sizeof(asbd);
        return 0;
    }
    memset(data, 0, *size);
    return 0;
}

static int16_t to_s16(const asbd *f, const uint8_t *p) {
    uint32_t fl = f->mFormatFlags;
    if (fl & kAudioFormatFlagIsFloat) {
        float v = *(const float *)p * 32767.0f;
        return v > 32767 ? 32767 : v < -32768 ? -32768 : (int16_t)v;
    }
    if (f->mBitsPerChannel == 32) { // 8.24 fixed-point (canonical AudioUnit) or s32
        int32_t v = *(const int32_t *)p >> ((fl & kAudioFormatFlagsFixed824) ? 9 : 16);
        return v > 32767 ? 32767 : v < -32768 ? -32768 : v;
    }
    return *(const int16_t *)p;
}

// Llama a un render callback del juego (formato f) y suma GRAIN frames estéreo s16 a acc con volumen vol.
static void render_into(const asbd *f, void *proc, void *refcon, uint32_t bus, uint8_t *ts, uint8_t *raw,
                        int32_t *acc, float vol) {
    int ch = f->mChannelsPerFrame, bytes = f->mBitsPerChannel / 8;
    int nonint = (f->mFormatFlags & kAudioFormatFlagIsNonInterleaved) && ch == 2;
    buffer_list bl;
    if (nonint)
        bl = (buffer_list){2, {{1, GRAIN * bytes, raw}, {1, GRAIN * bytes, raw + GRAIN * 4}}};
    else
        bl = (buffer_list){1, {{ch, GRAIN * ch * bytes, raw}}};
    uint32_t flags = 0;
    call_guest6(proc, (uint32_t)refcon, (uint32_t)&flags, (uint32_t)ts, bus, GRAIN, (uint32_t)&bl);
    int v = (int)(vol * 256);
    for (int i = 0; i < GRAIN; i++) {
        int l, r;
        if (nonint) {
            l = to_s16(f, raw + i * bytes);
            r = to_s16(f, raw + GRAIN * 4 + i * bytes);
        } else {
            l = to_s16(f, raw + i * ch * bytes);
            r = ch == 2 ? to_s16(f, raw + (i * 2 + 1) * bytes) : l;
        }
        acc[i * 2] += l * v >> 8;
        acc[i * 2 + 1] += r * v >> 8;
    }
}

static int audio_thread(SceSize args, void *argp) {
    audio_unit *u = *(audio_unit **)argp, *m = u->src;
    // ponytail: el mezclador no remuestrea; se asume que cada bus usa la frecuencia de salida (MixerHost: 44.1 kHz)
    int rate = (int)(m ? m->fmt.mSampleRate : u->fmt.mSampleRate);
    int port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, GRAIN, rate, SCE_AUDIO_OUT_MODE_STEREO);
    if (port < 0) {
        l_error("sceAudioOutOpenPort(%d Hz) = 0x%08X", rate, port);
        return 0;
    }
    uint8_t *raw = malloc(GRAIN * 2 * 4);
    int32_t acc[GRAIN * 2];
    int16_t out[GRAIN * 2];
    uint8_t ts[64] = {0}; // AudioTimeStamp
    double sample_time = 0;
    while (u->running) {
        memcpy(ts, &sample_time, 8);
        ((uint32_t *)ts)[14] = 1; // mFlags = kAudioTimeStampSampleTimeValid
        memset(acc, 0, sizeof(acc));
        if (m) {
            for (int b = 0; b < m->nbus; b++)
                if (m->bus[b].proc && m->bus[b].on)
                    render_into(&m->bus[b].fmt, m->bus[b].proc, m->bus[b].refcon, b, ts, raw, acc,
                                m->bus[b].vol * m->out_vol);
        } else {
            render_into(&u->fmt, u->proc, u->refcon, 0, ts, raw, acc, 1.0f);
        }
        sample_time += GRAIN;
        for (int i = 0; i < GRAIN * 2; i++)
            out[i] = acc[i] > 32767 ? 32767 : acc[i] < -32768 ? -32768 : acc[i];
        sceAudioOutOutput(port, out);
    }
    sceAudioOutReleasePort(port);
    free(raw);
    return 0;
}

static int au_Start(audio_unit *u) {
    if (u->running || (!u->proc && !u->src))
        return 0;
    u->running = 1;
    u->thread = sceKernelCreateThread("coreaudio", audio_thread, 0x10000100, 256 * 1024, 0, CPU_HELPERS, NULL);
    sceKernelStartThread(u->thread, sizeof(u), &u);
    return 0;
}

static int au_Stop(audio_unit *u) {
    if (u->running) {
        u->running = 0;
        sceKernelWaitThreadEnd(u->thread, NULL, NULL);
        sceKernelDeleteThread(u->thread);
    }
    return 0;
}

static int au_Dispose(audio_unit *u) {
    au_Stop(u);
    free(u);
    return 0;
}

static int au_Render(void) { return -1; } // audio input (microphone): not supported

// AUGraph: nodos = audio_unit; un MultiChannelMixer ('mcmx') conectado a un RemoteIO.
#define MAX_NODES 8
typedef struct {
    audio_unit *node[MAX_NODES];
    int n;
} au_graph;

static audio_unit *graph_unit(au_graph *g, int node) {
    return node >= 1 && node <= g->n ? g->node[node - 1] : NULL;
}

static int ag_New(au_graph **out) {
    *out = calloc(1, sizeof(au_graph));
    return 0;
}

static int ag_AddNode(au_graph *g, const uint32_t *desc, int *out) { // desc: AudioComponentDescription
    if (g->n == MAX_NODES)
        return -1;
    audio_unit *u;
    au_InstanceNew(NULL, &u);
    if (desc[1] == FOURCC('m', 'c', 'm', 'x')) {
        u->is_mixer = 1;
        u->nbus = 1;
        u->out_vol = 1.0f;
        for (int b = 0; b < MAX_BUS; b++) {
            u->bus[b].fmt = u->fmt;
            u->bus[b].vol = 1.0f;
            u->bus[b].on = 1;
        }
    }
    g->node[g->n++] = u;
    *out = g->n;
    l_info("AUGraph: nodo %d '%c%c%c%c'", g->n, desc[1] >> 24, (desc[1] >> 16) & 0xFF, (desc[1] >> 8) & 0xFF,
           desc[1] & 0xFF);
    return 0;
}

static int ag_NodeInfo(au_graph *g, int node, uint32_t *desc, audio_unit **unit) {
    audio_unit *u = graph_unit(g, node);
    if (!u)
        return -10860; // kAUGraphErr_InvalidNode
    if (desc)
        memset(desc, 0, 20);
    if (unit)
        *unit = u;
    return 0;
}

static int ag_SetNodeInputCallback(au_graph *g, int node, uint32_t bus, void *const *cb) {
    audio_unit *u = graph_unit(g, node);
    if (!u)
        return -10860;
    return au_SetProperty(u, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, bus, cb, 8);
}

static int ag_Connect(au_graph *g, int src, uint32_t src_out, int dst, uint32_t dst_in) {
    audio_unit *s = graph_unit(g, src), *d = graph_unit(g, dst);
    if (!s || !d)
        return -10860;
    d->src = s;
    return 0;
}

static audio_unit *graph_output(au_graph *g) {
    for (int i = 0; i < g->n; i++)
        if (!g->node[i]->is_mixer)
            return g->node[i];
    return NULL;
}

static int ag_Start(au_graph *g) {
    audio_unit *o = graph_output(g);
    return o ? au_Start(o) : -1;
}

static int ag_Stop(au_graph *g) {
    audio_unit *o = graph_output(g);
    return o ? au_Stop(o) : 0;
}

static int ag_IsRunning(au_graph *g, uint8_t *out) {
    audio_unit *o = graph_output(g);
    *out = o && o->running;
    return 0;
}

static int ag_Dispose(au_graph *g) {
    ag_Stop(g);
    for (int i = 0; i < g->n; i++)
        free(g->node[i]);
    free(g);
    return 0;
}

// kMultiChannelMixerParam_Volume = 0, _Enable = 1 (softfp: el Float32 llega como float normal)
static int au_SetParameter(audio_unit *u, uint32_t id, uint32_t scope, uint32_t elem, float v, uint32_t ofs) {
    if (!u->is_mixer)
        return 0;
    if (id == 0 && scope == kAudioUnitScope_Output)
        u->out_vol = v;
    else if (scope == kAudioUnitScope_Input && elem < MAX_BUS) {
        if (id == 0)
            u->bus[elem].vol = v;
        else if (id == 1)
            u->bus[elem].on = v != 0.0f;
    }
    return 0;
}

// AudioSession: queried by audio engines to select stream properties.
static int as_GetProperty(uint32_t id, uint32_t *size, void *data) {
    memset(data, 0, *size);
    switch (id) {
    case FOURCC('c', 'h', 's', 'r'): // CurrentHardwareSampleRate (Float64)
        *(double *)data = 44100.0;
        break;
    case FOURCC('c', 'h', 'o', 'c'): // CurrentHardwareOutputNumberChannels
        *(uint32_t *)data = 2;
        break;
    case FOURCC('c', 'h', 'b', 'd'): // CurrentHardwareIOBufferDuration (Float32)
        *(float *)data = (float)GRAIN / 44100.0f;
        break;
    case FOURCC('c', 'h', 'o', 'l'): // CurrentHardwareOutputLatency (Float32)
        *(float *)data = 0.012f;
        break;
    default:
        l_debug("AudioSessionGetProperty('%c%c%c%c') -> 0", id >> 24, (id >> 16) & 0xFF, (id >> 8) & 0xFF, id & 0xFF);
    }
    return 0;
}

const import_entry coreaudio_imports[] = {
    F(AudioComponentFindNext, au_FindNext), F(AudioComponentInstanceNew, au_InstanceNew),
    F(AudioComponentInstanceDispose, au_Dispose), F(AudioUnitSetProperty, au_SetProperty),
    F(AudioUnitGetProperty, au_GetProperty), F(AudioUnitInitialize, ok), F(AudioUnitUninitialize, ok),
    F(AudioOutputUnitStart, au_Start), F(AudioOutputUnitStop, au_Stop), F(AudioUnitRender, au_Render),
    F(NewAUGraph, ag_New), F(AUGraphAddNode, ag_AddNode), F(AUGraphOpen, ok), F(AUGraphNodeInfo, ag_NodeInfo),
    F(AUGraphSetNodeInputCallback, ag_SetNodeInputCallback), F(AUGraphConnectNodeInput, ag_Connect),
    F(AUGraphInitialize, ok), F(AUGraphUninitialize, ok), F(AUGraphStart, ag_Start), F(AUGraphStop, ag_Stop),
    F(AUGraphIsRunning, ag_IsRunning), F(AUGraphClose, ok), F(DisposeAUGraph, ag_Dispose), F(CAShow, ok),
    F(AudioUnitSetParameter, au_SetParameter),
    F(AudioSessionInitialize, ok), F(AudioSessionSetActive, ok), F(AudioSessionSetProperty, ok),
    F(AudioSessionGetProperty, as_GetProperty), F(AudioSessionAddPropertyListener, ok),
    // imported via __nl_symbol_ptr (function pointer): requires code stub, not data
    F(AudioSessionRemovePropertyListenerWithUserData, ok),
};
TABLE_END(coreaudio)

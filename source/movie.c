// Videos de MPMoviePlayerController con sceAvPlayer (H.264/AAC por hardware), leyendo el archivo original del IPA. Bloquea el hilo principal hasta
// que el video termina o se salta (X/O/START o toque): en iOS el video es modal y el juego espera la
// notificación de fin, que uikit.c manda después de volver de acá.
#include <malloc.h>
#include <psp2/audioout.h>
#include <psp2/avplayer.h>
#include <psp2/ctrl.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/sysmodule.h>
#include <psp2/touch.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vitaGL.h>

#include "gles.h"
#include "log.h"
#include "movie.h"

static void *mem_alloc(void *p, uint32_t align, uint32_t size) { return memalign(align, size); }
static void mem_free(void *p, void *ptr) { free(ptr); }

// Frames de video: memoria física contigua que escribe el decoder.
static void *frame_alloc(void *p, uint32_t align, uint32_t size) {
    if (align < 0x40000)
        align = 0x40000;
    size = (size + align - 1) & ~(align - 1);
    SceKernelAllocMemBlockOpt opt = {.size = sizeof(opt), .attr = 4, .alignment = align};
    SceUID b = sceKernelAllocMemBlock("movie_frame", SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW, size, &opt);
    if (b < 0) { // log 006: 0x80024309, la CDRAM ya es de vitaGL -> RAM física contigua (bloques de 1 MB)
        size = (size + 0xFFFFF) & ~0xFFFFF;
        b = sceKernelAllocMemBlock("movie_frame", SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_NC_RW, size, NULL);
    }
    if (b < 0) {
        l_error("movie: sin memoria para frames (%u bytes): 0x%08X", (unsigned)size, b);
        return NULL;
    }
    void *base = NULL;
    sceKernelGetMemBlockBase(b, &base);
    return base;
}
static void frame_free(void *p, void *ptr) {
    SceUID b = sceKernelFindMemBlockByAddr(ptr, 0);
    if (b >= 0)
        sceKernelFreeMemBlock(b);
}

// E/S propia: AvPlayer decide el contenedor por la extensión y no acepta .m4v/.mov; el archivo es MP4 igual.
static SceUID mfd;
static uint64_t msize;
static int file_open(void *p, const char *name) { return 0; }
static int file_close(void *p) { return 0; }
static int file_read(void *p, uint8_t *buf, uint64_t pos, uint32_t len) { return sceIoPread(mfd, buf, len, pos); }
static uint64_t file_size(void *p) { return msize; }

static SceAvPlayerHandle av;
static volatile int audio_run;

static int audio_thread(SceSize args, void *argp) {
    int port = -1, rate = 0, ch = 0;
    while (audio_run && sceAvPlayerIsActive(av)) {
        SceAvPlayerFrameInfo f;
        if (!sceAvPlayerGetAudioData(av, &f)) {
            sceKernelDelayThread(1000);
            continue;
        }
        int r = f.details.audio.sampleRate, c = f.details.audio.channelCount;
        int grain = f.details.audio.size / (2 * c);
        if (port < 0) {
            port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_MAIN, grain, r,
                                       c == 1 ? SCE_AUDIO_OUT_MODE_MONO : SCE_AUDIO_OUT_MODE_STEREO);
            if (port < 0) {
                l_error("movie: sceAudioOutOpenPort = 0x%08X (video sin sonido)", port);
                break;
            }
            int vol[2] = {SCE_AUDIO_VOLUME_0DB, SCE_AUDIO_VOLUME_0DB};
            sceAudioOutSetVolume(port, SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH, vol);
            rate = r, ch = c;
        } else if (r != rate || c != ch) {
            sceAudioOutSetConfig(port, grain, r, c == 1 ? SCE_AUDIO_OUT_MODE_MONO : SCE_AUDIO_OUT_MODE_STEREO);
            rate = r, ch = c;
        }
        sceAudioOutOutput(port, f.pData);
    }
    if (port >= 0) {
        sceAudioOutOutput(port, NULL); // esperar a que salga lo encolado
        sceAudioOutReleasePort(port);
    }
    return 0;
}

static int skip_pressed(void) {
    SceCtrlData pad;
    SceTouchData t;
    sceCtrlPeekBufferPositive(0, &pad, 1);
    sceTouchPeek(SCE_TOUCH_PORT_FRONT, &t, 1);
    return (pad.buttons & (SCE_CTRL_CROSS | SCE_CTRL_CIRCLE | SCE_CTRL_START)) || t.reportNum > 0;
}

int movie_play(const char *path, uint32_t *frames) {
    char abs[512]; // sceIoOpen no conoce el cwd de newlib (chdir del juego): "data/x.m4v" daba 0x80010016
    if (!strchr(path, ':') && path[0] != '/' && getcwd(abs, sizeof(abs))) {
        size_t n = strlen(abs);
        snprintf(abs + n, sizeof(abs) - n, "%s%s", n && abs[n - 1] == '/' ? "" : "/", path);
        path = abs;
    }
    struct stat st;
    if (stat(path, &st) != 0) {
        l_error("movie: no existe %s", path);
        return -1;
    }
    mfd = sceIoOpen(path, SCE_O_RDONLY, 0);
    if (mfd < 0) {
        l_error("movie: sceIoOpen(%s) = 0x%08X", path, mfd);
        return -1;
    }
    msize = st.st_size;

    sceSysmoduleLoadModule(SCE_SYSMODULE_AVPLAYER);
    SceAvPlayerInitData init = {0};
    init.memoryReplacement.allocate = mem_alloc;
    init.memoryReplacement.deallocate = mem_free;
    init.memoryReplacement.allocateTexture = frame_alloc;
    init.memoryReplacement.deallocateTexture = frame_free;
    init.fileReplacement.open = file_open;
    init.fileReplacement.close = file_close;
    init.fileReplacement.readOffset = file_read;
    init.fileReplacement.size = file_size;
    init.basePriority = 0xA0;
    init.numOutputVideoFrameBuffers = 2;
    init.autoStart = 1;
    av = sceAvPlayerInit(&init);
    if (!av || (unsigned)av >> 16 == 0x806A) { // el handle es un puntero (0x8xxxxxxx): "< 0" no es error

        l_error("movie: sceAvPlayerInit = 0x%08X", av);
        sceIoClose(mfd);
        return -1;
    }
    int r = sceAvPlayerAddSource(av, "movie.mp4"); // el nombre solo le dice el contenedor; lee el original (file_*)
    if (r < 0) {
        l_error("movie: sceAvPlayerAddSource(%s) = 0x%08X", path, r);
        sceAvPlayerClose(av);
        sceIoClose(mfd);
        return -1;
    }
    l_info("movie: reproduciendo %s", path);

    audio_run = 1;
    SceUID at = sceKernelCreateThread("movie_audio", audio_thread, 0x10000100 - 10, 0x4000, 0, 0, NULL);
    if (at >= 0)
        sceKernelStartThread(at, 0, NULL);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int shown = 0, skipped = 0, held = skip_pressed(); // un botón ya apretado al empezar no salta
    uint64_t t0 = sceKernelGetProcessTimeWide();
    while (sceAvPlayerIsActive(av)) {
        int s = skip_pressed();
        if (s && !held) {
            skipped = 1;
            break;
        }
        held = s;
        SceAvPlayerFrameInfo f;
        if (sceAvPlayerGetVideoData(av, &f)) {
            int w = f.details.video.width, h = f.details.video.height;
            glBindTexture(GL_TEXTURE_2D, tex);
            glCompressedTexImage2D(GL_TEXTURE_2D, 0, VGL_YUV420P_NV12_BT601, w, h, 0, w * h * 3 / 2, f.pData);
            gles_present_movie(tex, w, h);
            (*frames)++; // el watchdog de uikit.c cuenta presentaciones
            if (!shown++)
                l_info("movie: primer frame %dx%d", w, h);
        } else if (!shown && sceKernelGetProcessTimeWide() - t0 > 5000000) {
            l_error("movie: 5 s sin frames de video, abortado");
            break;
        } else
            sceKernelDelayThread(2000);
    }
    audio_run = 0;
    sceAvPlayerStop(av);
    if (at >= 0) {
        sceKernelWaitThreadEnd(at, NULL, NULL);
        sceKernelDeleteThread(at);
    }
    glFinish(); // la GPU deja de leer la textura antes de liberar los frames
    glDeleteTextures(1, &tex);
    sceAvPlayerClose(av);
    sceIoClose(mfd);
    l_info("movie: fin (%d frames%s)", shown, skipped ? ", saltado" : "");
    return 0;
}

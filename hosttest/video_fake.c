/* hosttest: sin decoder H.264. sceAvPlayerInit falla -> movie_play() devuelve -1 y uikit.c sigue por el
   camino "el video terminó" (MPMoviePlayerPlaybackDidFinishNotification). */
#include <psp2/avplayer.h>
SceAvPlayerHandle sceAvPlayerInit(SceAvPlayerInitData *d) { (void)d; return (SceAvPlayerHandle)0x806A0003; }
int sceAvPlayerAddSource(SceAvPlayerHandle h, const char *f) { (void)h; (void)f; return -1; }
int sceAvPlayerClose(SceAvPlayerHandle h) { (void)h; return 0; }
int sceAvPlayerStop(SceAvPlayerHandle h) { (void)h; return 0; }
SceBool sceAvPlayerIsActive(SceAvPlayerHandle h) { (void)h; return 0; }
SceBool sceAvPlayerGetAudioData(SceAvPlayerHandle h, SceAvPlayerFrameInfo *i) { (void)h; (void)i; return 0; }
SceBool sceAvPlayerGetVideoData(SceAvPlayerHandle h, SceAvPlayerFrameInfo *i) { (void)h; (void)i; return 0; }
/* resto de movie.c: no se llega a usar con sceAvPlayerInit fallando */
#include <psp2/audioout.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/io/fcntl.h>
#include <unistd.h>
int sceAudioOutSetConfig(int port, SceSize len, int freq, SceAudioOutMode mode) { (void)port; (void)len; (void)freq; (void)mode; return 0; }
SceUID sceKernelAllocMemBlock(const char *n, SceKernelMemBlockType t, SceSize s, SceKernelAllocMemBlockOpt *o) { (void)n; (void)t; (void)s; (void)o; return -1; }
SceUID sceKernelFindMemBlockByAddr(const void *a, SceSize s) { (void)a; (void)s; return -1; }
int sceKernelFreeMemBlock(SceUID b) { (void)b; return 0; }
int sceIoPread(SceUID fd, void *buf, SceSize n, SceOff off) { return pread(fd - 1, buf, n, (off_t)off); }

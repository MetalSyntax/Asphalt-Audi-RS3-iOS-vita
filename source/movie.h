#pragma once
#include <stdint.h>
// Reproduce un video (bloqueante) y suma 1 a *frames por frame presentado (watchdog). -1 si no se pudo.
int movie_play(const char *path, uint32_t *frames);

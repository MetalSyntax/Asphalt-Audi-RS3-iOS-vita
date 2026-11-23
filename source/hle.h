#pragma once
// Rutas en la Vita (sandbox de iOS) + configuración del port.
#include "port_config.h"

#ifndef DATA_PATH
#define DATA_PATH "ux0:data/" PORT_SLUG "/"
#endif
#define BUNDLE_PATH DATA_PATH "game_app/" // la .app del IPA (recursos + ejecutable)
#define DOCS_PATH DATA_PATH "Documents/"  // NSDocumentDirectory (partidas guardadas)
#define LOGS_PATH DATA_PATH "logs/"
#define CPU_HELPERS 0x00060000 // SCE_KERNEL_CPU_MASK_USER_1 | USER_2: hilos de apoyo (audio) fuera del núcleo del juego

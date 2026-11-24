#pragma once
// Configuración propia de este juego (el resto de source/ es común a los ports iOS de la familia).
#define PORT_NAME "Asphalt Audi RS 3"
#define PORT_SLUG "asphalt-audirs3"
#define PORT_EXEC "AsphaltAudiRS3"      // CFBundleExecutable
#define PORT_ROTATION 1                // Info.plist: LandscapeRight (0 = vertical)
#define PORT_2D 0                      // 1 = juego 2D: el menú de opciones ofrece además el filtro "pixel"
#define PORT_STRETCH 0                 // valor por defecto de options.txt stretch=
#define PORT_SYSTEM_VERSION "3.1.3"    // MinimumOSVersion 3.1.3
#define PORT_DEVICE_MACHINE "iPhone2,1" // 3GS: texturas de calidad alta en Gameloft
#define GLES_POOL_MB 16
#define PORT_ACCEL_FROM_STICK 1        // carreras con tilt: el stick izquierdo inclina el teléfono
#define PORT_SCREEN_W 320
#define PORT_SCREEN_H 480
#define PORT_SCREEN_SCALE 1.0f
// Controles táctiles que gles.c no dibuja en carrera (options.txt hud=1), en coordenadas de controls.txt (480x320):
// freno, gas, nitro, pausa. Medidas de la captura 2026-11-24-085618 con margen.
#define PORT_HUD_ZONES {{8, 244, 104, 320}, {300, 248, 396, 320}, {396, 206, 476, 294}, {0, 0, 38, 40}}

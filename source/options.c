// Menú de opciones de video y remapeo de controles (SELECT+START), al estilo del de Zenonia2-vita: panel con la fuente 8x8 de PSPSDK
// dibujado en una textura, abajo de la pantalla para ver el efecto del filtro en vivo. El juego queda en pausa
// (gles_present no vuelve hasta cerrar). Se guarda en DATA_PATH options.txt, editable a mano:
//     filter=sharp      (bilinear | sharp | pixel; pixel solo si PORT_2D)
//     msaa=0            (0 | 2 | 4; se aplica al reiniciar)
//     rotation=1        (0 sin girar, 1 landscape-right, 2 landscape-left)
//     stretch=0         (1 = pantalla completa)
//     hud=1             (1 = controles táctiles del juego ocultos en carrera; siguen funcionando)
//     steer=0           (0 inclinación + tocar lados, 1 solo inclinación, 2 solo tocar lados: stick/cruceta)
//     device=iPhone2,1  (hw.machine que ve el juego: elige calidad de texturas/efectos; al reiniciar)
#include <psp2/ctrl.h>
#include <stdio.h>
#include <string.h>
#include <vitaGL.h>

#include "hle.h"
#include "input.h"
#include "log.h"
#include "options.h"

#ifndef PORT_2D
#define PORT_2D 0 // port_config.h: 1 = juego 2D, habilita el filtro "pixel" (vecino más cercano)
#endif
#ifndef PORT_STRETCH
#define PORT_STRETCH 0
#endif
#define OPTIONS_PATH DATA_PATH "options.txt"

extern const unsigned char font8x8[];
extern int port_rotation;

int opt_filter = FILTER_SHARP, opt_msaa = 0, opt_msaa_active = 0, opt_stretch = PORT_STRETCH, opt_hud_hidden = 1;
int opt_steer = STEER_BOTH;
static const char *devices[][2] = {{"iPhone1,1", "IPHONE 2G"}, {"iPhone1,2", "IPHONE 3G"}, {"iPod2,1", "IPOD TOUCH 2G"},
                                   {"iPhone2,1", "IPHONE 3GS"}, {"iPod3,1", "IPOD TOUCH 3G"}};
// ponytail: sin iPhone 4 (iPhone3,1): el juego cambia a otros shaders de carrocería y se rompe el arreglo; agregarlo
// cuando haya un log de carrera con ese perfil. Un options.txt con device=iPhone3,1 vuelve a PORT_DEVICE_MACHINE.
#define NDEV (int)(sizeof(devices) / sizeof(devices[0]))
static int opt_device = -1, opt_device_active = -1; // -1 = PORT_DEVICE_MACHINE
const char *options_machine(void) { return opt_device_active < 0 ? PORT_DEVICE_MACHINE : devices[opt_device_active][0]; }
static const char *steer_labels[] = {"TILT + TOUCH SIDES", "TILT ONLY", "TOUCH SIDES ONLY"};
static volatile int open_req, active;

static const char *filter_names[] = {"bilinear", "sharp", "pixel"};
static const char *filter_labels[] = {"BILINEAR (SMOOTH)", "SHARP BILINEAR", "PIXEL (NEAREST)"};
static const char *rot_labels[] = {"NONE", "LANDSCAPE RIGHT", "LANDSCAPE LEFT"};

void options_load(void) {
    FILE *f = fopen(OPTIONS_PATH, "r");
    if (f) {
        char line[64], s[16];
        int v;
        while (fgets(line, sizeof(line), f)) {
            if (sscanf(line, "filter=%15s", s) == 1) {
                for (int i = 0; i < 3; i++)
                    if (!strcmp(s, filter_names[i]))
                        opt_filter = i;
            } else if (sscanf(line, "msaa=%d", &v) == 1 && (v == 0 || v == 2 || v == 4))
                opt_msaa = v;
            else if (sscanf(line, "rotation=%d", &v) == 1 && v >= 0 && v <= 2)
                port_rotation = v;
            else if (sscanf(line, "stretch=%d", &v) == 1)
                opt_stretch = !!v;
            else if (sscanf(line, "hud=%d", &v) == 1)
                opt_hud_hidden = !!v;
            else if (sscanf(line, "steer=%d", &v) == 1 && v >= 0 && v <= 2)
                opt_steer = v;
            else if (sscanf(line, "device=%15s", s) == 1)
                for (int i = 0; i < NDEV; i++)
                    if (!strcmp(s, devices[i][0]))
                        opt_device = i;
        }
        fclose(f);
    }
    if (opt_filter == FILTER_PIXEL && !PORT_2D)
        opt_filter = FILTER_SHARP;
    opt_msaa_active = opt_msaa;
    if (opt_device < 0)
        for (int i = 0; i < NDEV; i++)
            if (!strcmp(PORT_DEVICE_MACHINE, devices[i][0]))
                opt_device = i;
    opt_device_active = opt_device;
    l_info("options.txt: filtro %s, MSAA %dx, rotación %d, pantalla completa %d, dirección %d, equipo %s",
           filter_names[opt_filter], opt_msaa, port_rotation, opt_stretch, opt_steer, options_machine());
}

static void options_save(void) {
    FILE *f = fopen(OPTIONS_PATH, "w");
    if (!f)
        return;
    fprintf(f, "filter=%s\nmsaa=%d\nrotation=%d\nstretch=%d\nhud=%d\nsteer=%d\n", filter_names[opt_filter], opt_msaa,
            port_rotation, opt_stretch, opt_hud_hidden, opt_steer);
    if (opt_device >= 0)
        fprintf(f, "device=%s\n", devices[opt_device][0]);
    fclose(f);
}

int options_msaa_mode(void) {
    return opt_msaa == 4 ? SCE_GXM_MULTISAMPLE_4X : opt_msaa == 2 ? SCE_GXM_MULTISAMPLE_2X : SCE_GXM_MULTISAMPLE_NONE;
}

void options_open(void) { open_req = 1; }
int options_pending(void) { return open_req; }
int options_active(void) { return active; }

// ---------------------------------------------------------------- panel

enum { ROW_SIZE, ROW_FILTER, ROW_MSAA, ROW_ROT, ROW_HUD, ROW_STEER, ROW_DEVICE, ROWS_VIDEO };
#define ROWS (ROWS_VIDEO + input_bind_count()) // + una fila por acción de controls.txt
#define PW 480
#define PH 272 // x2 = 544: pantalla completa con 8 acciones
static uint32_t px[PW * PH]; // ABGR (GL_RGBA/UNSIGNED_BYTE en little endian)
static GLuint tex;
static int cursor;

#define C_BG 0xFF201008
#define C_SEL 0xFF603818
#define C_TEXT 0xFFE0E0E0
#define C_DIM 0xFF909090
#define C_HEAD 0xFF40C0FF

static void fill(int y, int h, uint32_t c) {
    for (int i = y * PW; i < (y + h) * PW; i++)
        px[i] = c;
}
static void text(int x, int y, uint32_t c, const char *s) {
    for (; *s && x + 8 <= PW; s++, x += 8)
        for (int r = 0; r < 8; r++) {
            unsigned char bits = font8x8[(unsigned char)*s * 8 + r];
            for (int col = 0; col < 8; col++)
                if (bits & (0x80 >> col))
                    px[(y + r) * PW + x + col] = c;
        }
}

static void render(void) {
    static const char *labels[ROWS_VIDEO] = {"Screen size", "Screen filter", "Anti-aliasing (MSAA)", "Rotation",
                                              "On-screen buttons", "Steering (stick/d-pad)", "iPhone profile"};
    char val[48];
    fill(0, PH, C_BG);
    text(16, 8, C_HEAD, "VIDEO OPTIONS");
    if (input_bind_count())
        text(16, 28 + ROWS_VIDEO * 14 + 4, C_HEAD, "CONTROLS (button -> on-screen action)");
    for (int r = 0; r < ROWS; r++) {
        int y = 28 + r * 14 + (r >= ROWS_VIDEO ? 20 : 0); // 14: caben 7 opciones + 8 acciones
        if (r == cursor)
            fill(y - 3, 14, C_SEL);
        text(16, y, C_TEXT, r < ROWS_VIDEO ? labels[r] : input_bind_label(r - ROWS_VIDEO));
        if (r >= ROWS_VIDEO)
            strcpy(val, input_bind_button(r - ROWS_VIDEO));
        else if (r == ROW_SIZE)
            strcpy(val, opt_stretch ? "FULL SCREEN" : "ORIGINAL ASPECT");
        else if (r == ROW_FILTER)
            strcpy(val, filter_labels[opt_filter]);
        else if (r == ROW_MSAA)
            snprintf(val, sizeof(val), "%s%s", opt_msaa == 4 ? "4X" : opt_msaa == 2 ? "2X" : "OFF",
                     opt_msaa != opt_msaa_active ? " (RESTART)" : "");
        else if (r == ROW_HUD)
            strcpy(val, opt_hud_hidden ? "HIDDEN (STILL WORK)" : "SHOWN");
        else if (r == ROW_STEER)
            strcpy(val, steer_labels[opt_steer]);
        else if (r == ROW_DEVICE)
            snprintf(val, sizeof(val), "%s%s", opt_device < 0 ? PORT_DEVICE_MACHINE : devices[opt_device][1],
                     opt_device != opt_device_active ? " (RESTART)" : "");
        else
            strcpy(val, rot_labels[port_rotation]);
        text(208, y, C_TEXT, val);
    }
    text(16, PH - 28, C_DIM, "LEFT/RIGHT or X change   O save & close");
    text(16, PH - 16, C_DIM, opt_msaa != opt_msaa_active || opt_device != opt_device_active
                                 ? "MSAA/profile apply after restarting the game"
                                 : "Older iPhone profile = lighter graphics, more FPS");
}

static void change(int row, int d) {
    if (row >= ROWS_VIDEO)
        input_bind_cycle(row - ROWS_VIDEO, d);
    else if (row == ROW_SIZE)
        opt_stretch = !opt_stretch;
    else if (row == ROW_FILTER) {
        int n = PORT_2D ? 3 : 2;
        opt_filter = (opt_filter + d + n) % n;
    } else if (row == ROW_MSAA) {
        static const int v[] = {0, 2, 4};
        int i = opt_msaa / 2; // 0,2,4 -> 0,1,2
        opt_msaa = v[(i + d + 3) % 3];
    } else if (row == ROW_HUD)
        opt_hud_hidden = !opt_hud_hidden;
    else if (row == ROW_STEER)
        opt_steer = (opt_steer + d + 3) % 3;
    else if (row == ROW_DEVICE && opt_device >= 0)
        opt_device = (opt_device + d + NDEV) % NDEV;
    else
        port_rotation = (port_rotation + d + 3) % 3;
}

static void draw_panel(void) {
    if (!tex) {
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    render();
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, PW, PH, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
    // x2 en la franja de abajo (960x224); la proyección del present es 0..960 x 0..544 con y hacia abajo
    static const float xy[] = {0, 544 - PH * 2, 960, 544 - PH * 2, 0, 544, 960, 544};
    static const float uv[] = {0, 0, 1, 0, 0, 1, 1, 1};
    glVertexPointer(2, GL_FLOAT, 0, xy);
    glTexCoordPointer(2, GL_FLOAT, 0, uv);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static uint32_t pad(void) {
    SceCtrlData c;
    sceCtrlPeekBufferPositive(0, &c, 1);
    return c.buttons;
}

void options_run(void (*draw_game)(void)) {
    open_req = 0, active = 1;
    uint32_t prev = pad(); // SELECT+START siguen apretados
    l_info("menú de opciones abierto");
    for (;;) {
        uint32_t b = pad(), p = b & ~prev;
        prev = b;
        if (p & (SCE_CTRL_CIRCLE | SCE_CTRL_START))
            break;
        if (p & SCE_CTRL_UP)
            cursor = (cursor + ROWS - 1) % ROWS;
        if (p & SCE_CTRL_DOWN)
            cursor = (cursor + 1) % ROWS;
        if (p & (SCE_CTRL_RIGHT | SCE_CTRL_CROSS))
            change(cursor, 1);
        if (p & SCE_CTRL_LEFT)
            change(cursor, -1);
        draw_panel();
        vglSwapBuffers(GL_FALSE);
        draw_game(); // filtro/rotación nuevos en vivo
    }
    options_save();
    input_save_controls();
    active = 0; // antes de esperar a soltar: ya no hay riesgo de cuelgue largo
    l_info("opciones: filtro %s, MSAA %dx (activo %dx), rotación %d, pantalla completa %d", filter_names[opt_filter],
           opt_msaa, opt_msaa_active, port_rotation, opt_stretch);
    // no devolver el botón de cierre al juego (input.c lo leería como toque)
    while (pad()) {
        vglSwapBuffers(GL_FALSE);
        draw_game();
    }
}

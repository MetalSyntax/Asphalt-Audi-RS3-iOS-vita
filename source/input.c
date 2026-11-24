// Touch frontal + botones de la Vita -> toques de UIKit (uikit.c) en coordenadas de la vista.
//
// La pantalla muestra el drawable girado/escalado (gles_present); acá se hace el mapeo inverso:
// punto de pantalla -> normalizado en el rectángulo destino -> coordenadas de la vista de iOS.
// Botones: cada uno "toca" un punto configurable en ux0:data/<slug>/controls.txt, en coordenadas
// lógicas de lo que se ve (p. ej. 480x320 para un juego apaisado):
//     CROSS 420 260
//     RTRIGGER 440 160
// Se edita por FTP sin recompilar. SELECT+START abre el menú de opciones (filtro, MSAA, rotación).
#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/touch.h>
#include <stdio.h>
#include <string.h>

#include "gles.h"
#include "hle.h"
#include "input.h"
#include "options.h"
#include "log.h"

#define FINGERS 6
#define BTN_FINGER0 2 // los botones usan dedos virtuales 2..9 (los reales, 0..1); uikit.c MAX_TOUCH = 11

static struct {
    int id, down;
} fingers[2];
// Cuadrantes del táctil trasero como botones (como Zenonia 2 / Inotia 4), en bits que SceCtrl no usa.
#define REAR_TL 0x10000000u
#define REAR_TR 0x20000000u
#define REAR_BL 0x40000000u
#define REAR_BR 0x80000000u
static uint32_t tap_up; // binds de START/SELECT que tocaron al soltar: se levantan en el siguiente poll
static uint32_t prev_buttons;
static float stick_x, stick_y;

static const struct {
    const char *name;
    uint32_t mask;
} names[] = {
    {"CROSS", SCE_CTRL_CROSS},       {"CIRCLE", SCE_CTRL_CIRCLE}, {"SQUARE", SCE_CTRL_SQUARE},
    {"TRIANGLE", SCE_CTRL_TRIANGLE}, {"LTRIGGER", SCE_CTRL_LTRIGGER}, {"RTRIGGER", SCE_CTRL_RTRIGGER},
    {"START", SCE_CTRL_START},       {"SELECT", SCE_CTRL_SELECT}, {"UP", SCE_CTRL_UP},
    {"DOWN", SCE_CTRL_DOWN},         {"LEFT", SCE_CTRL_LEFT},     {"RIGHT", SCE_CTRL_RIGHT},
    {"REAR_TL", REAR_TL},            {"REAR_TR", REAR_TR},        {"REAR_BL", REAR_BL},
    {"REAR_BR", REAR_BR},
};
#define MAX_BIND 8
#define NNAMES (int)(sizeof(names) / sizeof(names[0]))
static struct {
    int name;   // índice en names[]
    float x, y; // lógicas (lo que se ve)
    char label[24];
} binds[MAX_BIND];
static int nbinds;
// STICK x y r: stick izquierdo / cruceta = dedo virtual arrastrado alrededor de (x,y) lógicos (pad táctil).
static float pad_x, pad_y, pad_r;
static int pad_down;
#define PAD_FINGER (BTN_FINGER0 + MAX_BIND)

static void load_controls(void) {
    FILE *f = fopen(DATA_PATH "controls.txt", "r");
    if (!f) {
        l_info("sin controls.txt: solo touch + stick (acelerómetro)");
        return;
    }
    char line[128], n[32], label[24];
    float x, y, r;
    while (fgets(line, sizeof(line), f)) {
        label[0] = 0;
        if (sscanf(line, "STICK %f %f %f", &x, &y, &r) == 3)
            pad_x = x, pad_y = y, pad_r = r;
        else if (line[0] != '#' && nbinds < MAX_BIND && sscanf(line, "%31s %f %f %23[^\r\n]", n, &x, &y, label) >= 3)
            for (int i = 0; i < NNAMES; i++)
                if (!strcmp(n, names[i].name)) {
                    binds[nbinds].name = i, binds[nbinds].x = x, binds[nbinds].y = y;
                    strcpy(binds[nbinds++].label, label);
                }
    }
    fclose(f);
    l_info("controls.txt: %d botones%s", nbinds, pad_r > 0 ? " + STICK" : "");
}

int input_bind_count(void) { return nbinds; }
const char *input_bind_label(int i) {
    if (!binds[i].label[0])
        snprintf(binds[i].label, sizeof(binds[i].label), "Touch %d,%d", (int)binds[i].x, (int)binds[i].y);
    return binds[i].label;
}
const char *input_bind_button(int i) { return names[binds[i].name].name; }
void input_bind_cycle(int i, int d) { binds[i].name = (binds[i].name + d + NNAMES) % NNAMES; }

void input_save_controls(void) {
    if (!nbinds && pad_r <= 0)
        return;
    FILE *f = fopen(DATA_PATH "controls.txt", "w");
    if (!f)
        return;
    fprintf(f, "# Botón x y acción (coordenadas de lo que se ve, 480x320 apaisado). Se remapea con SELECT+START.\n"
               "# Botones: CROSS CIRCLE SQUARE TRIANGLE LTRIGGER RTRIGGER START SELECT UP DOWN LEFT RIGHT\n"
               "# REAR_TL REAR_TR REAR_BL REAR_BR = cuadrantes del táctil trasero. START y SELECT tocan al soltar.\n");
    if (pad_r > 0)
        fprintf(f, "STICK %g %g %g\n", pad_x, pad_y, pad_r);
    for (int i = 0; i < nbinds; i++)
        fprintf(f, "%s %g %g %s\n", names[binds[i].name].name, binds[i].x, binds[i].y, binds[i].label);
    fclose(f);
}

void input_init(void) {
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    load_controls();
}

void input_stick(float *x, float *y) { // -> acelerómetro (uikit.c)
    int tilt = opt_steer != STEER_TOUCH;
    *x = tilt ? stick_x : 0;
    *y = tilt ? stick_y : 0;
}

// Normalizado [0,1] en lo que se ve -> coordenadas de la vista (ver uv_rot en gles.c).
static void to_view(float nx, float ny, float *vx, float *vy) {
    switch (gles_rotation()) {
    case 1:
        *vx = (1 - ny) * fb_w, *vy = nx * fb_h;
        break;
    case 2:
        *vx = ny * fb_w, *vy = (1 - nx) * fb_h;
        break;
    default:
        *vx = nx * fb_w, *vy = ny * fb_h;
    }
}

static int screen_to_view(int sx, int sy, float *vx, float *vy) {
    int dx, dy, dw, dh;
    gles_dest_rect(&dx, &dy, &dw, &dh);
    float nx = (sx - dx) / (float)dw, ny = (sy - dy) / (float)dh;
    if (nx < 0 || nx > 1 || ny < 0 || ny > 1)
        return 0;
    to_view(nx, ny, vx, vy);
    return 1;
}

static void logical_size(float *w, float *h) {
    *w = gles_rotation() ? fb_h : fb_w;
    *h = gles_rotation() ? fb_w : fb_h;
}

void input_poll(void) {
    SceTouchData t;
    if (sceTouchPeek(SCE_TOUCH_PORT_FRONT, &t, 1) < 1)
        t.reportNum = 0;
    // dos dedos como máximo (los juegos de 2009 rara vez usan más)
    for (int k = 0; k < 2; k++) {
        int found = -1;
        for (int i = 0; i < (int)t.reportNum; i++)
            if (fingers[k].down && t.report[i].id == fingers[k].id)
                found = i;
        if (fingers[k].down && found < 0) {
            fingers[k].down = 0;
            ui_touch_event(k, 3, -1, -1);
        }
    }
    for (int i = 0; i < (int)t.reportNum && i < 2; i++) {
        float vx, vy;
        if (!screen_to_view(t.report[i].x / 2, t.report[i].y / 2, &vx, &vy))
            continue;
        int k = -1;
        for (int j = 0; j < 2; j++)
            if (fingers[j].down && fingers[j].id == t.report[i].id)
                k = j;
        if (k < 0) {
            for (int j = 0; j < 2 && k < 0; j++)
                if (!fingers[j].down)
                    k = j;
            if (k < 0)
                continue;
            fingers[k].down = 1;
            fingers[k].id = t.report[i].id;
            ui_touch_event(k, 0, vx, vy);
        } else {
            ui_touch_event(k, 1, vx, vy);
        }
    }

    SceCtrlData pad;
    sceCtrlPeekBufferPositive(0, &pad, 1);
    stick_x = (pad.lx - 128) / 127.0f;
    stick_y = (pad.ly - 128) / 127.0f;
    if (stick_x > -0.12f && stick_x < 0.12f)
        stick_x = 0;
    if (stick_y > -0.12f && stick_y < 0.12f)
        stick_y = 0;
    SceTouchData rear;
    if (sceTouchPeek(SCE_TOUCH_PORT_BACK, &rear, 1) < 1)
        rear.reportNum = 0;
    for (int i = 0; i < (int)rear.reportNum; i++) // panel trasero ~1920x(108..889)
        pad.buttons |= rear.report[i].x < 960 ? (rear.report[i].y < 498 ? REAR_TL : REAR_BL)
                                              : (rear.report[i].y < 498 ? REAR_TR : REAR_BR);
    uint32_t both = SCE_CTRL_SELECT | SCE_CTRL_START;
    static int combo; // SELECT+START apretados juntos desde el último soltar: no tocan nada
    if ((pad.buttons & both) == both)
        combo = 1;
    uint32_t b = pad.buttons, changed = b ^ prev_buttons, bound = 0;
    for (int i = 0; i < nbinds; i++)
        bound |= names[binds[i].name].mask;
    // Cruceta izquierda/derecha = inclinación digital si no está asignada a un toque ni al pad táctil (STICK)
    if (pad_r <= 0 && !stick_x && !(bound & (SCE_CTRL_LEFT | SCE_CTRL_RIGHT)))
        stick_x = !!(b & SCE_CTRL_RIGHT) - !!(b & SCE_CTRL_LEFT);
    for (int i = 0; i < nbinds; i++) {
        uint32_t mask = names[binds[i].name].mask;
        float lw, lh, vx, vy;
        logical_size(&lw, &lh);
        to_view(binds[i].x / lw, binds[i].y / lh, &vx, &vy);
        if (tap_up & (1u << i)) {
            tap_up &= ~(1u << i);
            ui_touch_event(BTN_FINGER0 + i, 3, vx, vy);
            continue;
        }
        if (!(changed & mask))
            continue;
        if (mask & both) { // al soltar, y solo si no fue parte de SELECT+START
            if (!(b & mask) && !combo)
                ui_touch_event(BTN_FINGER0 + i, 0, vx, vy), tap_up |= 1u << i;
            continue;
        }
        ui_touch_event(BTN_FINGER0 + i, (b & mask) ? 0 : 3, vx, vy);
    }
    if (!(b & both))
        combo = 0;
    if (pad_r > 0) {
        float px = stick_x, py = stick_y;
        int dx = !!(b & SCE_CTRL_RIGHT) - !!(b & SCE_CTRL_LEFT), dy = !!(b & SCE_CTRL_DOWN) - !!(b & SCE_CTRL_UP);
        if (px * px + py * py < 0.35f * 0.35f) // zona muerta grande: un dedo fantasma bloquea el toque real
            px = py = 0;
        if (dx || dy)
            px = dx * (dy ? 0.707f : 1), py = dy * (dx ? 0.707f : 1);
        int on = px != 0 || py != 0;
        if (on || pad_down) {
            float lw, lh, vx, vy;
            logical_size(&lw, &lh);
            to_view((pad_x + px * pad_r) / lw, (pad_y + py * pad_r) / lh, &vx, &vy);
            ui_touch_event(PAD_FINGER, on ? (pad_down ? 1 : 0) : 3, vx, vy);
            pad_down = on;
        }
    }
    // Control D del juego (tocar la mitad izquierda/derecha para girar): stick/cruceta -> dedo virtual, en carrera
    static int steer; // -1 izquierda, 1 derecha, 0 suelto
    if (pad_r <= 0 && (steer || opt_steer != STEER_TILT)) {
        int want = opt_steer != STEER_TILT && gles_in_race() ? (stick_x < -0.35f ? -1 : stick_x > 0.35f) : 0;
        if (want != steer) {
            float vx, vy;
            to_view((steer ? steer : want) < 0 ? 0.2f : 0.8f, 0.45f, &vx, &vy);
            ui_touch_event(PAD_FINGER, steer ? 3 : 0, vx, vy);
            steer = steer ? 0 : want; // cambiar de lado: soltar ahora, apoyar en el siguiente poll
        }
    }
    // SELECT+START: menú de opciones de video (options.c; ahí también va la rotación)
    if ((b & both) == both && (prev_buttons & both) != both)
        options_open();
    prev_buttons = b;
}

#pragma once
void input_init(void);
void input_poll(void);                  // lo llama ui_runloop_tick
void input_stick(float *x, float *y);   // stick izquierdo en [-1, 1] (acelerómetro)
void ui_touch_event(int finger, int phase, float x, float y); // uikit.c (0 began, 1 moved, 3 ended)
// Remapeo desde el menú de opciones (options.c): cada línea de controls.txt es una acción con su botón.
int input_bind_count(void);
const char *input_bind_label(int i);    // texto de la acción (4.ª columna de controls.txt) o el punto x,y
const char *input_bind_button(int i);
void input_bind_cycle(int i, int d);    // siguiente/anterior botón
void input_save_controls(void);

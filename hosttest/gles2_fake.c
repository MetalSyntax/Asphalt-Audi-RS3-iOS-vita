/* hosttest: ajustes GLES2 sobre el GL de escritorio de Mesa (las funciones GLES2 las da Mesa directamente). */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define HOSTTEST_REAL_GL
#include "vitaGL.h"

/* Antepone "#version 100" (GLSL ES 1.00) si el shader no declara versión. */
void host_glShaderSource(GLuint s, GLsizei n, const GLchar *const *src, const GLint *len)
{
    static int nsh;
    if (++nsh % 25 == 0) { fprintf(stderr, "[HOSTTEST] %d shaders\n", nsh); }
    const GLchar *v[65] = {"#version 100\n"};
    GLint l[65] = {-1};
    if (n <= 0 || n > 64 || !strncmp(src[0], "#version", 8) || (len && len[0] >= 0 && len[0] < 8)) {
        glShaderSource(s, n, src, len);
        return;
    }
    for (int i = 0; i < n; i++)
        v[i + 1] = src[i], l[i + 1] = len ? len[i] : -1;
    /* Glitch's shader files (RS3: file000351.dat) contain "#endif]": the PowerVR compiler ignores the junk,
       Mesa's preprocessor errors out ("unexpected ']', expecting NEWLINE"). Blank any junk after #endif/#else. */
    static GLchar *clean[65];
    for (int i = 1; i <= n; i++) {
        size_t sz = l[i] >= 0 ? (size_t)l[i] : strlen(v[i]);
        free(clean[i]);
        clean[i] = malloc(sz + 1);
        memcpy(clean[i], v[i], sz);
        clean[i][sz] = 0;
        for (char *p = clean[i]; (p = strstr(p, "#endif")) || 0; p += 6)
            for (char *q = p + 6; *q && *q != '\n' && *q != '\r'; q++)
                if (*q == ']') *q = ' ';
        v[i] = clean[i];
        l[i] = (GLint)sz;
    }
    glShaderSource(s, n + 1, v, l);
}

/* Límites como los de vitaGL (get_info.c), no los de Mesa de escritorio: el motor (Glitch) dimensiona
   sus permutaciones de shaders con ellos. GL_MAX_SAMPLES no existe en vitaGL (error, *v sin tocar). */
void host_glGetIntegerv(GLenum p, GLint *v)
{
    switch (p) {
    case 0x8872: case 0x8B4C: case 0x8B4D: /* MAX_TEXTURE_IMAGE_UNITS, MAX_VERTEX/COMBINED_TEXTURE_IMAGE_UNITS */
    case 0x8869: *v = 16; return;          /* MAX_VERTEX_ATTRIBS */
    case 0x8DFB: *v = 128; return;         /* MAX_VERTEX_UNIFORM_VECTORS */
    case 0x8DFD: *v = 16; return;          /* MAX_FRAGMENT_UNIFORM_VECTORS */
    case 0x8DFC: *v = 8; return;           /* MAX_VARYING_VECTORS */
    case 0x84E2: case 0x8871: *v = 2; return; /* MAX_TEXTURE_UNITS, MAX_TEXTURE_COORDS */
    case 0x8D57: return;                   /* MAX_SAMPLES */
    }
    glGetIntegerv(p, v);
}

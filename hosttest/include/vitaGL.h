/* hosttest: lo de vitaGL.h que usa el loader, sobre el GLES 1.1 real de Linux
 * (Mesa). Las funciones vgl* y los nombres sin sufijo OES de FBO, que vitaGL
 * expone y GLES 1.1 no, están en vitagl_fake.c. */
#ifndef HOSTTEST_VITAGL_H
#define HOSTTEST_VITAGL_H
#include <GLES/gl.h>
#define GL_GLEXT_PROTOTYPES
#define VGL_YUV420P_NV12_BT601 0x18E70
#include <GLES/glext.h>
#include <GLES2/gl2.h> /* el runtime maestro también trae la ruta GLES2 (Gangstar/Six Guns) */
void *glMapBuffer(GLenum target, GLenum access);
GLboolean glUnmapBuffer(GLenum target);
#include <stddef.h>

#ifndef SCE_GXM_MULTISAMPLE_NONE
#define SCE_GXM_MULTISAMPLE_NONE 0
#define SCE_GXM_MULTISAMPLE_2X 1
#define SCE_GXM_MULTISAMPLE_4X 2
/* GLES2 de vitaGL que la tabla de gles.c exporta (Feanor no los usa): stubs en gles2_fake.c */
void glBlendEquation(GLenum m);
void glBlendFuncSeparate(GLenum a, GLenum b, GLenum c, GLenum d);
void glVertexAttribPointer(GLuint i, GLint size, GLenum type, GLboolean norm, GLsizei stride, const void *p);
void glEnableVertexAttribArray(GLuint i);
void glDisableVertexAttribArray(GLuint i);
void glVertexAttrib4f(GLuint i, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void glDeleteProgram(GLuint p);
void glGenerateMipmap(GLenum t);
#endif
#define GL_FRAMEBUFFER          GL_FRAMEBUFFER_OES
#define GL_RENDERBUFFER         GL_RENDERBUFFER_OES
#define GL_COLOR_ATTACHMENT0    GL_COLOR_ATTACHMENT0_OES
#define GL_DEPTH_ATTACHMENT     GL_DEPTH_ATTACHMENT_OES
#define GL_STENCIL_ATTACHMENT   GL_STENCIL_ATTACHMENT_OES
#define GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_OES
#define GL_DEPTH_COMPONENT16    GL_DEPTH_COMPONENT16_OES
#define GL_RGBA8                GL_RGBA8_OES
#define GL_FRAMEBUFFER_BINDING  GL_FRAMEBUFFER_BINDING_OES
#define GL_DEPTH24_STENCIL8     GL_DEPTH24_STENCIL8_OES
#define GL_DEPTH_STENCIL_ATTACHMENT GL_DEPTH_ATTACHMENT_OES /* Mesa GLES1: depth only, enough here */

typedef enum { VGL_MEM_VRAM, VGL_MEM_RAM, VGL_MEM_PHYCONT, VGL_MEM_SLOW, VGL_MEM_BUDGET, VGL_MEM_EXTERNAL, VGL_MEM_ALL } vglMemType;

GLboolean vglInitExtended(int legacy_pool_size, int width, int height, int ram_threshold, int msaa);
GLboolean vglInitWithCustomThreshold(int pool_size, int width, int height, int ram_threshold, int cdram_threshold,
                                     int phycont_threshold, int cdlg_threshold, int msaa);
void vglSwapBuffers(GLboolean has_commondialog);
void vglUseTripleBuffering(GLboolean usage);
#define VGL_TYPE_COLOR 3
void vglSetShaderCachePath(const char *path);
void vglAddSemanticBindingHint(const GLchar *const *varying, GLenum type);
size_t vglMemFree(vglMemType type);

void glGenFramebuffers(GLsizei n, GLuint *ids);
void glDeleteFramebuffers(GLsizei n, const GLuint *ids);
void glBindFramebuffer(GLenum target, GLuint fb);
void glFramebufferTexture2D(GLenum target, GLenum att, GLenum textarget, GLuint tex, GLint level);
void glFramebufferRenderbuffer(GLenum target, GLenum att, GLenum rbtarget, GLuint rb);
GLenum glCheckFramebufferStatus(GLenum target);
void glGetFramebufferAttachmentParameteriv(GLenum target, GLenum att, GLenum pname, GLint *params);
void glGenRenderbuffers(GLsizei n, GLuint *ids);
void glDeleteRenderbuffers(GLsizei n, const GLuint *ids);
void glBindRenderbuffer(GLenum target, GLuint rb);
void glRenderbufferStorage(GLenum target, GLenum fmt, GLsizei w, GLsizei h);
void glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint *params);

/* GLES2 de vitaGL que la tabla de gles.c exporta (Feanor no los usa): stubs en gles2_fake.c */
void glBlendEquation(GLenum m);
void glBlendFuncSeparate(GLenum a, GLenum b, GLenum c, GLenum d);
void glVertexAttribPointer(GLuint i, GLint size, GLenum type, GLboolean norm, GLsizei stride, const void *p);
void glEnableVertexAttribArray(GLuint i);
void glDisableVertexAttribArray(GLuint i);
void glVertexAttrib4f(GLuint i, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void glDeleteProgram(GLuint p);
void glGenerateMipmap(GLenum t);
/* GLSL ES sin #version: Mesa (GL de escritorio) lo toma como GLSL 1.10 y rechaza precision/lowp (gles2_fake.c) */
#ifndef HOSTTEST_REAL_GL
void host_glShaderSource(GLuint s, GLsizei n, const GLchar *const *src, const GLint *len);
#define glShaderSource host_glShaderSource
void host_glGetIntegerv(GLenum p, GLint *v);
#define glGetIntegerv host_glGetIntegerv
#endif
#endif

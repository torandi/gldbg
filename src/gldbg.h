#ifndef GLDBG_H
#define GLDBG_H

#include "gl.inc.h"

typedef unsigned char GLubyte ;

typedef void* (*GL_X_GET_PROC_ADDRESS_FUNC) (const GLubyte*);

#define EXPORT __attribute__ ((visibility ("default")))

extern void (*__real_SDL_GL_SwapBuffers)(void);
extern void (*__real_SDL_Quit)(void);
extern GL_X_GET_PROC_ADDRESS_FUNC __real_glXGetProcAddressARB;
extern GL_X_GET_PROC_ADDRESS_FUNC __real_glXGetProcAddress;

void __gldbg_init();
void __gldbg_finish();

void __attribute__((format(printf, 1,2))) __gldbg_printf(const char* fmt, ...);
void __attribute__((format(printf, 1,2))) __gldbg_log(const char* fmt, ...);

#endif

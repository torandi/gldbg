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

enum __output_t { OUT_PRINT = 1 , OUT_LOG = 2, OUT_BOTH = 3 };

void __attribute__((format(printf, 1,2))) __gldbg_printf(const char* fmt, ...);
void __attribute__((format(printf, 2,3))) __gldbg_out(enum __output_t output_target, const char* fmt, ...);

#endif

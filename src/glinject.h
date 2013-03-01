#ifndef GLINJECT_H
#define GLINJECT_H

#include "gl.inc.h"

/* Handles the injection into the opengl library */

struct __gl_func_t {
	const char * name;
	void * func;
	void * func_arb;
	void * real_func;
	void * real_func_arb;
};

extern struct __gl_func_t __functions[];

/* Defined functions */
enum {
	GLDBG_GL_GEN_BUFFERS = 0,
	GLDBG_GL_DELETE_BUFFERS,
	GLDBG_GL_BIND_BUFFER,
};


#endif

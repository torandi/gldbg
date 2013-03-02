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
	void ** internal_func; /* A pointer to set to the real_func, for internal use */
};

extern struct __gl_func_t __functions[];

/* Defined functions */
enum {
	GLDBG_GL_GEN_BUFFERS = 0,
	GLDBG_GL_DELETE_BUFFERS,
	GLDBG_GL_BIND_BUFFER,
	GLDBG_GL_BUFFER_DATA,
};

/* Typedefs of the different function signatures */

typedef void (*GL_GEN_BUFFERS_FUNC) (GLsizei, GLuint*);
typedef void (*GL_BIND_BUFFER_FUNC) (GLenum, GLuint);
typedef void (*GL_BUFFER_DATA_FUNC) (GLenum, GLsizeiptr, const GLvoid *, GLenum);


/* Pointers to the real functions we which to use internaly */

extern GL_BIND_BUFFER_FUNC __real_glBindBuffer;

#endif

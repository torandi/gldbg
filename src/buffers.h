#ifndef BUFFERS_H
#define BUFFERS_H

#include "gl.inc.h"

#include "gldbg.h"

enum __data_type_t { GLDBG_BUFFER_FLOAT = 0, GLDBG_BUFFER_INT = 1 };

struct __buffer_type_t {
	enum __data_type_t data_type;
	int group_size;
};

struct __gl_buffer_t {
	GLuint name;
	GLenum target;
	GLint size;
	struct __buffer_type_t type;
	unsigned char valid;
};

extern struct __gl_buffer_t ** __buffers;
extern unsigned int __num_buffers;

struct __gl_buffer_t * __find_buffer(GLuint name);

void __free_buffers();

typedef void (*GL_GEN_BUFFERS_FUNC) (GLsizei, GLuint*);
typedef void (*GL_BIND_BUFFER_FUNC) (GLenum, GLuint);

EXPORT void __glGenBuffers(GLsizei n, GLuint * buffers);
EXPORT void __glGenBuffersARB(GLsizei n, GLuint * buffers);

EXPORT void __glDeleteBuffers(GLsizei n, GLuint * buffers);
EXPORT void __glDeleteBuffersARB(GLsizei n, GLuint * buffers);

EXPORT void __glBindBuffer(GLenum target, GLuint buffer);
EXPORT void __glBindBufferARB(GLenum target, GLuint buffer);

#endif

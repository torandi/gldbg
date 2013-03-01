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
	char * str;
};

extern struct __gl_buffer_t * __buffers;

extern unsigned int __num_buffers;
extern unsigned int __allocated_buffers;

void __free_buffers();

EXPORT void __glBindBuffer(GLenum target, GLuint buffer);
EXPORT void __glBindBufferARB(GLenum target, GLuint buffer);

#endif

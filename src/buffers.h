#ifndef BUFFERS_H
#define BUFFERS_H

#include "gl.inc.h"

#include "gldbg.h"

enum __data_type_t { FLOAT = 1, INT = 2 };

struct __buffer_type_t {
	enum __data_type_t data_type;
	int group_size;
	enum __output_t output;
};

struct __gl_buffer_t {
	GLuint name;
	GLenum target;
	GLint size;
	struct __buffer_type_t type;
	unsigned char valid;
	void * data;
};

extern struct __gl_buffer_t ** __buffers;
extern unsigned int __num_buffers;

struct __gl_buffer_t * __find_buffer(GLuint name);

/* 
 * Updates the data in the buffer's data-pointer
 * Returns 1 on success, 0 on failure
 */
int __read_buffer(struct __gl_buffer_t * buffer);

void __log_buffer(struct __gl_buffer_t * buffer, enum __output_t output_target);

void __free_buffers();

/* Returns an index for the target, used for data lookup */
int __target_index(GLenum target);
extern const char * __target_names[];

EXPORT void __glGenBuffers(GLsizei n, GLuint * buffers);
EXPORT void __glGenBuffersARB(GLsizei n, GLuint * buffers);

EXPORT void __glDeleteBuffers(GLsizei n, GLuint * buffers);
EXPORT void __glDeleteBuffersARB(GLsizei n, GLuint * buffers);

EXPORT void __glBindBuffer(GLenum target, GLuint buffer);
EXPORT void __glBindBufferARB(GLenum target, GLuint buffer);

EXPORT void __glBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
EXPORT void __glBufferDataARB(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);

#endif

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "glinject.h"



/* Internal versions of the overriden functions */
static void __glGenBuffersINT(GL_GEN_BUFFERS_FUNC real_func, GLsizei n, GLuint * buffers);
static void __glDeleteBuffersINT(GL_GEN_BUFFERS_FUNC real_func, GLsizei n, GLuint * buffers);
static void __glBindBufferINT(GL_BIND_BUFFER_FUNC real_func, GLenum target, GLuint buffer);

#include "buffers.func.h"

struct __gl_buffer_t ** __buffers = NULL;

unsigned int __num_buffers = 0; 

static void __alloc_buffers(GLsizei n, GLuint * buffers);

/* Returns the bound buffer on given target, or -1 if the target is not handled */
static GLint __bound_buffer(GLenum target);

/* Returns an index for the target, used for data lookup */
static int __target_index(GLenum target);

/* 
 * target data, indexed by index given by __target_index. 
 */

static GLenum __target_get_enums[] = {
	GL_ARRAY_BUFFER_BINDING,
	GL_ATOMIC_COUNTER_BUFFER_BINDING,
	GL_ELEMENT_ARRAY_BUFFER_BINDING,
	GL_TRANSFORM_FEEDBACK_BUFFER_BINDING,
	GL_UNIFORM_BUFFER_BINDING,
	[5 ... 11] = 0,
};

static const char * __target_names[] = {
	"GL_ARRAY_BUFFER",
	"GL_ATOMIC_COUNTER_BUFFER",
	"GL_COPY_READ_BUFFER",
	"GL_COPY_WRITE_BUFFER",
	"GL_ELEMENT_ARRAY_BUFFER",
	"GL_TRANSFORM_FEEDBACK_BUFFER",
	"GL_UNIFORM_BUFFER",
	"GL_DRAW_INDIRECT_BUFFER",
	"GL_PIXEL_PACK_BUFFER",
	"GL_PIXEL_UNPACK_BUFFER",
	"GL_TEXTURE_BUFFER",
	"UNKNOWN_BUFFER_ENUM",
};

static void __glGenBuffersINT(GL_GEN_BUFFERS_FUNC real_func, GLsizei n, GLuint * buffers) {
	__gldbg_printf("glGenBuffers()\n");

	real_func(n, buffers);

	__alloc_buffers(n, buffers);
}

static void __glDeleteBuffersINT(GL_GEN_BUFFERS_FUNC real_func, GLsizei n, GLuint * buffers) {
	__gldbg_printf("glDeleteBuffers()\n");

	struct __gl_buffer_t * buffer;
	for(int i = 0; i < n; ++i) {
		buffer = __find_buffer(buffers[i]);
		if(buffer != NULL) buffer->valid = 0;
	}

	real_func(n, buffers);

}

static void __glBindBufferINT(GL_BIND_BUFFER_FUNC real_func, GLenum target, GLuint buffer) {
	real_func(target, buffer);

	if(buffer == 0) return;

	if(glIsBuffer(buffer) == GL_TRUE) {
		struct __gl_buffer_t * buffer_obj = __find_buffer(buffer);
		if(buffer_obj != NULL) {
			if(buffer_obj->target == target) return;

			if(buffer_obj->target != 0) {
				__gldbg_printf("Buffer %u changed binding target from %s to %s\n", 
						buffer,
						__target_names[__target_index(buffer_obj->target)],
						__target_names[__target_index(target)]
					);
			} else {
				__gldbg_printf("Buffer %u bound to target %s\n", 
						buffer,
						__target_names[__target_index(target)]
					);
			}

			buffer_obj->target = target;
		}
	}
}

//		glGetBufferParameteriv(target, GL_BUFFER_SIZE, &(buffer->size));

void __free_buffers() {
	for(unsigned int i = 0; i < __num_buffers; ++i) {
		if(__buffers[i] != NULL) free(__buffers[i]);
	}
	free(__buffers);
	__buffers = NULL;
}


static void __alloc_buffers(GLsizei n, GLuint * buffers) {

	/* Find highest id for the buffers */
	GLuint max_id = 0;
	for(int i = 0; i< n; ++i) {
		max_id = buffers[i] > max_id ? buffers[i] : max_id;
	}
	++max_id; /* And the list needs to be one longer than that */

	if(__buffers  == NULL) {
		__buffers  = (struct __gl_buffer_t**) calloc( sizeof(struct __gl_buffer_t**), max_id );

	} else if(max_id >= __num_buffers) {
		__buffers = (struct __gl_buffer_t**) realloc( (void*)__buffers, sizeof(struct __gl_buffer_t**) * max_id );

		memset(__buffers + __num_buffers, 0, (max_id - __num_buffers) * sizeof(struct __gl_buffer_t**));
	}

	__num_buffers = max_id;

	for(unsigned int i = 0; i< n; ++i) {
		struct __gl_buffer_t buffer = {
			.name = buffers[i],
			.target = 0,
			.size = 0,
			.type = { GLDBG_BUFFER_FLOAT, 1 },
			.valid = 1,
		};

		__buffers[i] = (struct __gl_buffer_t*) malloc(sizeof(struct __gl_buffer_t));

		memcpy(__buffers[i], &buffer, sizeof(struct __gl_buffer_t));

		__gldbg_printf("Generated buffer %u\n", buffers[i]);
	}
}

struct __gl_buffer_t * __find_buffer(GLuint name) {
	if(name >= __num_buffers) {
		__gldbg_printf("Internal error: Accessing buffer (%u) outside array (size: %u)\n", name, __num_buffers);
		return NULL;
	}
	return __buffers[name];
}

static GLint __bound_buffer(GLenum target) {
	GLenum get_enum = __target_get_enums[__target_index(target)];

	if(get_enum == 0) {
		return -1;
	}

	GLint bound_buffer;

	glGetIntegerv(get_enum, &bound_buffer);

	return bound_buffer;
}

static int __target_index(GLenum target) {
		switch(target) {
		case GL_ARRAY_BUFFER: return 0;
		case GL_ATOMIC_COUNTER_BUFFER: return 1;
		case GL_COPY_READ_BUFFER: return 2;
		case GL_COPY_WRITE_BUFFER: return 3;
		case GL_ELEMENT_ARRAY_BUFFER: return 4;
		case GL_TRANSFORM_FEEDBACK_BUFFER: return 5;
		case GL_UNIFORM_BUFFER: return 6;
		case GL_DRAW_INDIRECT_BUFFER: return 7;
		case GL_PIXEL_PACK_BUFFER: return 8;
		case GL_PIXEL_UNPACK_BUFFER: return 9;
		case GL_TEXTURE_BUFFER: return 10;
		default: return 11;
	}
}

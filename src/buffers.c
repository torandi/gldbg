#include <GL/glew.h>

#include "buffers.h"
#include "glinject.h"

#include <stdio.h>
#include <stdlib.h>


/* Internal versions of the overriden functions */
static void __glGenBuffersINT(GL_GEN_BUFFERS_FUNC real_func, GLsizei n, GLuint * buffers);
static void __glDeleteBuffersINT(GL_GEN_BUFFERS_FUNC real_func, GLsizei n, GLuint * buffers);
static void __glBindBufferINT(GL_BIND_BUFFER_FUNC real_func, GLenum target, GLuint buffer);

#include "buffers.func.h"

struct __gl_buffer_t * __buffers = NULL;

/* A list of pointers for quick lookup of a given id */
static struct __gl_buffer_t **__buffers_jumplist = NULL;
static unsigned int __buffers_jumplist_size = 0;


unsigned int __num_buffers = 0; 
static unsigned int __allocated_buffers = 0;

static void __alloc_buffers(GLsizei n, GLuint * buffers);

/* Returns an index for the target, used for data lookup */
static int __target_index(GLenum target);

/* 
 * target data, indexed by index given by __target_index. 
 */

/* There are 12 different types of targets*/
static GLuint __bound_buffer[12] = {0, };

static GLenum __target_get_enums[] = {
	GL_ARRAY_BUFFER_BINDING,
	GL_ATOMIC_COUNTER_BUFFER_BINDING,
	GL_ELEMENT_ARRAY_BUFFER_BINDING,
	GL_TRANSFORM_FEEDBACK_BUFFER_BINDING,
	GL_UNIFORM_BUFFER_BINDING,
	[5 ... 12] = 0,
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
	__gldbg_init();

	__gldbg_printf("glGenBuffers()\n");

	real_func(n, buffers);

	__alloc_buffers(n, buffers);
}

static void __glDeleteBuffersINT(GL_GEN_BUFFERS_FUNC real_func, GLsizei n, GLuint * buffers) {
	__gldbg_init();

	__gldbg_printf("glDeleteBuffers()\n");

	real_func(n, buffers);

	struct __gl_buffer_t * buffer;
	for(int i = 0; i < n; ++i) {
		buffer = __find_buffer(buffers[i]);
		if(buffer != NULL) buffer->valid = 0;
	}
}

static void __glBindBufferINT(GL_BIND_BUFFER_FUNC real_func, GLenum target, GLuint buffer) {
	__gldbg_init();

	real_func(target, buffer);

	if(buffer == 0) return;

	GLenum get_enum = __target_get_enums[__target_index(target)];

	if(get_enum == 0) {
		return;
	}

	GLint bound_buffer;

	glGetIntegerv(get_enum, &bound_buffer);

	if(glIsBuffer(buffer) == GL_TRUE && ((GLuint)bound_buffer == buffer)) {
		struct __gl_buffer_t * buffer_obj = __find_buffer(buffer);
		if(buffer_obj != NULL) {
			if(buffer_obj->target == target) return;

			if(buffer_obj->target != 0) {
				__gldbg_printf("Buffer %u changed binding target from %s to %s\n", 
						buffer,
						__target_names[__target_index(buffer_obj->target)],
						__target_names[__target_index(target)]
					);
			}

			buffer_obj->target = target;
		}
	}
}

//		glGetBufferParameteriv(target, GL_BUFFER_SIZE, &(buffer->size));

void __free_buffers() {
	free(__buffers);
	free(__buffers_jumplist);
	__allocated_buffers = 0;
	__buffers = NULL;
	__buffers_jumplist = NULL;
}


static void __alloc_buffers(GLsizei n, GLuint * buffers) {
	if(__buffers == NULL) {
		__allocated_buffers = (unsigned int)n * 2;
		__buffers = (struct __gl_buffer_t*) malloc(sizeof(struct __gl_buffer_t) * __allocated_buffers);
	}

	unsigned int first_id = __num_buffers;

	__num_buffers += (unsigned int)n;
	if(__num_buffers >= __allocated_buffers) {
		while(__num_buffers >= __allocated_buffers) {
			__allocated_buffers *= 2;
		}

		__buffers = (struct __gl_buffer_t*) realloc((void*)__buffers, sizeof(struct __gl_buffer_t) * __allocated_buffers);
	}

	GLuint max_id = 0;
	for(int i = 0; i< n; ++i) {
		max_id = buffers[i] > max_id ? buffers[i] : max_id;
	}

	if(__buffers_jumplist == NULL) {
		__buffers_jumplist = (struct __gl_buffer_t**) calloc( sizeof(struct __gl_buffer_t*), (max_id + 1) );
	} else if(max_id >= __buffers_jumplist_size) {
		__buffers_jumplist = (struct __gl_buffer_t**) realloc( (void*)__buffers_jumplist, sizeof(struct __gl_buffer_t*) * (max_id + 1) );
		for(unsigned int i=__buffers_jumplist_size; i < ( max_id + 1) ; ++i) {
			__buffers_jumplist[i] = NULL;
		}
	}
	__buffers_jumplist_size = max_id + 1;

	for(unsigned int i = 0; i< n; ++i) {
		__buffers[first_id + i].name = buffers[i];
		__buffers[first_id + i].target = 0;
		__buffers[first_id + i].size = 0;
		__buffers[first_id + i].type.data_type = GLDBG_BUFFER_FLOAT;
		__buffers[first_id + i].type.group_size = 1;
		__buffers[first_id + i].valid = 1;
		__buffers_jumplist[buffers[i]] = __buffers + first_id + i;
	}
}

struct __gl_buffer_t * __find_buffer(GLuint name) {
	if(name >= __buffers_jumplist_size) {
		__gldbg_printf("Internal error: Accessing buffer (%u) outside jumplist (size: %u)\n", name, __buffers_jumplist_size);
		return NULL;
	}
	return __buffers_jumplist[name];
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
		case GL_PIXEL_UNPACK_BUFFER: return 10;
		case GL_TEXTURE_BUFFER: return 11;
		default: return 12;
	}
}

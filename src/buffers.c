#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffers.h"
#include "glinject.h"
#include "config.h"



/* Internal versions of the overriden functions */
static void __glGenBuffersINT(GL_GEN_BUFFERS_FUNC real_func, GLsizei n, GLuint * buffers);
static void __glDeleteBuffersINT(GL_GEN_BUFFERS_FUNC real_func, GLsizei n, GLuint * buffers);
static void __glBindBufferINT(GL_BIND_BUFFER_FUNC real_func, GLenum target, GLuint buffer);
static void __glBufferDataINT(GL_BUFFER_DATA_FUNC real_func, GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);

#include "buffers.func.h"

struct __gl_buffer_t ** __buffers = NULL;

unsigned int __num_buffers = 0; 

static void __alloc_buffers(GLsizei n, GLuint * buffers);

/* Returns the bound buffer on given target, or -1 if the target is not handled */
static GLint __bound_buffer(GLenum target);

/* 
 * target data, indexed by index given by __target_index. 
 */

static const GLenum __target_get_enums[] = {
	GL_ARRAY_BUFFER_BINDING,
	GL_ATOMIC_COUNTER_BUFFER_BINDING,
	GL_ELEMENT_ARRAY_BUFFER_BINDING,
	GL_TRANSFORM_FEEDBACK_BUFFER_BINDING,
	GL_UNIFORM_BUFFER_BINDING,
	[5 ... 11] = 0,
};

const char * __target_names[] = {
	"GL_ARRAY_BUFFER",
	"GL_ATOMIC_COUNTER_BUFFER",
	"GL_ELEMENT_ARRAY_BUFFER",
	"GL_TRANSFORM_FEEDBACK_BUFFER",
	"GL_UNIFORM_BUFFER",
	"GL_COPY_READ_BUFFER",
	"GL_COPY_WRITE_BUFFER",
	"GL_DRAW_INDIRECT_BUFFER",
	"GL_PIXEL_PACK_BUFFER",
	"GL_PIXEL_UNPACK_BUFFER",
	"GL_TEXTURE_BUFFER",
	"UNKNOWN_BUFFER_ENUM",
};

static void __glGenBuffersINT(GL_GEN_BUFFERS_FUNC real_func, GLsizei n, GLuint * buffers) {
	real_func(n, buffers);

	__alloc_buffers(n, buffers);
}

static void __glDeleteBuffersINT(GL_GEN_BUFFERS_FUNC real_func, GLsizei n, GLuint * buffers) {
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
			__configure_buffer(buffer_obj);
		}
	}
}

static void __glBufferDataINT(GL_BUFFER_DATA_FUNC real_func, GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage) {
	real_func(target, size, data, usage);

	GLint buffer_index = __bound_buffer(target);

	if(buffer_index == 0) {
		__gldbg_printf("Warning! Buffering data to target %s with buffer 0 bound!\n", __target_names[__target_index(target)]);
		return;
	}

	if(buffer_index == -1) {
		return; /* We don't handle this buffer type */
	}

	struct __gl_buffer_t * buffer = __find_buffer((GLuint)buffer_index);

	if(buffer == NULL) {
		__gldbg_printf("Internal error! No internal data found for buffer %u bound to target %s\n", buffer_index, __target_names[__target_index(target)]);
		abort();
		return;
	}

	glGetBufferParameteriv(target, GL_BUFFER_SIZE, &(buffer->size));
	__gldbg_printf("Size for buffer %u (%s) set to %d\n", buffer_index, __target_names[__target_index(target)], buffer->size);

	/* Allocate data buffer */
	if(buffer->data != NULL) free(buffer->data);
	if(buffer->size > 0) {
		buffer->data = malloc((size_t)buffer->size);
	} else {
		buffer->data = NULL;
	}
}

void __free_buffers() {
	for(unsigned int i = 0; i < __num_buffers; ++i) {
		if(__buffers[i] != NULL) {
			if(__buffers[i]->data != NULL) free(__buffers[i]->data);
			free(__buffers[i]);
		}
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
		GLuint index = buffers[i];
		struct __gl_buffer_t buffer = {
			.name = index,
			.target = 0,
			.size = 0,
			.type = __default_buffer_type,
			.valid = 1,
			.data = NULL,
		};

		__buffers[index] = (struct __gl_buffer_t*) malloc(sizeof(struct __gl_buffer_t));

		memcpy(__buffers[index], &buffer, sizeof(struct __gl_buffer_t));

		__gldbg_printf("Generated buffer: %u\n", index);
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

int __read_buffer(struct __gl_buffer_t * buffer) {
	if(buffer->valid == 0) {
		__gldbg_printf("Internal error: Trying to read a deleted buffer (%u)\n", buffer->name);
		return 0;
	}

	if(buffer->target == 0) {
		__gldbg_printf("Internal error: Trying to read a buffer (%u) with no target set\n", buffer->name);
		return 0;
	}

	int index = __target_index(buffer->target);

	GLint current = __bound_buffer(buffer->target);
	if(current == -1) {
		__gldbg_printf("Internal error: Trying to read a buffer (%u) bound to unhandled target %s\n", buffer->name, __target_names[index]);
		return 0;
	}

	if(buffer->data == NULL) {
		__gldbg_printf("Internal error: Trying to read a buffer (%u - %s) of size 0\n", buffer->name, __target_names[index]);
		return 0;
	}

	if(current != buffer->name) __real_glBindBuffer(buffer->target, buffer->name);

	glGetBufferSubData(buffer->target, 0, buffer->size, buffer->data);

	if( current != buffer->name) __real_glBindBuffer(buffer->target, (GLuint) current); /* Restore original bound buffer */

	return 1;
}

void __log_buffer(struct __gl_buffer_t * buffer, enum __output_t output_target) {
	output_target &= buffer->type.output;
	if(output_target == 0) return;
	__gldbg_out(output_target, "%u (%s):\n", buffer->name, __target_names[__target_index(buffer->target)]);
	int count = 0;
	GLint size = 0; /* Size of the data type used to interprent the buffer */
	switch(buffer->type.data_type) {
		case FLOAT: 
			size = sizeof(float);
			break;
		case INT: 
			size = sizeof(int);
			break;
	}

	int values = buffer->size / size;

	if(buffer->size % size != 0) {
		__gldbg_out(output_target, "Warning! Data type size does not evenly divide buffer size!\n");
	}


	for(int i=0;i < values; ++i) {
		switch(buffer->type.data_type) {
			case FLOAT:
				__gldbg_out(output_target, "%f\t",  ((float*)buffer->data)[i]);
				break;
			case INT:
				__gldbg_out(output_target, "%d\t",  ((int*)buffer->data)[i]);
				break;
		}
		++count;
		if(count % buffer->type.group_size == 0) __gldbg_out(output_target, "\n");
	}

	if(count % buffer->type.group_size != 0) __gldbg_out(output_target, "\n");

	__gldbg_out(output_target, "\n");
}

int __target_index(GLenum target) {
		switch(target) {
		case GL_ARRAY_BUFFER: return 0;
		case GL_ATOMIC_COUNTER_BUFFER: return 1;
		case GL_ELEMENT_ARRAY_BUFFER: return 2;
		case GL_TRANSFORM_FEEDBACK_BUFFER: return 3;
		case GL_UNIFORM_BUFFER: return 4;
		case GL_COPY_READ_BUFFER: return 5;
		case GL_COPY_WRITE_BUFFER: return 6;
		case GL_DRAW_INDIRECT_BUFFER: return 7;
		case GL_PIXEL_PACK_BUFFER: return 8;
		case GL_PIXEL_UNPACK_BUFFER: return 9;
		case GL_TEXTURE_BUFFER: return 10;
		default: return 11;
	}
}

#include <GL/glew.h>

#include "buffers.h"
#include "glinject.h"

#include <stdio.h>
#include <stdlib.h>

/* Internal versions of the overriden functions */
static void __glBindBufferINT(GL_BIND_BUFFER_FUNC real_func, GLenum target, GLuint buffer);

/* Exported opengl override functions */
EXPORT void __glBindBuffer(GLenum target, GLuint buffer) {
	__glBindBufferINT(__functions[GLDBG_GL_BIND_BUFFER].real_func, target, buffer);
}

EXPORT void __glBindBufferARB(GLenum target, GLuint buffer) {
	__glBindBufferINT(__functions[GLDBG_GL_BIND_BUFFER].real_func_arb,target, buffer);
}

/* End opengl functions */

struct __gl_buffer_t * __buffers = NULL;

unsigned int __num_buffers = 0; 
unsigned int __allocated_buffers = 0;

static void __add_buffer(GLenum target, GLuint buffer);
static GLenum __target_get_enum(GLenum target);
static struct __gl_buffer_t * __alloc_buffer();
static struct __gl_buffer_t * __find_buffer(GLuint name);
static const char * __buffer_enum_to_string(GLenum buffer_enum);

static void __glBindBufferINT(GL_BIND_BUFFER_FUNC real_func, GLenum target, GLuint buffer) {
	__gldbg_init();

	__gldbg_printf("glBindBuffer()\n");

	real_func(target, buffer);

	GLenum get_enum = __target_get_enum(target);

	if(get_enum == 0) {
		return;
	}

	GLint bound_buffer;

	glGetIntegerv(get_enum, &bound_buffer);

	if(glIsBuffer(buffer) == GL_FALSE) {
		return;
	} else if((GLuint)bound_buffer != buffer) {
		return;
	} else {
		__add_buffer(target, buffer);
	}
}

void __free_buffers() {
	for(int i=0; i < __num_buffers; ++i) {
		free(__buffers[i].str);
	}
	free(__buffers);
	__allocated_buffers = 0;
	__buffers = NULL;
}

static void __add_buffer(GLenum target, GLuint name) {
	struct __gl_buffer_t * buffer = __alloc_buffer();
	buffer->name = name;
	buffer->target = target;
	buffer->size = 0;
	buffer->valid = 1;

	glGetBufferParameteriv(target, GL_BUFFER_SIZE, &(buffer->size));

	const char * enum_name = __buffer_enum_to_string(target);

	if(asprintf(&(buffer->str), "%u (%s)", name, enum_name) == -1) {
		fprintf(stderr,"[GLDBG] Critical error: Failed to allocate memory for buffer name string\n");
		abort();
	}

	__gldbg_printf("Tracking buffer %s.\n", buffer->str);
}

static struct __gl_buffer_t * __alloc_buffer() {
	if(__buffers == NULL) {
		__allocated_buffers = 2;
		__buffers = (struct __gl_buffer_t*) malloc(sizeof(struct __gl_buffer_t) * __allocated_buffers);
	}

	unsigned int id = __num_buffers++;
	if(id >= __allocated_buffers) {
		__allocated_buffers *= 2;
		__buffers = (struct __gl_buffer_t*) realloc((void*)__buffers, sizeof(struct __gl_buffer_t) * __allocated_buffers);
	}

	return __buffers + id;
}

static struct __gl_buffer_t * __find_buffer(GLuint name) {
	for(int i = 0; i < __num_buffers; ++i) {
		if(__buffers[i].name == name) return __buffers + i;
	}
	return NULL;
}

/*
 * Finds the correct enum for doing glGet to find the currently bound buffer for a given target
 * @return the correct enum or 0 if the target is not handled
 */
static GLenum __target_get_enum(GLenum target) {
	switch(target) {
		case GL_ARRAY_BUFFER: return GL_ARRAY_BUFFER_BINDING;
		case GL_ATOMIC_COUNTER_BUFFER: return GL_ATOMIC_COUNTER_BUFFER_BINDING;
		case GL_ELEMENT_ARRAY_BUFFER: return GL_ELEMENT_ARRAY_BUFFER_BINDING;
		case GL_TRANSFORM_FEEDBACK_BUFFER: return GL_TRANSFORM_FEEDBACK_BUFFER_BINDING;
		case GL_UNIFORM_BUFFER: return GL_UNIFORM_BUFFER_BINDING;
		//case GL_DISPATCH_INDIRECT_BUFFER: return GL_DISPATCH_INDIRECT_BUFFER_BINDING;
		//case GL_COPY_READ_BUFFER: return GL_COPY_READ_BUFFER_BINDING;
		//case GL_COPY_WRITE_BUFFER: return GL_COPY_WRITE_BUFFER_BINDING;
		//GL_DRAW_INDIRECT_BUFFER,
		//GL_PIXEL_PACK_BUFFER,
		//GL_PIXEL_UNPACK_BUFFER,
		//GL_SHADER_STORAGE_BUFFER,
		//GL_TEXTURE_BUFFER,
		default: return 0;
	}
}

static const char * __buffer_enum_to_string(GLenum buffer_enum) {
	switch(buffer_enum) {
		case GL_ARRAY_BUFFER: return "GL_ARRAY_BUFFER";
		case GL_ATOMIC_COUNTER_BUFFER: return "GL_ATOMIC_COUNTER_BUFFER";
		case GL_COPY_READ_BUFFER: return "GL_COPY_READ_BUFFER";
		case GL_COPY_WRITE_BUFFER: return "GL_COPY_WRITE_BUFFER";
		case GL_ELEMENT_ARRAY_BUFFER: return "GL_ELEMENT_ARRAY_BUFFER";
		case GL_TRANSFORM_FEEDBACK_BUFFER: return "GL_TRANSFORM_FEEDBACK_BUFFER";
		case GL_UNIFORM_BUFFER: return "GL_UNIFORM_BUFFER";
		case GL_DRAW_INDIRECT_BUFFER: return "GL_DRAW_INDIRECT_BUFFER";
		case GL_PIXEL_PACK_BUFFER: return "GL_PIXEL_PACK_BUFFER";
		case GL_PIXEL_UNPACK_BUFFER: return "GL_PIXEL_UNPACK_BUFFER";
		case GL_TEXTURE_BUFFER: return "GL_TEXTURE_BUFFER";
		// case GL_SHADER_STORAGE_BUFFER: return "GL_SHADER_STORAGE_BUFFER";
		// case GL_DISPATCH_INDIRECT_BUFFER: return "GL_DISPATCH_INDIRECT_BUFFER";
		default: return "UNKNOWN_BUFFER_ENUM";
	}
}


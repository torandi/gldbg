#ifndef BUFFER_FUNC_H
#define BUFFER_FUNC_H

/* Exported opengl override functions */

EXPORT void __glGenBuffers(GLsizei n, GLuint * buffers) {
	__glGenBuffersINT(__functions[GLDBG_GL_GEN_BUFFERS].real_func, n, buffers);
}

EXPORT void __glGenBuffersARB(GLsizei n, GLuint * buffers) {
	__glGenBuffersINT(__functions[GLDBG_GL_GEN_BUFFERS].real_func_arb, n, buffers);
}

EXPORT void __glDeleteBuffers(GLsizei n, GLuint * buffers) {
	__glDeleteBuffersINT(__functions[GLDBG_GL_DELETE_BUFFERS].real_func, n, buffers);
}

EXPORT void __glDeleteBuffersARB(GLsizei n, GLuint * buffers) {
	__glDeleteBuffersINT(__functions[GLDBG_GL_DELETE_BUFFERS].real_func_arb, n, buffers);
}

EXPORT void __glBindBuffer(GLenum target, GLuint buffer) {
	__glBindBufferINT(__functions[GLDBG_GL_BIND_BUFFER].real_func, target, buffer);
}

EXPORT void __glBindBufferARB(GLenum target, GLuint buffer) {
	__glBindBufferINT(__functions[GLDBG_GL_BIND_BUFFER].real_func_arb,target, buffer);
}

EXPORT void __glBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage) {
	__glBufferDataINT(__functions[GLDBG_GL_BUFFER_DATA].real_func, target, size, data, usage);
}

EXPORT void __glBufferDataARB(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage) {
	__glBufferDataINT(__functions[GLDBG_GL_BUFFER_DATA].real_func_arb, target, size, data, usage);
}

#endif

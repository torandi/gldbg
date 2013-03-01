#ifndef GL_INC_H
#define GL_INC_H

#ifdef HAVE_GL_GL_H
#include <GL/gl.h>
#elif HAVE_OPENGL_GL_H
#include <OpenGL/gl.h>
#else
#error "Missing gl.h"
#endif

#endif

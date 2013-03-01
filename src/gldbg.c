#include "gldbg.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dlfcn.h>

void (*__real_SDL_GL_SwapBuffers)() = NULL;
GL_X_GET_PROC_ADDRESS_FUNC __real_glXGetProcAddressARB = NULL;
GL_X_GET_PROC_ADDRESS_FUNC __real_glXGetProcAddress = NULL;
void (*__real_SDL_Quit)() = NULL;

static int initialized = 0;

void __gldbg_init() {
	if(initialized) return;

	/* load real functions */

	__real_SDL_GL_SwapBuffers = dlsym(RTLD_NEXT, "SDL_GL_SwapBuffers");
	if (NULL == __real_SDL_GL_SwapBuffers) {
			fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
			return;
	}

	__real_SDL_Quit = dlsym(RTLD_NEXT, "SDL_Quit");
	if (NULL == __real_SDL_Quit) {
			fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
			return;
	}

	__real_glXGetProcAddress = dlsym(RTLD_NEXT, "glXGetProcAddress");
	if (NULL == __real_glXGetProcAddress) {
			fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
			return;
	}

	__real_glXGetProcAddressARB = dlsym(RTLD_NEXT, "glXGetProcAddressARB");
	if (NULL == __real_glXGetProcAddressARB) {
			fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
			return;
	}


	initialized = 1;
}

void __gldbg_printf(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	char* message = NULL;
	if(vasprintf(&message, fmt, ap) != -1) {
		fprintf(stderr, "[GLDBG] %s", message);

		free(message);
	} else {
		fprintf(stderr, "[GLDBG] Memory allocation failed\n");
		abort();
	}

	va_end(ap);
}

void __gldbg_log(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	char* message = NULL;
	if(vasprintf(&message, fmt, ap) != -1) {
		fprintf(stderr, "[GLDBG LOG] %s", message);
		//TODO: Write to log

		free(message);
	} else {
		fprintf(stderr, "[GLDBG] Memory allocation failed\n");
		abort();
	}

	va_end(ap);
}

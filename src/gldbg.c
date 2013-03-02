#include "gldbg.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dlfcn.h>

#define LOG_FILE "gldbg.log"

void (*__real_SDL_GL_SwapBuffers)() = NULL;
GL_X_GET_PROC_ADDRESS_FUNC __real_glXGetProcAddressARB = NULL;
GL_X_GET_PROC_ADDRESS_FUNC __real_glXGetProcAddress = NULL;
void (*__real_SDL_Quit)() = NULL;

static int __initialized = 0;

static FILE * __log = NULL;

void __gldbg_init() {
	if(__initialized) return;

	__log = fopen(LOG_FILE, "w");
	if(__log == NULL) __gldbg_printf("Failed to open logfile %s\n", LOG_FILE);

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

	__load_config();

	__initialized = 1;
}

void __gldbg_finish() {
	__write_buffer_config();
	__free_buffers();
	__free_config();

	if(__log != NULL) fclose(__log);
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

void __gldbg_out(enum __output_t output_target, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	char* message = NULL;
	if(vasprintf(&message, fmt, ap) != -1) {
		if(output_target & OUT_PRINT) fprintf(stderr, "%s", message);
		if(__log != NULL && output_target & OUT_LOG) fprintf(__log, "%s", message);
		free(message);
	} else {
		fprintf(stderr, "[GLDBG] Memory allocation failed\n");
		abort();
	}

	va_end(ap);
}

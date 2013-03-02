#include "gldbg.h"
#include "buffers.h"

static void __debug();
static void __log_buffers();

EXPORT void SDL_GL_SwapBuffers() {
		__gldbg_init();

		__debug();

		__real_SDL_GL_SwapBuffers();
}

EXPORT void SDL_Quit() {
		__gldbg_init();

		__gldbg_printf("Quit()\n");

		__gldbg_finish();

		__real_SDL_Quit();
}

/* Run the debugging */
static void __debug() {
	//__log_buffers();
}

static void __log_buffers() {
	for(unsigned int i=0; i<__num_buffers; ++i) {
		if(__buffers[i] != NULL && __buffers[i]->valid && __buffers[i]->target != 0 && __buffers[i]->size > 0) {
			if(__read_buffer(__buffers[i])) {
				__log_buffer(__buffers[i]);
			}
		}
	}
}

#include "gldbg.h"
#include "buffers.h"

static void __debug();
static void __collect_data();

EXPORT void SDL_GL_SwapBuffers() {
		__gldbg_init();

		__gldbg_printf("SwapBuffers()\n");

		__debug();

		__real_SDL_GL_SwapBuffers();
}

EXPORT void SDL_Quit() {
		__gldbg_init();

		__gldbg_printf("Quit()\n");

		//__free_buffers();

		__real_SDL_Quit();
}

/* Run the debugging */
static void __debug() {
	__collect_data();
}

static void __collect_data() {
	
}

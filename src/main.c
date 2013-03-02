#include "gldbg.h"
#include "buffers.h"
#include "config.h"

static void __debug();
static void __log_buffers(enum __output_t output_target);

static int print_interval = -3;
static int log_interval = -3;

static int cycle = 0;

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
	if(print_interval == -3) print_interval = __parse_interval(__config("buffers:print_interval"));
	if(log_interval == -3) log_interval = __parse_interval(__config("buffers:log_interval"));

	if(print_interval > INTERVAL_KEYDOWN || log_interval > INTERVAL_KEYDOWN) {
		int target =  ( print_interval > INTERVAL_KEYDOWN && cycle % print_interval == 0 ? OUT_PRINT : 0) | ( log_interval > INTERVAL_KEYDOWN && cycle % log_interval == 0 ? OUT_LOG : 0);
		if(target > 0) __log_buffers(target);
	}
	++cycle;
}

static void __log_buffers(enum __output_t output_target) {
	for(unsigned int i=0; i<__num_buffers; ++i) {
		if(__buffers[i] != NULL && __buffers[i]->valid && __buffers[i]->target != 0 && __buffers[i]->size > 0) {
			if(__read_buffer(__buffers[i])) {
				__log_buffer(__buffers[i], output_target);
			}
		}
	}
}

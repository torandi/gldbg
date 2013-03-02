#include "gldbg.h"
#include "buffers.h"
#include "config.h"
#include "keyboard.h"

static void __debug();
static void __log_buffers(enum __output_t output_target);

static int __print_interval;
static int __log_interval;
static struct __keyconf_t __buffer_keyconf;

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

void __init_debug() {
	__print_interval = __parse_interval(__config("buffers:print_interval"));
	__log_interval = __parse_interval(__config("buffers:log_interval"));
	__buffer_keyconf = __parse_keyconf(__config("keyboard:print_buffers"));
}

/* Run the debugging */
static void __debug() {

	if(__print_interval > INTERVAL_KEYDOWN || __log_interval > INTERVAL_KEYDOWN) {
		int target =  
			( __print_interval > INTERVAL_KEYDOWN && cycle % __print_interval == 0 ? OUT_PRINT : 0)
			| ( __log_interval > INTERVAL_KEYDOWN && cycle % __log_interval == 0 ? OUT_LOG : 0);
		if(target > 0) __log_buffers(target);
	}
	++cycle;

	if(__print_interval == INTERVAL_KEYDOWN || __log_interval == INTERVAL_KEYDOWN) {
		__update_keyboard();
		if(__keyconf_pressed(&__buffer_keyconf)) {
			__log_buffers( (__print_interval == INTERVAL_KEYDOWN) * OUT_PRINT | (__log_interval == INTERVAL_KEYDOWN) * OUT_LOG );
		}
	}
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

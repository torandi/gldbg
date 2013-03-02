#include <SDL/SDL.h>
#include <stdio.h>
#include <ctype.h>

#include "keyboard.h"
#include "config.h"

static Uint8 * __keystate = NULL;

void __update_keyboard() {
	if(__keystate == NULL) __keystate = SDL_GetKeyState(NULL);

	SDL_PumpEvents();
}

struct __keyconf_t __parse_keyconf(const char * conf) {
	struct __keyconf_t config = { 
		.keys = { SDLK_UNKNOWN, },
		.down = 0,
	};

	char key[16];
	int key_len = 0;
	int index = 0;
	for(int i=0; i<= strlen(conf); ++i) {
		if(i < strlen(conf) && !isspace(conf[i])) {
			key[key_len++] = conf[i];
		} else if(key_len > 0) {
			key[key_len] = 0;
			lowercase(key);
			if(index == 5) {
				__gldbg_printf("To many keys in combination %s\n", conf);
				break;
			}

			if(strlen(key) == 1) {
				config.keys[index] = (int)key[0];
			} else if(strcmp(key, "ctrl") == 0 || strcmp(key, "lctrl") == 0) {
				config.keys[index] = SDLK_LCTRL;
			} else if(strcmp(key, "rctrl") == 0) {
				config.keys[index] = SDLK_RCTRL;
			} else if(strcmp(key, "shift") == 0 || strcmp(key, "lshift") == 0) {
				config.keys[index] = SDLK_LSHIFT;
			} else if(strcmp(key, "rshift") == 0) {
				config.keys[index] = SDLK_RSHIFT;
			} else if(strcmp(key, "alt") == 0 || strcmp(key, "lalt") == 0) {
				config.keys[index] = SDLK_LALT;
			} else if(strcmp(key, "ralt") == 0) {
				config.keys[index] = SDLK_RALT;
			} else if(strcmp(key, "meta") == 0 || strcmp(key, "lmeta") == 0) {
				config.keys[index] = SDLK_LMETA;
			} else if(strcmp(key, "rmeta") == 0) {
				config.keys[index] = SDLK_RMETA;
			} else {
				__gldbg_printf("Unknown key %s\n", key);
				--index;
			}
			++index;
			key_len = 0;
		}
	}

	return config;
}

int __keyconf_pressed(struct __keyconf_t * conf) {
	for(int i=0; i<5; ++i) {
		if(conf->keys[i] != SDLK_UNKNOWN && __keystate[conf->keys[i]] == 0) {
			conf->down = 0;
			return 0;
		}
	}
	int ret = conf->down;
	conf->down = 1;
	return !ret;
}

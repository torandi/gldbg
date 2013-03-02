#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <SDL/SDL.h>

struct __keyconf_t {
	SDLKey keys[5]; /* No more than five keys per key combination */
};

void __update_keyboard();

struct __keyconf_t __parse_keyconf(const char * conf);

int __keyconf_pressed(const struct __keyconf_t * conf);



#endif

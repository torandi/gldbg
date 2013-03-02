#include <SDL/SDL.h>

static Uint8 * keystate = NULL;

void __update_keyboard() {
	if(keystate == NULL) keystate = SDL_GetKeyState(NULL);

	SDL_PumpEvents();
}

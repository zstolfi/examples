#include <SDL.h>
#include <iostream>
#include <string>
#include "window.hh"

int main(int argc, char* argv[]) {
	Window window("Pixel Font Experiments", 800, 600);
	if (!window.init())      { std::exit(1); }
	if (!window.loadMedia()) { std::exit(2); }

	std::cout << "Window initialized.\n";

	GameState state { .text = "EXAMPLE STRING" };
	bool redraw = true;

	/* MAIN LOOP */
	for (bool quit=false; !quit; ) {
		// handle events
		for (SDL_Event ev; SDL_PollEvent(&ev); ) {
			switch (ev.type) {

			case SDL_QUIT: { quit = true; } break;

			case SDL_KEYDOWN:
				switch (ev.key.keysym.sym) {
				case SDLK_ESCAPE: { quit = true; } break;
				case SDLK_RETURN: { redraw = true; } break;
			} break;
		} }

		if (redraw) {
			window.draw(state);

			state.frameCount++;
			redraw = false;
		}
	}

	return 0;
}
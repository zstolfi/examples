#include <SDL.h>
#include <iostream>
#include <string>
#include "window.hh"

int main(int argc, char* args[]) {
	const int W = 800;
	const int H = 600;

	Window window("Blank SDL Project", W, H);
	if (!window.init())      { std::exit(1); }
	if (!window.loadMedia()) { std::exit(2); }

	std::cout << "Window initialized.\n";

	/* MAIN LOOP */
	for (bool quit=false; !quit; ) {
		// handle events
		for (SDL_Event ev; SDL_PollEvent(&ev); ) {
			switch (ev.type) {

			case SDL_QUIT: { quit = true; } break;

			case SDL_KEYDOWN:
				switch (ev.key.keysym.sym) {
				case SDLK_ESCAPE: { quit = true; break; }
			} break;
		} }

		for (std::size_t i=0; i < W*H; i++) {
			unsigned* pixels = (unsigned*)window.surface->pixels;
			pixels[i] = 0xFFFFFFFF;
		}
		window.update();

		// TODO: figure out proper fps capping
		// SDL_Delay(1000/60);
	}

	return 0;
}
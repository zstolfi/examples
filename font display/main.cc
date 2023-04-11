#include <SDL.h>
#include <iostream>
#include <string>
#include "window.hh"

struct parseArguments {
	std::string text = "SAMPLE TEXT";

	parseArguments (int argc, char* argv[]) {
		if (argc > 1) { text = argv[1]; }
	}
};

int main(int argc, char* argv[]) {
	auto settings = parseArguments {argc, argv};

	Window window("Pixel Font Experiments", 800, 600);
	if (!window.init())      { std::exit(1); }
	if (!window.loadMedia()) { std::exit(2); }

	std::cout << "Window initialized.\n";

	GameState state { .text = settings.text };
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
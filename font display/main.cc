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

	std::string fontName = "font_2";
	std::cout << "Searching font: " << fontName << "\n";
	Font* font = nullptr;
	for (auto& f : window.media.fonts) {
		if (f.name == fontName) { font = &f; break; }
	}
	if (!font) { std::cout << "Font not found!\n"; std::exit(3); }
	else { std::cout << "Font found.\n"; }

	GameState state { .text = settings.text, .selectedFont = font };
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

		SDL_Delay(1000/60);
	}

	return 0;
}
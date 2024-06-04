#include <SDL2/SDL.h>
#include "window.hh"
#include "font.hh"
#include "sdl_lib.hh"
#include <iostream>
#include <string>
#include <utility>
#include <cstdint>

int main(int argc, char* args[]) try {
	const unsigned W = 800;
	const unsigned H = 600;
	std::string text = (argc > 1) ? args[1] : "";

	Window window {"SDL Project", W, H, {"assets/ftotf.png"}};
	Font font {"Unnamed Font", 0x20, 0x7E,
		// Glyph region size functor:
		[](char c) { return std::pair {10u, 10u}; },
		// Get-pixel functor:
		[img = window.media[0]] (char c, unsigned x, unsigned y) {
			uint8_t r=0, g=0, b=0;
			SDL_GetRGB(
				SDL::surfaceGetPixel(img, 10u*(c-0x20) + x, y),
				img->format, &r, &g, &b
			);
			// RED  : letter shapes
			// GREEN: (unused)
			// BLUE : letter padding
			return r == 0;
		},
		// Set-pixel functor:
		[img = window.surface] (unsigned x, unsigned y) {
			if (!(0 <= x&&x < W) || !(0 <= y&&y < H)) return;
			SDL::surfaceSetPixel(
				img, x, y, SDL_MapRGB(img->format, 0, 0, 0)
			);
		}
	};

	/* Main loop: */
	bool input = true, quit = false;
	while (!quit) {
		// Handle events.
		for (SDL_Event ev; SDL_PollEvent(&ev); input=true) {
			switch (ev.type) {
				case SDL_QUIT: { quit = true; } break;
				case SDL_KEYDOWN: {
					switch (ev.key.keysym.sym) {
						case SDLK_ESCAPE: { quit = true; } break;
					} break;
				}
			}
		}

		if (input) {
			SDL_FillRect(
				window.surface, nullptr,
				SDL_MapRGB(window.surface->format, 255, 255, 255)
			);
			font.draw(text, 10, 10, 2);
			window.update();
		}

		SDL_Delay(1000/60);
	}
}
catch (const std::string& error) {
	std::cerr << "ERROR:\t" << error << "\n";
}

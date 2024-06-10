#include <SDL2/SDL.h>
#include "window.hh"
#include "font.hh"
#include "sdl_lib.hh"
#include <iostream>
#include <string>
#include <utility>
#include <cstdint>

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

int main(int argc, char* args[]) try {
	const unsigned W = 800;
	const unsigned H = 600;
	std::string text = (argc > 1) ? args[1] : "";

	Window window {"SDL Project", W, H, {"assets/nivosia_beta.png"}};
	Font font {"Nivosia Beta",
		// Input settings:
		{.range = AsciiRange {0x20, 0x7E}, .width = 10, .height = 10},
		// Default render settings:
		{.lineHeight = 15, .tabStops = 20},
		// Get-pixel functor:
		[img = window.media[0]] (char c, unsigned x, unsigned y) {
			uint8_t r=0, g=0, b=0;
			SDL_GetRGB(
				SDL::surfaceGetPixel(img, 10u*(c-0x20) + x, y),
				img->format, &r, &g, &b
			);
			return PixelData {.isLetter = !r, .isMargin = !b};
		},
		// Set-pixel functor:
		[img = window.surface] (unsigned x, unsigned y, Color col) {
			if (!(0 <= x&&x < W) || !(0 <= y&&y < H)) return;
			SDL::surfaceSetPixel(
				img, x, y, SDL_MapRGB(img->format, col.r, col.g, col.b)
			);
		}
	};

	/* ~~~~ Main Loop ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

	bool input = true, quit = false;
	while (!quit) {
		// Handle events.
		input = false;
		for (SDL_Event ev; SDL_PollEvent(&ev); input=true) {
			switch (ev.type) {
				case SDL_QUIT: { quit = true; } break;
				case SDL_KEYDOWN: {
					switch (ev.key.keysym.sym) {
						case SDLK_ESCAPE: { quit = true; } break;
					}
				} break;
			}
		}

		// Only render on input.
		if (input) {
			SDL_FillRect(
				window.surface, nullptr,
				SDL_MapRGB(window.surface->format, 255, 255, 255)
			);
			// Render our text!
			font.draw(text, {
				.position = {12, 12},
				.scale = 4,
				.color = {0x00, 0x00, 0x00},
			});
			font.draw(text, {
				.position = {10, 10},
				.scale = 4,
				.color = {0xFF, 0xFF, 0xFF},
			});
			window.update();
		}

		SDL_Delay(1000/60);
	}
}
catch (const std::string& error) {
	std::cerr << "ERROR:\t" << error << "\n";
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

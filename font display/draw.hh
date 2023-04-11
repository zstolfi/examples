#pragma once
#include <SDL.h>
#include "game state.hh"

class Canvas {
private:
	unsigned* pixels;
	const int W;
	const int H;

public:
	Canvas(SDL_Surface* surface)
	: pixels{(unsigned*)surface->pixels}
	, W{surface->w}
	, H{surface->h} {}

	void draw(GameState& s) {
		for (int i=0; i < W*H; i++) {
			unsigned x = i%W;
			unsigned y = i/W;

			pixels[i] = 0xFF<<24 | (i + s.frameCount*s.frameCount)*(x/8%2 ^ y/8%2);
		}
	}
};
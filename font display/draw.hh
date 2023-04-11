#pragma once
#include <SDL.h>

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

	void draw() {
		for (int i=0; i < W*H; i++) {
			pixels[i] = 0xFFFFFFFF;
		}
	}
};
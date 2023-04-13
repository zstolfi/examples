#pragma once
#include <SDL.h>
#include "game state.hh"
#include "fonts.hh"

class Canvas {
private:
	using Color = unsigned;

	Color* pixels;
	const int W;
	const int H;

	void put(int x, int y, Color c) {
		if ((0 <= x&&x < W) && (0 <= y&&y < H)) {
			pixels[y * W + x] = c;
		}
	}

public:
	Canvas(SDL_Surface* surface)
	: pixels{(Color*)surface->pixels}
	, W{surface->w}
	, H{surface->h} {}

	void draw(const GameState& s) {
		for (int i=0; i < W*H; i++) {
			pixels[i] = 0xFFFFFFFF;
		}

		const Font& font = fonts.at("font 2");

		int cursorX = 10;
		int cursorY = 10;
		for (char c : s.text) {
			if (c == '\n') {
				cursorX = 10;
				cursorY += font.lineHeight + 3;
				continue;
			}
			if (!font.glyphs.contains(c)) { continue; }

			const CharData& glyph = font.glyphs.at(c);
			
			for (unsigned y=0; y < glyph.h; y++) {
			for (unsigned x=0; x < glyph.w; x++) {
				if (glyph.pixels[y * glyph.w + x]) {
					put(cursorX+x, cursorY+y, 0xED1C24);
				}
			} }

			cursorX += glyph.w + 3;
		}
	}
};
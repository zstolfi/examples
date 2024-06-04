#pragma once
#include <string>
#include <map>
#include <functional>
#include <utility>
#include <iostream>

class Font {

	struct Glyph {
		unsigned sizeX;
		unsigned sizeY;
		std::vector<bool> pixels;
	};

private:
	std::function<void(unsigned, unsigned)> setPixel {};
	std::map<char, Glyph> m_glyphs;

public:
	Font(
		std::string title, unsigned min, unsigned max,
		std::function<std::pair<unsigned, unsigned>(char)> glyphRegionSize,
		std::function<bool(char, unsigned, unsigned)> getPixel,
		std::function<void(unsigned, unsigned)> setPixel
	)
	: setPixel{setPixel} {
		for (unsigned c=min; c<max; c++) {
			auto [W, H] = glyphRegionSize(c);
			std::vector<bool> pixels (W*H, false);
			for (unsigned y=0; y<W; y++) {
			for (unsigned x=0; x<H; x++) {
				if (getPixel(c, x, y)) { pixels[y * W + x] = true; }
			} }
			m_glyphs[c] = Glyph {W, H, pixels};
		}
	}

	void draw(std::string str, unsigned x0, unsigned y0) {
		unsigned x=x0, y=y0;
		for (char c : str) {
			if (!m_glyphs.contains(c)) continue;
			auto glyph = m_glyphs.at(c);
			for (unsigned i=0; i<glyph.pixels.size(); i++) {
				if (glyph.pixels[i]) setPixel(
					 x + i % glyph.sizeX,
					 y + i / glyph.sizeX
				);
			}
			x += 10u;
		}
	}
};

#pragma once
#include <string>
#include <map>
#include <algorithm>
#include <functional>
#include <utility>
#include <iostream>

class Font {

	struct Glyph {
		unsigned sizeX, sizeY;
		std::vector<bool> pixels;
		unsigned startX, endX;

		Glyph(unsigned sizeX, unsigned sizeY, std::vector<bool> pixels)
		: sizeX{sizeX}, sizeY{sizeY}, pixels{pixels} {
			startX = sizeX-1, endX = 0;
			for (unsigned i=0; i<pixels.size(); i++) {
				if (pixels[i]) startX = std::min(i % sizeX, startX)
				,              endX   = std::max(i % sizeX, endX  );
			}
			if (startX > endX) startX = 0, endX = sizeX-1;
		}
	};

private:
	std::function<void(unsigned, unsigned)> setPixel;
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
			m_glyphs.emplace(c, Glyph {W, H, pixels});
		}
	}

	void draw(std::string str, unsigned x0, unsigned y0, unsigned scale=2) {
		unsigned x=x0, y=y0;
		for (char c : str) {
			if (c == '\n') { x = x0, y += 15*scale; continue; }
			if (!m_glyphs.contains(c)) continue;
			auto glyph = m_glyphs.at(c);
			for (unsigned i=0; i<glyph.pixels.size(); i++) {
				if (!glyph.pixels[i]) continue;
				for (unsigned z=0; z<scale; z++) {
				for (unsigned w=0; w<scale; w++) {
					setPixel(
						x + (i % glyph.sizeX - glyph.startX)*scale + z,
						y + (i / glyph.sizeX               )*scale + w
					);
				} }
			}
			x += (glyph.endX - glyph.startX + 2)*scale;
		}
	}
};

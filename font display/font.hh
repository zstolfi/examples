#pragma once
#include <string>
#include <map>
#include <functional>
#include <algorithm>
#include <numeric>
#include <utility>
#include <iostream>
#include <limits>
namespace ranges = std::ranges;

class Font {
public:
	struct PixelData {
		bool isLetter {}, isPadding {};
		PixelData(bool l, bool p) : isLetter{l}, isPadding{p} {}
	};

	struct Hull {
		std::vector<unsigned> left, right;
		Hull(std::vector<bool> pixels, unsigned sx, unsigned sy)
		: left(sy, sx), right(sy, 0) {
			for (unsigned y=0; y<sy; y++) {
			for (unsigned x=0; x<sx; x++) {
				if (pixels[y * sy + x]) left [y] = std::min(x  , left [y])
				,                       right[y] = std::max(x+1, right[y]);
			} }
		}

		bool isEmptyAt(unsigned y) { return left[y] > right[y]; }
	};

	struct Glyph {
		const char which;
		unsigned sizeX, sizeY;
		std::vector<bool> pixels;
		Hull letterHull;
		Hull paddingHull;

		Glyph(
			char c, unsigned sx, unsigned sy,
			std::vector<bool> pixels, std::vector<bool> padding
		)
		: which{c}, sizeX{sx}, sizeY{sy}, pixels{pixels}
		, letterHull{pixels, sx, sy}, paddingHull{padding, sx, sy} {
			for (unsigned y=0; y<sy; y++) {
				if (paddingHull.isEmptyAt(y)) {
					std::cerr << "Warning: Padding on glyph '"<< which <<"'\n";
					paddingHull.left [y] = 0, paddingHull.right[y] = 1;
				}
			}

			if (ranges::none_of(pixels, std::identity {})) {
				letterHull = paddingHull;
			}
		}
	};

private:
	std::function<void(unsigned, unsigned)> setPixel;
	std::map<char, Glyph> m_glyphs;
	unsigned lineHeight;
	unsigned tabWidth;

public:
	const std::string title;

	Font(
		std::string title, unsigned min, unsigned max,
		unsigned lineHeight, unsigned tabWidth,
		std::function<std::pair<unsigned, unsigned>(char)> glyphRegionSize,
		std::function<Font::PixelData(char, unsigned, unsigned)> getPixel,
		std::function<void(unsigned, unsigned)> setPixel
	)
	: setPixel{setPixel}
	, lineHeight{lineHeight}, tabWidth{tabWidth}
	, title{title} {
		for (unsigned c=min; c<=max; c++) {
			auto [W, H] = glyphRegionSize(c);
			std::vector<bool> pixels (W*H, false), padding (W*H, false);
			for (unsigned y=0; y<W; y++) {
			for (unsigned x=0; x<H; x++) {
				PixelData p = getPixel(c, x, y);
				if (p.isLetter) pixels[y * W + x] = true;
				if (p.isPadding) padding[y * W + x] = true;
			} }
			m_glyphs.emplace(c, Glyph {(char)c, W, H, pixels, padding});
		}
	}

	void draw(std::string str, unsigned x0, unsigned y0, unsigned scale=2) {
		int x=x0, y=y0;
		const Glyph* prevGlyph = nullptr;
		for (char c : str) switch(c) {
		break; case '\n':
			x = x0;
			y += (lineHeight+1)*scale;
			prevGlyph = nullptr;
		break; case '\t':
			x = nextTabStop(x/scale)*scale;
			prevGlyph = nullptr;
		break; default: {
			if (!m_glyphs.contains(c)) continue;
			const Glyph& glyph = m_glyphs.at(c);

			x += kerning(prevGlyph, &glyph) * scale;
			prevGlyph = &glyph;

			// Draw it!
			for (unsigned gy=0; gy<glyph.sizeY; gy++) {
			for (unsigned gx=0; gx<glyph.sizeX; gx++) {
				if (!glyph.pixels[gy * glyph.sizeY + gx]) continue;
				drawSquare(
					x + gx*scale,
					y + gy*scale,
					scale
				);
			} }
		} }

	}

private:
	int kerning(const Glyph* a, const Glyph* b) {
		if (b == nullptr) return 0;
		if (a == nullptr) return -*ranges::min_element(b->letterHull.left);
		int dist = std::numeric_limits<int>::min();
		for (unsigned y=0; y<lineHeight; y++) {
			dist = std::max<int>({dist,
				(int)(a->paddingHull.right[y] - b->letterHull.left[y]),
				(int)(a->letterHull.right[y] - b->paddingHull.left[y])
			});
		}
		return dist;
	}

	void drawSquare(unsigned x0, unsigned y0, unsigned size) {
		if (size == 1) { setPixel(x0, y0); return; }
		for (unsigned y=0; y<size; y++) {
		for (unsigned x=0; x<size; x++) {
			setPixel(x + x0, y + y0);
		} }
	}

	unsigned nextTabStop(unsigned x) {
		return tabWidth * (x / tabWidth + 1);
	}
};

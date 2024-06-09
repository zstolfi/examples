#pragma once
#include <string>
#include <map>
#include <functional>
#include <algorithm>
#include <numeric>
#include <utility>
#include <variant>
#include <iostream>
#include <climits>
namespace ranges = std::ranges;

struct PixelData {
	bool isLetter {}, isMargin {};
};

struct AsciiRange {
	std::string list = "";
	AsciiRange() {}
	AsciiRange(std::string s) : list{s} {}
	AsciiRange(char lo, char hi) {
		list.resize(hi-lo + 1);
		std::iota(list.begin(), list.end(), lo);
	}
};

struct ImportSettings {
	AsciiRange range {};
	unsigned width {}, height {};
};

struct RenderSettings {
	unsigned lineHeight = 15u;
	unsigned tabStops = 20u;
};

struct Hull {
	std::vector<int> left, right;
	Hull(unsigned height) : left(height, INT_MAX), right(height, INT_MIN) {}
	bool isEmptyAt(unsigned y) const { return left[y] > right[y]; }

	void init(
		unsigned sx, unsigned sy,
		std::function<bool(unsigned, unsigned)> pred
	) {
		for (unsigned y=0; y<sy; y++) {
		for (unsigned x=0; x<sx; x++) {
			if (pred(x, y)) left [y] = std::min<int>(x  , left [y])
			,               right[y] = std::max<int>(x+1, right[y]);
		} }
	}
};

struct Glyph {
	const char code;
	unsigned sizeX, sizeY;
	std::vector<bool> pixels;
	Hull letterHull, marginHull;

	Glyph(
		char c, unsigned sx, unsigned sy,
		std::function<PixelData(char, unsigned, unsigned)> getPixel
	)
	: code{c}, sizeX{sx}, sizeY{sy}, pixels(sx*sy, false)
	, letterHull{sy}, marginHull{sy} {
		for (unsigned y=0; y<sy; y++) {
		for (unsigned x=0; x<sx; x++) {
			pixels[y * sy + x] = getPixel(c, x, y).isLetter;
		} }
		letterHull.init(sx, sy,
			[&](unsigned x, unsigned y) { return getPixel(c, x, y).isLetter; }
		);
		marginHull.init(sx, sy,
			[&](unsigned x, unsigned y) { return getPixel(c, x, y).isMargin; }
		);

		for (unsigned y=0; y<sy; y++) {
			if (marginHull.isEmptyAt(y)) {
				std::cerr << "Warning: marign on glyph '" << c << "'\n";
				marginHull.left[y] = 0, marginHull.right[y] = 1;
			}
		}

		if (ranges::none_of(pixels, std::identity {})) {
			letterHull = marginHull;
		}
	}
};

class Font {
public:
	const std::string title;
	RenderSettings render;

private:
	const ImportSettings import;
	std::function<void(unsigned, unsigned)> m_setPixel;
	std::map<char, Glyph> m_glyphs;

public:
	Font(
		std::string title, ImportSettings is, RenderSettings rs,
		std::function<PixelData(char, unsigned, unsigned)> getPixel,
		std::function<void(unsigned, unsigned)> setPixel
	)
	: title{title}, render{rs}, import{is}, m_setPixel{setPixel} {
		for (char c : import.range.list) {
			m_glyphs.emplace(c,
				Glyph {c, import.width, import.height, getPixel}
			);
		}
	}

	void draw(std::string str, unsigned x0, unsigned y0, unsigned scale=2) {
		int x=x0, y=y0;
		const Glyph* prevGlyph = nullptr;
		for (char c : str) switch(c) {
		break; case '\n':
			x = x0;
			y += (render.lineHeight+1)*scale;
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
		const Hull& aLtr = a->letterHull, aMgn = a->marginHull;
		const Hull& bLtr = b->letterHull, bMgn = b->marginHull;

		int dist = INT_MAX;
		for (unsigned y=0; y<import.height; y++) {
			int d1 = INT_MAX, d2 = INT_MAX;
			if (!aLtr.isEmptyAt(y)) d1 = bMgn.left[y] - aLtr.right[y];
			if (!bLtr.isEmptyAt(y)) d2 = bLtr.left[y] - aMgn.right[y];
			dist = std::min({dist, d1, d2});
		}
		return -dist;
	}

	void drawSquare(unsigned x0, unsigned y0, unsigned size) {
		if (size == 1) { m_setPixel(x0, y0); return; }
		for (unsigned y=0; y<size; y++) {
		for (unsigned x=0; x<size; x++) {
			m_setPixel(x + x0, y + y0);
		} }
	}

	unsigned nextTabStop(unsigned x) {
		return render.tabStops * (x / render.tabStops + 1);
	}
};

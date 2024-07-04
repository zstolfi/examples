#include "font.hh"
#include <iostream>
#include <climits>
namespace ranges = std::ranges;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

Hull::Hull(unsigned height) : left(height, INT_MAX), right(height, INT_MIN) {}

bool Hull::isEmptyAt(unsigned y) const { return left[y] > right[y]; }

void Hull::init(
	unsigned sx, unsigned sy,
	std::function<bool(unsigned, unsigned)> pred
) {
	for (unsigned y=0; y<sy; y++) {
	for (unsigned x=0; x<sx; x++) {
		if (pred(x, y)) left [y] = std::min<int>(x  , left [y])
		,               right[y] = std::max<int>(x+1, right[y]);
	} }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

Glyph::Glyph(
	char c, unsigned sx, unsigned sy,
	std::function<PixelData(char, unsigned, unsigned)> getPixel
)
: code{c}, sizeX{sx}, sizeY{sy}, pixels(sx*sy, false)
, letterHull{sy}, marginHull{sy} {
	auto letterPx = [&](auto x, auto y) { return getPixel(c, x, y).isLetter; };
	auto marginPx = [&](auto x, auto y) { return getPixel(c, x, y).isMargin; };
	// Initialize pixel/hull member variables.
	for (unsigned y=0; y<sy; y++) {
	for (unsigned x=0; x<sx; x++) {
		pixels[y * sx + x] = letterPx(x, y);
	} }
	letterHull.init(sx, sy, letterPx);
	marginHull.init(sx, sy, marginPx);
	// Check for missing margin lines.
	for (unsigned y=0; y<sy; y++) {
		if (marginHull.isEmptyAt(y)) {
			std::cerr << "Warning: marign on glyph '" << code << "'\n";
			// Fill with valid, but probably wrong, data. 
			marginHull.left[y] = 0, marginHull.right[y] = 1;
		}
	}
	// Check if the glyph is a space character (i.e. no pixels to display).
	// This allows the start or end of a line to be justified with a space.
	if (ranges::none_of(pixels, std::identity {})) {
		letterHull = marginHull;
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

Font::Font(
	std::string title, ImportSettings is, RenderSettings rs,
	std::function<PixelData(char, unsigned, unsigned)> getPixel,
	std::function<void(unsigned, unsigned, Color)> setPixel
)
: title{title}, render{rs}, import{is}, m_setPixel{setPixel} {
	for (char c : import.range.list) {
		m_glyphs.emplace(c, Glyph {c, import.width, import.height, getPixel});
	}
}

void Font::draw(std::string str, StyleSettings style) {
	int x=0, y=0;
	const Glyph* prevGlyph = nullptr;
	
	for (char c : str) switch(c) {
	break; case '\n':
		x = 0, y += (render.lineHeight);
		prevGlyph = nullptr;
	break; case '\t':
		x = nextTabStop(x);
		prevGlyph = nullptr;
	break; default: {
		if (!m_glyphs.contains(c)) continue;
		const Glyph& glyph = m_glyphs.at(c);
		// Update our state.
		x += kerning(prevGlyph, &glyph);
		prevGlyph = &glyph;
		// Draw our glyph!
		for (unsigned gy=0; gy<glyph.sizeY; gy++) {
		for (unsigned gx=0; gx<glyph.sizeX; gx++) {
			if (glyph.pixels[gy * glyph.sizeX + gx]) {
				drawSquare(x+gx, y+gy, style);
			}
		} }
	} }
}

int Font::kerning(const Glyph* a, const Glyph* b) {
	// If there is no b, don't worry about kerning.
	if (b == nullptr) return 0;
	// if there is no a, justify to the left.
	if (a == nullptr) return -*ranges::min_element(b->letterHull.left);
	const Hull& aLtr = a->letterHull, aMgn = a->marginHull;
	const Hull& bLtr = b->letterHull, bMgn = b->marginHull;
	// Find the maximum amount our glyphs have "collided"
	// Check both Letter -> Margin, and Margin <- Letter.
	int dist = INT_MIN;
	for (unsigned y=0; y<import.height; y++) {
		int d1 = INT_MIN, d2 = INT_MIN;
		if (!aLtr.isEmptyAt(y)) d1 = aLtr.right[y] - bMgn.left[y];
		if (!bLtr.isEmptyAt(y)) d2 = aMgn.right[y] - bLtr.left[y];
		dist = std::max({dist, d1, d2});
	}
	return dist;
}

unsigned Font::nextTabStop(unsigned x) {
	return render.tabStops * (x / render.tabStops + 1);
}

void Font::drawSquare(unsigned x0, unsigned y0, StyleSettings style) {
	for (unsigned y=0; y<style.scale; y++) {
	for (unsigned x=0; x<style.scale; x++) {
		// Translate font-space into screen-space.
		m_setPixel(
			x0*style.scale + x + style.position.x,
			y0*style.scale + y + style.position.y,
			style.color
		);
	} }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

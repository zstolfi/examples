#pragma once
#include <functional>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <utility>
#include <cstdint>

/* ~~~~ User Input Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

struct PixelData {
	bool isLetter : 1 {}, isMargin : 1 {};
};

struct AsciiRange {
	std::string list = "";
	AsciiRange(std::string s) : list{s} {}
	AsciiRange(char lo, char hi) {
		list.resize(hi-lo + 1);
		std::iota(list.begin(), list.end(), lo);
	}
};

struct Color {
	uint8_t r {}, g {}, b {};
};

struct StyleSettings {
	struct { unsigned x, y; } position = {0, 0};
	unsigned scale = 1;
	Color color = {0, 0, 0};
};

/* ~~~~ Font Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

// The left & right edges of a shape. (Used for kerning).
struct Hull {
	std::vector<int> left, right;
	Hull(unsigned height);
	bool isEmptyAt(unsigned y) const;
	void init(
		unsigned sx, unsigned sy,
		std::function<bool(unsigned, unsigned)> pred
	);
};

class Glyph {
public:
	const char code;
	unsigned sizeX, sizeY;
	std::vector<bool> pixels;
	Hull letterHull, marginHull;

	Glyph(
		char c, unsigned sx, unsigned sy,
		std::function<PixelData(char, unsigned, unsigned)> getPixel
	);
};

class Font {
public:
	struct ImportSettings {
		AsciiRange range {""};
		unsigned width {}, height {};
	};

	struct RenderSettings {
		unsigned lineHeight = 15;
		unsigned tabStops = 20;
	};

	const std::string title;
	RenderSettings render;

private:
	const ImportSettings import;
	std::function<void(unsigned, unsigned, Color)> m_setPixel;
	std::map<char, Glyph> m_glyphs;

public:
	Font(
		std::string title, ImportSettings, RenderSettings,
		std::function<PixelData(char, unsigned, unsigned)> getPixel,
		std::function<void(unsigned, unsigned, Color)> setPixel
	);

	void draw(std::string str, StyleSettings = {});

private:
	int kerning(const Glyph* a, const Glyph* b);

	unsigned nextTabStop(unsigned x);

	void drawSquare(unsigned x0, unsigned y0, StyleSettings);
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

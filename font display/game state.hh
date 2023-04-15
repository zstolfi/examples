#pragma once
#include <string>
#include "fonts.hh"

struct GameState {
	unsigned frameCount = 0;
	std::string text = "";
	Font* selectedFont = nullptr;
};

struct Media {
	std::vector<Font> fonts;
};
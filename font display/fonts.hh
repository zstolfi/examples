#pragma once
#include <map>
#include <vector>
#include <string>
#include <istream>
#include <optional>

struct CharData {
	unsigned w, h;
	std::vector<bool> pixels;

	CharData(unsigned w, unsigned h, std::string str)
	: w{w}, h{h}
	{
		pixels.resize(w*h);
		for (std::size_t i=0; i < w*h; i++) {
			pixels[i] = str[i] != ' ';
		}
	}
};

struct Font {
	std::string name;
	unsigned lineHeight;
	std::map<char, CharData> glyphs;
};

std::optional<Font> parseFont(std::istream s) {
	std::optional<Font> result = std::nullopt;

	auto isWhitespace = [](char c) { return c==' ' || c=='\t' || c=='\0'; };
	auto parseNumber = [](std::string s) -> int {
		if (str[0] == '\'' && str[2] == '\'') { return str[1]; }
		if (str.starts_with("0x")) { return std::stoi(str.substr(2), nullptr,  16); }
		return std::stoi(str);
	}

	/* ~~~ Parse Variables ~~~ */
	Font currentFont {};
	CharData currentChar {};
	std::string currentString {};
	/* ~~~~~~~~~~~~~~~~~~~~~~~ */

	const MAX_LINE = 1024;
	std::string line {};
	line.resize(MAX_LINE);
	while (s.getline(line.data(), MAX_LINE)) {
		std::size_t i = 0;
		// ignore whitespace
		while (isWhitespace(line[i])) { i++; }
		// skip comments
		if (line[i] == '#') { continue; }
		// append string values
		if (line[i] == '"') {
			i++;
			std::size_t len = 0;
			while (line[i+len] != '"') { len++; }
			currentString += line.substr(i, len);
			continue;
		}
		// Font declaration
		if (line.substr(i).starts_with("Font") && isWhitespace(line[i+5])) {
			i += 4;
			currentFont = {};

			while (isWhitespace(line[i])) { i++; }
			std::size_t nameLen = 0;
			while (!isWhitespace(line[i+nameLen])) { nameLen++; }
			currentFont.name = line.substr(i, nameLen);
			i+= nameLen;

			while (isWhitespace(line[i])) { i++; }
			std::size_t heightLen = 0;
			while (!isWhitespace(line[i+heightLen]));
			currentFont.lineHeight = parseNumber(line.substr(i, heightLen));
			i += heightLen;
			continue;
		}
	}
}
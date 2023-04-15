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
			pixels[i] = (str[i] != ' ');
		}
	}
};

struct Font {
	std::string name;
	unsigned lineHeight;
	std::map<char, CharData> glyphs;
};



std::optional<Font> parseFont(std::istream& s) {
	std::optional<Font> result = std::nullopt;

	auto isWhitespace = [](char c) {
		return c==' ' || c=='\t' || c=='\0';
	};
	auto parseNum = [](std::string str) -> int {
		if (str[0] == '\'' && str[2] == '\'') { return str[1]; }
		if (str.starts_with("0x")) { return std::stoi(str.substr(2), nullptr,  16); }
		return std::stoi(str);
	};
	auto nextArg = [&](std::string& line, std::size_t& i) -> std::string {
		while (isWhitespace(line[i])) { i++; }
		std::size_t len = 0;
		while (!isWhitespace(line[i+len])) { len++; }
		auto result = line.substr(i, len);
		i += len;
		return result;
	};

	/* ~~~ Parse Variables ~~~ */
	Font currentFont {};
	char currentChar {};
	unsigned currentCharW {};
	unsigned currentCharH {};
	std::string currentString {};
	/* ~~~~~~~~~~~~~~~~~~~~~~~ */

	const int MAX_LINE = 1024;
	std::string line {};
	line.resize(MAX_LINE);
	while (s.getline(line.data(), MAX_LINE)) {
		std::size_t i = 0;
		// ignore whitespace
		while (isWhitespace(line[i]) && i < line.size()) { i++; }
		// skip blank lines
		if (i == line.size()) { continue; }
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
		if (line.substr(i).starts_with("Font") && isWhitespace(line[i+4])) {
			i += 4;
			currentFont = Font {
				.name = nextArg(line, i),
				.lineHeight = parseNum(nextArg(line, i))
			};
			continue;
		}
		// Font end
		if (line.substr(i).starts_with("FontEnd") && isWhitespace(line[i+7])) {
			i += 7;
			return currentFont;
		}
		// Char declaration
		if (line.substr(i).starts_with("Char") && isWhitespace(line[i+4])) {
			i += 4;
			currentChar  = parseNum(nextArg(line, i));
			currentCharW = parseNum(nextArg(line, i));
			currentCharH = parseNum(nextArg(line, i));
			currentString = "";
			continue;
		}
		// Char end
		if (line.substr(i).starts_with("CharEnd") && isWhitespace(line[i+7])) {
			i += 7;
			// currentFont.glyphs.emplace(currentChar,
			// 	currentCharW, currentCharH, currentString
			// );
			currentFont.glyphs.insert({currentChar,
				CharData { currentCharW, currentCharH, currentString }
			});
			continue;
		}
	}

	return result;
}
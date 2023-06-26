#pragma once
#include "common.hh"
#include <istream>

namespace detail {
	char parseChar(std::istream& is);
}

auto preprocess(std::istream& is) {
	std::vector<Line> result {};

	// https://files.catbox.moe/6roobe.png
	enum { Code, String, Char1, Char2, Comment, Accept } state = Code;

	Line line;
	std::string curString;
	for (char c=is.get(); is; c=is.get()) {
		if (state == Accept) { state = Code; } // prev line accepted

		if (state == Code) {
			switch (c) {
			case '"' : state = string;
				curString = "";         continue;
			case '\'': state = Char1;   continue;
			case ';' : state = Comment; continue;
			case '\n':
			case '\\': state = Accept;
				result.push_back(line);
				line = {};              continue;
			default: line.text += c;    continue;
			}
		} else if (state == String) {
			if (c == '"') { state = Code;
				line.strings.push_back(curString);
				continue; }
			curString += parseChar(is);
		} else if (state == Char1) {
			line.characters.push_back(parseChar(is));
			state = Char2;
		} else if (state == Char2) {
			if (c != '\'') { printError("multi-char literal"); }
			state = Code;
		} else if (state == Comment) {
			if (c == '\n') { state = Accept; }
		}
	}

	return result;
}
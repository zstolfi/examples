#pragma once
#include "common.hh"
#include <istream>

namespace /*anonymous*/ {
	char hexDigit (char);
	char parseChar(std::istream& is);
}

auto preprocess(std::istream& is) {
	std::vector<Line> result {};

	// https://files.catbox.moe/971dd5.png
	enum { Code, String, Char1, Char2, Comment, Accept } state = Code;

	Line line;
	std::vector<char> curString;
	for (char c=is.get(); is; c=is.get()) {
		if (state == Accept) { state = Code; } // prev line accepted

		if (state == Code) {
			switch (c) {
			case '"' : state = String;
				curString = {};         continue;
			case '\'': state = Char1;   continue;
			case ';' : state = Comment; continue;
			case '\n':
			case '\\': state = Accept;
				result.push_back(line);
				line = {};              continue;
			default: line.text += c;    continue;
		} }
		else if (state == String) {
			if (c == '"') { state = Code;
				line.strings.push_back(curString);
				continue; }
			curString.push_back(parseChar(is));
		}
		else if (state == Char1) {
			line.characters.push_back(parseChar(is));
			state = Char2;
		}
		else if (state == Char2) {
			if (c != '\'') { printError("multi-char literal"); }
			state = Code;
		}
		else if (state == Comment) {
			if (c == '\n') { state = Accept; }
		}
	}

	if (state != Accept
	&&  state != Code  ) { printError("syntax error at EOF"); }
	return result;
}

namespace /*anonymous*/ {
	char parseChar(std::istream& is) {
		char result = 0;
		enum { Begin, Esc, Hex1, Hex2, Accept } state = Begin;

		for (char c=is.get(); is && state!=Accept; c=is.get()) {
			if (state == Begin) {
				if (c == '\\') { state = Esc; continue; }
				state = Accept;
				result = c;
			}
			else if (state == Esc) {
				switch (c) {
				case 'x' : state = Hex1 ; continue;
				state = Accept;
				case '0' : result = '\0'; continue;
				case 'a' : result = '\a'; continue;
				case 'b' : result = '\b'; continue;
				case 'e' : result = '\\'; continue;
				case 'f' : result = '\f'; continue;
				case 'n' : result = '\n'; continue;
				case 'r' : result = '\r'; continue;
				case 't' : result = '\t'; continue;
				case 'v' : result = '\v'; continue;
				case '\\': result = '\\'; continue;
				case '\'': result = '\''; continue;
				case '"' : result = '"' ; continue;
				default: printError("unknown esc sequence");
			} }
			else if (state == Hex1) {
				result = hexDigit(c);
				state = Hex2;
			}
			else if (state == Hex2) {
				result = result<<4 | hexDigit(c);
				state = Accept;
			}
		}

		if (state != Accept) { printError("missing single quote at EOF"); }
		return result;
	}

	char hexDigit(char c) {
		if ('0' <= c&&c <= '9') { return c-'0'; }
		if ('A' <= c&&c <= 'F') { return c-'A'+10; }
		if ('a' <= c&&c <= 'f') { return c-'a'+10; }
		printError("invalid hex digit"); return {};
	}
}
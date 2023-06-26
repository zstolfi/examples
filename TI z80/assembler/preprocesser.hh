#pragma once
#include "common.hh"
#include <istream>

namespace /*anonymous*/ {
	std::size_t  row = 1,  col = 1;
	int peek(std::istream& is) { return is.peek(); }
	int get( std::istream& is) {
		int c = is.get();
		if (c == '\n') { row++, col=1; } else { col++; }
		return c;
	}

	char hexDigit (char);
	char parseChar(char, std::istream&);
}

auto preprocess(std::istream& is) {
	std::vector<Line> result {};

	// https://files.catbox.moe/971dd5.png
	enum { Code, String, Char1, Char2, Comment, Accept } state = Code;

	Line line { row, col };
	std::vector<char> curString;
	for (char c=get(is); is; c=get(is)) {
		if (state == Accept) { // prev line accepted
			state = Code;
			result.push_back(line);
			line = { row, col };
		}

		if (state == Code) { std::cout << row << "\t" << col << "\t'" << c << "'\tCode\n";
			switch (c) {
			case '"' : state = String;
				curString = {};         continue;
			case '\'': state = Char1;   continue;
			case ';' : state = Comment; continue;
			case '\n':
			case '\\': state = Accept;  continue;
			default: line.text += c;    continue;
		} }
		else if (state == String) { std::cout << row << "\t" << col << "\t'" << c << "'\tString\n";
			if (c == '"') { state = Code;
				line.strings.push_back(curString);
				line.text += '"';
				continue; }
			curString.push_back(parseChar(c,is));
		}
		else if (state == Char1) { std::cout << row << "\t" << col << "\t'" << c << "'\tChar1\n";
			line.characters.push_back(parseChar(c,is));
			state = Char2;
		}
		else if (state == Char2) { std::cout << row << "\t" << col << "\t'" << c << "'\tChar2\n";
			if (c != '\'') { printError("multi-char literal"); }
				line.text += '\'';
			state = Code;
		}
		else if (state == Comment) { std::cout << row << "\t" << col << "\t'" << c << "'\tComment\n";
			if (c == '\n') { state = Accept; }
		}
	}
	result.push_back(line);

	if (state != Accept
	&&  state != Code  ) { printError("syntax error at EOF"); }
	return result;
}

namespace /*anonymous*/ {
	char parseChar(char c, std::istream& is) {
		if (c != '\\') { return c; }
		c = get(is);
		if (c == 'x') {
			char h1 = get(is);
			char h2 = get(is);
			return hexDigit(h1) << 4 | hexDigit(h2);
		}
		/**/ if (c=='0' ) { return '\0'; }
		else if (c=='a' ) { return '\a'; }
		else if (c=='b' ) { return '\b'; }
		else if (c=='e' ) { return '\\'; }
		else if (c=='f' ) { return '\f'; }
		else if (c=='n' ) { return '\n'; }
		else if (c=='r' ) { return '\r'; }
		else if (c=='t' ) { return '\t'; }
		else if (c=='v' ) { return '\v'; }
		else if (c=='\\') { return '\\'; }
		else if (c=='\'') { return '\''; }
		else if (c=='"' ) { return '"' ; }
		printError("unknown esc char"); return {};
	}

	char hexDigit(char c) {
		if ('0' <= c&&c <= '9') { return c-'0'; }
		if ('A' <= c&&c <= 'F') { return c-'A'+10; }
		if ('a' <= c&&c <= 'f') { return c-'a'+10; }
		printError("invalid hex digit"); return {};
	}
}
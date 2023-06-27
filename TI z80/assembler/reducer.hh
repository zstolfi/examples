#pragma once
#include "common.hh"
#include <string>
#include <algorithm>
#include <ranges>
namespace ranges = std::ranges;

namespace /*detail*/ {
	using integer = unsigned long;

	//integer parseExpression(const std::string&);
	integer parseInteger   (const std::string&);
}

// assembler directives:
//     .org  .origin (set program counter)
//     .db   .byte   (byte literals)
//     .dw   .word   (word literals)
//     .ds   .space  (reserve bytes)
//     .equ  =       (const assignment)
//     .end          (stop parsing)

auto& reduce(std::vector<Line>& lines) {
	// pass 0: remove empty lines
	for (auto it=lines.begin(); it!=lines.end(); /**/) {
		bool empty = !ranges::any_of(it->text,
			[](char c) { return !isWhitespace(c); }
		);
		if (empty) {
			it = lines.erase(it);
		} else { ++it; }
	}

	// pass 1: evaluate all addresses
	std::size_t programCounter = 0;

	for (Line& l : lines) {
		l.text = std::to_string(parseInteger(l.text));
	}

	return lines;
}

namespace /*detail*/ {
	integer parseInteger(const std::string& str) {
		integer result = 0;

		// Octal will not supported, because evil
		// number systems are not to be tolerated
		enum Radix { Dec=10, Hex=16, Bin=2 };
		Radix base = Dec;
		bool positive = true;

		int suffix = false;
		switch (str.back()) {
			case 'd': suffix = Dec; break;
			case 'h': suffix = Hex; break;
			case 'b': suffix = Bin; break;
		}

		int i=0;
		char c = str[i];
		if (c=='+') { /* * * * * * * */ c=str[++i]; }
		if (c=='-') { positive = false; c=str[++i]; }

		int prefix = false;
		/**/ if (c=='#') { prefix = Dec; c=str[++i]; }
		else if (c=='$') { prefix = Hex; c=str[++i]; }
		else if (c=='%') { prefix = Bin; c=str[++i]; }

		if ((prefix && suffix) && (prefix != suffix))
			printError("mismatched bases in integer");
		else if (prefix) { base = (Radix)prefix; }
		else if (suffix) { base = (Radix)suffix; }

		CharToNumber& digitB =
			  (base==Dec) ? digit
			: (base==Hex) ? hexDigit
			:/*base==Bin*/  binDigit;

		if (digitB(c) == Invalid_Digit)
			printError("digit expected in integer");

		while (digitB(c=str[i++]) != Invalid_Digit)
			result = base*result + digitB(c);

		if (!positive) { result = ~result + 1; }

		return result;
	}
}
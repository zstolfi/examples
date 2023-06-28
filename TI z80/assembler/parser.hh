#pragma once
#include "common.hh"
#include "lexer.hh"

namespace /*detail*/ {
	using integer = unsigned long;

	integer parseExpression(std::string_view);
	integer parseInteger   (std::string_view);
}

// assembler directives:
//     .org  .origin (set program counter)
//     .db   .byte   (byte literals)
//     .dw   .word   (word literals)
//     .ds   .space  (reserve bytes)
//     .equ  =       (const assignment)
//     .end          (stop parsing)

auto reduce(std::vector<Line>& lines) {
	std::vector<TokenList> result {};

	// pass 0: remove empty lines, and lex
	for (auto it=lines.begin(); it!=lines.end(); ++it) {
		if (!ranges::all_of(it->text, isWhitespace))
			result.push_back(lex(it->text));
	}
	return result;

	// pass 1: evaluate all label addresses and put them into context
	// Context ctx;
	/* ... */

	// pass 2: reduce all expressions into standard form
	for (Line& l : lines) {
		l.text = std::to_string(parseExpression(l.text));
	}

	return result;
}

namespace /*detail*/ {

	integer parseExpression(std::string_view str) {
		return parseInteger(str);
	}

	integer parseInteger(std::string_view str) {
		integer result = 0;

		// Octal will not supported, because evil
		// number systems are not to be tolerated
		enum Radix { Dec=10, Hex=16, Bin=2 };
		Radix base = Dec;

		switch (str.back()) {
			case 'd': base = Dec; break;
			case 'h': base = Hex; break;
			case 'b': base = Bin; break;
		}

		std::size_t i=0;
		char c = str[i];

		/**/ if (c=='#') { base = Dec; c=str[++i]; }
		else if (c=='$') { base = Hex; c=str[++i]; }
		else if (c=='%') { base = Bin; c=str[++i]; }

		CharToNumber& digitB =
			  (base==Dec) ? digit
			: (base==Hex) ? hexDigit
			:/*base==Bin*/  binDigit;

		if (digitB(c) == Invalid_Digit)
			PrintError("incorrect base in integer\n");

		while (digitB(c=str[i++]) != Invalid_Digit)
			result = base*result + digitB(c);

		return result;
	}
}

#pragma once
#include "common.hh"
#include <algorithm>
#include <ranges>
namespace ranges = std::ranges;
using namesace std::literals;

namespace /*detail*/ {
	using integer = unsigned long;

	integer parseExpression(std::string_view);
	integer parseInteger   (std::string_view);
	bool       isInteger   (std::string_view);
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
	for (auto it=lines.begin(); it!=lines.end(); /**/) {
		if (ranges::all_of(it->text, isWhitespace)) {
			it = lines.erase(it); // remove line if it's just whitespace
		} else {
			auto str = std::string_view(it->text);
			while (isWhitespace(str.front())) { str.remove_prefix(1); }
			while (isWhitespace(str.back ())) { str.remove_suffix(1); }
			*it = str; // reassign line with trimmed version of itself
			++it;
		}
	}

	// pass 1: evaluate all label addresses and put them into context
	Context ctx;
	/* ... */

	// pass 2: reduce all expressions into standard form
	for (Line& l : lines) {
		l.text = std::to_string(parseExpression(l.text));
	}

	return result;
}

namespace /*detail*/ {

	integer parseExpression(std::string_view str) {

	}

	bool isInteger(std::string_view str) {
		if (str.empty()) { return false; }
		if ("#$%"sv.contains(str.front())) { str.remove_prefix(1); }
		if ("dhb"sv.contains(str.back ())) { str.remove_suffix(1); }
		return str.size() && std::all_of(str, isAnyDigit);
	}

	integer parseInteger(std::string_view str) {
		integer result = 0;

		// Octal will not supported, because evil
		// number systems are not to be tolerated
		enum Radix { Dec=10, Hex=16, Bin=2 };
		Radix base = Dec;

		int suffix = false;
		switch (str.back()) {
			case 'd': suffix = Dec; break;
			case 'h': suffix = Hex; break;
			case 'b': suffix = Bin; break;
		}

		int i=0;
		char c = str[i];

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

		return result;
	}
}
#pragma once
#include "common.hh"
#include <algorithm>
#include <ranges>
namespace ranges = std::ranges;
using namespace std::literals;

namespace /*detail*/ {
	using integer = unsigned long;

	TokenList lex(std::string_view);

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

	TokenList lex(std::string_view str) {
		TokenList result {};

		auto sub = [=](auto i, auto size) { return str.substr(i,size); };

		for (std::size_t i=0, size=1; i+size <= str.size(); i+=size, size=1) {
			// whitespace
			if (isWhitespace(str[i])) { continue; }
			// identifier
			if (isLetter(str[i])) {
				while (isLetter(str[i+size]) || isDigit(str[i+size]))
					size++;
				result.emplace_back(TokenType::Identifier, sub(i,size));
				continue;
			}
			// integer
			if (isInteger(sub(i,size=1))
			||  isInteger(sub(i,size=2))) {
				while (isInteger(sub(i,size+1)) && i+size+1 <= str.size())
					size++;
				result.emplace_back(TokenType::Integer, sub(i,size));
				continue;
			}

			#define Match(S,TT)                       \
			if (sub(i,size = sizeof(S)-1) == S##sv) { \
			    result.emplace_back(TT);              \
			    continue;                             \
			}                                          
			
			Match("==", TokenType::Equal);
			Match("=" , TokenType::Assign);
			Match("**", TokenType::Exp);
			Match("*" , TokenType::Mult);
			Match("&&", TokenType::LogAnd);
			Match("&" , TokenType::BitAnd);
			Match("||", TokenType::LogOr);
			Match("|" , TokenType::BitOr);
			Match("<<", TokenType::BitLeft);
			Match("<=", TokenType::LTE);
			Match("<" , TokenType::LT);
			Match(">>", TokenType::BitRight);
			Match(">=", TokenType::GTE);
			Match(">" , TokenType::GT);
			Match("!=", TokenType::NotEqual);
			Match("!" , TokenType::LogNot);
			Match("$" , TokenType::ProgCounter);
			Match("/" , TokenType::Div);
			Match("%" , TokenType::Mod);
			Match("+" , TokenType::Plus);
			Match("-" , TokenType::Minus);
			Match("~" , TokenType::BitNot);
			Match("^" , TokenType::BitXor);
			Match("(" , TokenType::Paren0);
			Match(")" , TokenType::Paren1);

			#undef Match
			PrintWarning("unknown token\n");
		}
		return result;
	}

	integer parseExpression(std::string_view str) {
		return parseInteger(str);
	}

	bool isInteger(std::string_view str) {
		if (str.empty()) { return false; }
		char prefix = str.front(), suffix = str.back();

		if ("#$%"sv.contains(prefix))
			str.remove_prefix(1);

		if ((prefix=='#' && (suffix=='h' || suffix=='b'))
		||  (prefix=='%' && (suffix=='d' || suffix=='h')))
			PrintError("mismatched base prefix/suffix in integer\n");

		if ("dhb"sv.contains(suffix) && (prefix=='#' || prefix=='%'))
			str.remove_suffix(1);

		return str.size() && ranges::all_of(str, isAnyDigit);
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
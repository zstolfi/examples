#pragma once
#include "common.hh"
using namespace std::literals;

namespace /*detail*/ {
	bool isInteger(std::string_view);
}

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
		PrintWarning("unknown token\n");

		#undef Match
	}
	return result;
}

namespace /*detail*/ {
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
}
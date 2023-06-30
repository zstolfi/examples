#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <list>
#include <algorithm>
#include <ranges>
namespace ranges = std::ranges;

using integer = unsigned long;



// struct Scope {
// 	Sope* parent;
// 	bool isInside(Scope); // TODO
// }
// Scope* rootScope = nullptr;

struct Line {
	std::size_t row, col;
	std::string text;

	std::list<std::vector<char>> strings;
	std::list<char> characters;
//	Scope scope;
};

enum struct TokenType {
	   Identifier, Directive, Integer, String,
	// $            ,      :
	   ProgCounter, Comma, LabelDef,
	// =       (       )
	   Assign, Paren0, Paren1,
	// **   *     /    %    +     -
	   Exp, Mult, Div, Mod, Plus, Minus,
	// ==     !=        <   <=   >   >=
	   Equal, NotEqual, LT, LTE, GT, GTE,
	// !       &&      ||
	   LogNot, LogAnd, LogOr,
	// ~       &       |      ^
	   BitNot, BitAnd, BitOr, BitXor,
	// <<       >>
	   BitLeft, BitRight,
};

struct Token {
	TokenType type;
	std::string strValue;
	integer intValue = 0;
	Token(TokenType t) : type(t) {}
	Token(TokenType t, integer          val) : type(t), intValue(val) {}
	Token(TokenType t, std::string      val) : type(t), strValue(val) {}
	Token(TokenType t, std::string_view val) : type(t), strValue(val) {}
};

using TokenArray = std::vector<Token>;



constexpr int Invalid_Digit = -1;

using CharToNumber = int(char);
int digit(char c) {
	if ('0' <= c&&c <= '9') { return c-'0'; }
	return Invalid_Digit;
}
int hexDigit(char c) {
	if ('0' <= c&&c <= '9') { return c-'0'; }
	if ('A' <= c&&c <= 'F') { return c-'A'+10; }
	if ('a' <= c&&c <= 'f') { return c-'a'+10; }
	return Invalid_Digit;
}
int binDigit(char c) {
	if ('0' <= c&&c <= '1') { return c-'0'; }
	return Invalid_Digit;
}

// short-hand versions
using CharIsNumber = bool(char);
bool isDigit   (char c) { return digit(c)    != Invalid_Digit; }
bool isHexDigit(char c) { return hexDigit(c) != Invalid_Digit; }
bool isBinDigit(char c) { return binDigit(c) != Invalid_Digit; }
bool isAnyDigit(char c) { return isHexDigit(c); }



using CharPredicate = bool(char);
bool isWhitespace(char c) { return c==' ' || c=='\t' || c=='\n'; }
bool isLower (char c) { return 'a' <= c&&c <= 'z'; }
bool isUpper (char c) { return 'A' <= c&&c <= 'Z'; }
bool isLetter(char c) { return isLower(c) || isUpper(c); }

using CharTransformation = char(char);
char toLower(char c) { return isUpper(c) ? c + ('a'-'A') : c; }
char toUpper(char c) { return isLower(c) ? c + ('A'-'a') : c; }



#define DEBUG(msg) do {} while(0)
// #define DEBUG(msg)    do {std::cout << msg; } while(0)

#define  PrintStatus(msg) do { std::cerr /*           */ << msg;          } while(0)
#define PrintWarning(msg) do { std::cerr << "Warning:\t" << msg;          } while(0)
#define   PrintError(msg) do { std::cerr << "Error:  \t" << msg; exit(1); } while(0)
#pragma once
#include <iostream>
#include <string>
#include <vector>

// #define DEBUG(msg) do {} while(0)
#define DEBUG(msg)    do {std::cout << msg; } while(0)

struct Line {
	std::size_t row, col;
	std::string text;

	std::vector<std::vector<char>> strings;
	std::vector<char> characters;

};



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

bool isWhitespace(char c) { return c==' ' || c=='\t' || c=='\n'; }



void printStatus (const char* msg) { std::cerr /*           */ << msg; }
void printWarning(const char* msg) { std::cerr << "Warning:\t" << msg; }
void printError  (const char* msg) { std::cerr << "Error:  \t" << msg; exit(1); }
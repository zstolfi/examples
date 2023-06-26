#pragma once
#include <iostream>
#include <string>
#include <vector>

void printStatus (const char* msg) { std::cout                 << msg; }
void printWarning(const char* msg) { std::cout << "Warning:\t" << msg; }
void printError  (const char* msg) { std::cout << "Error  :\t" << msg; exit(1); }



struct Line {
	std::size_t row, col;
	std::string text;

	std::vector<std::string> strings;
	std::vector<char> characters;

};
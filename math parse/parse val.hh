#pragma once
#include <string>
#include <optional>
#include <functional>

template <typename T>
class MathParser {
	std::function< std::optional<T>(std::string) > parseT {};

public:
	explicit MathParser(auto&& pT) : parseT{pT} {}

	std::optional<T> eval(std::string s) {
		return parseT(s);
	}

};
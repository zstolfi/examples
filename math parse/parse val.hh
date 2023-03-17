#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <optional>
#include <functional>

#include <iostream>

template <typename Num>
class MathParser {
private:
	std::function< std::optional<Num>(std::string) > parseNum {};
	std::vector<std::string> tokens {};

public:
	explicit MathParser(auto&& pN) : parseNum{pN} {}

	std::optional<Num> eval(const std::string& s) {
		/* parentheses check */
		unsigned pOpen = 0, pClose = 0;
		for (const char c : s) {
			pOpen  += (c == '(');
			pClose += (c == ')');
		}
		if (pOpen != pClose) { return std::nullopt; }

		tokenize(s);

		for (std::string token : tokens) {
			std::cout << "'" << token << "'" << "\n";
		}

		return 0;
		// return parseNum(s);
	}

private:
	enum direction {left, right};
	const std::vector<std::tuple< std::vector<std::string>, direction >> operators {
		{{"^"}, right} ,
		{{"*", "/"}, right} ,
		{{"+", "-"}, right} ,
	};

	void tokenize(const std::string& s) {
		#define Next i += tokens.back().size(); continue;

		for (std::size_t i=0; i < s.size(); ) {
			/* whitespace */
			if (s[i] == ' ' || s[i] == '\t' || s[i] == '\n') {
				i++; continue;
			}
			/* parentheses */
			if (s[i] == '(' || s[i] == ')') {
				tokens.emplace_back(1,s[i]);
				Next;
			}
			/* operators */
			bool opMatch = false;
			for (const auto& v : operators) {
			for (const std::string& op : std::get<0>(v)) {
				if (auto check = s.substr(i, op.size()); check == op) {
					tokens.emplace_back(op);
					opMatch = true;
				}
			} }
			if (opMatch) { Next; }

			/* numbers */
			tokens.emplace_back("");
			for (std::size_t j=0; i+j < s.size() && parseNum(s.substr(i,j+1)); j++) {
				tokens.back() += s[i+j];
			}
			Next;
		}

		#undef Next
	}
};
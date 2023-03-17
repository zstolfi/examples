#pragma once
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <functional>

#include <iostream>

template <typename Num>
class MathParser {
private:
	std::function< std::optional<Num>(std::string) > parseNum {};
	std::vector<std::string> tokens {};
	enum class TokenType {num, op, paren};
	std::vector<TokenType> tokenTypes {};

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

		return evalFromTokens(0, tokens.size());
	}

private:

	struct Operator {
		std::function<Num(Num,Num)> func {};

		std::optional<Num> operator () (std::optional<Num> x, std::optional<Num> y) {
			if (x && y) { return func(*x, *y); }
			else { return std::nullopt; }
		}
	};

	// TODO: operator precedence
	// TODO: unary operators
	const std::map<std::string, Operator> operators {
		{"^", Operator {[&](Num x, Num y) -> Num { return pow(x, y); }}} ,
		{"*", Operator {[ ](Num x, Num y) -> Num { return x * y;     }}} ,
		{"/", Operator {[ ](Num x, Num y) -> Num { return x / y;     }}} ,
		{"+", Operator {[ ](Num x, Num y) -> Num { return x + y;     }}} ,
		{"-", Operator {[ ](Num x, Num y) -> Num { return x - y;     }}} ,
	};



	std::optional<Num> evalFromTokens(std::size_t begin, std::size_t end) {
		std::size_t size = end - begin;
		if (size == 0) { return std::nullopt; }
		if (size == 1 && tokenTypes[begin] != TokenType::num) { return std::nullopt; }
		if (size == 1) { return parseNum(tokens[begin]); }

		if (end - begin == 3) {
			if (tokenTypes[begin+0] == TokenType::num
			&&  tokenTypes[begin+1] == TokenType::op
			&&  tokenTypes[begin+2] == TokenType::num) {
				return operators.at("+")(parseNum(tokens[begin+0]), parseNum(tokens[begin+2]));
			}
		}

		return std::nullopt;
	}

	void tokenize(const std::string& s) {
		#define Next i += tokens.back().size(); continue;

		for (std::size_t i=0; i < s.size(); ) {
			/* whitespace */
			while (s[i] == ' ' || s[i] == '\t' || s[i] == '\n') { i++; }

			/* parentheses */
			if (s[i] == '(' || s[i] == ')') {
				tokens.emplace_back(1,s[i]);
				tokenTypes.push_back(TokenType::paren);
				Next;
			}
			/* operators */
			bool opMatch = false;
			for (const auto& [op, _ ] : operators) {
				if (auto check = s.substr(i, op.size()); check == op) {
					tokens.emplace_back(op);
					tokenTypes.push_back(TokenType::op);
					opMatch = true;
				}
			}
			if (opMatch) { Next; }

			/* numbers */
			tokens.emplace_back("");
			tokenTypes.push_back(TokenType::num);
			for (std::size_t j=0; i+j < s.size() && parseNum(s.substr(i,j+1)); j++) {
				tokens.back() += s[i+j];
			}
			Next;
		}
		#undef Next
	}



private:
	Num pow(Num v, Num w) {
		Num result = 1;
		for (;w; w = w-1) {
			result = result * v;
		}
		return result;
	}
};
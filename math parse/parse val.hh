#pragma once
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <functional>
#include <cassert>
#include <iostream>

template <typename Num>
class MathParser {
private:
	std::function< std::optional<Num>(std::string) > parseNum {};
	std::vector<std::string> tokens {};
	enum class TokenType { num, op, paren };
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

		// for (std::size_t i=0; i < tokens.size(); i++) {
		// 	std::cout << static_cast<int>(tokenTypes[i]) << " - " << tokens[i] << "\n";
		// }

		return evalFromTokens(0, tokens.size());
	}

private:

	using opID = std::string;

	struct Operator {
		int order; // using the word 'order' instead of 'precedence' for brevity
		std::function<Num(Num,Num)> func;

		std::optional<Num> operator () (std::optional<Num> x, std::optional<Num> y) const {
			if (x && y) { return func(*x, *y); }
			else { return std::nullopt; }
		}
	};

	enum Direction { left, right };

	struct OrderInfo {
		Direction direction;
	};

	const std::map<int, OrderInfo> orders {
		{1, OrderInfo{right}} ,
		{2, OrderInfo{right}} ,
		{3, OrderInfo{right}} ,
	};

	// TODO: operator precedence
	// TODO: unary operators
	const std::map<opID, Operator> operators {
		{"^", Operator {1  , [&](Num x, Num y) -> Num { return pow(x, y); }}} ,
		{"*", Operator { 2 , [ ](Num x, Num y) -> Num { return x * y; }}    } ,
		{"/", Operator { 2 , [ ](Num x, Num y) -> Num { return x / y; }}    } ,
		{"+", Operator {  3, [ ](Num x, Num y) -> Num { return x + y; }}    } ,
		{"-", Operator {  3, [ ](Num x, Num y) -> Num { return x - y; }}    } ,
	};



	struct ExprList {
		// https://files.catbox.moe/og6o90.png
		// So far only binary operators are supported.
		std::vector<std::optional<Num>> nums;
		std::vector<opID>               ops;

		void pushNum(std::optional<Num> n) {
			assert(nums.size() == ops.size());
			nums.push_back(n);
		}

		void pushOp(opID o) {
			assert(nums.size() == ops.size()+1);
			ops.push_back(o);
		}

		// Nested classes "don't know" what's inside
		// their parent class, plus I cannot for the
		// life of me make 'operators' a static obj.
		bool valid(const auto& opsList) {
			for (auto n : nums) { if (!n) { return false; } }
			for (auto o : ops) { if (!opsList.contains(o)) { return false; } }
			return true;
		}

		bool sameLevel() {
			if (ops.size() == 0) { return false; }
			for (auto o : ops) { if (o != ops[0]) { return false; } }
			return true;
		}

		void print() {
			std::cout << *nums[0] << "\t";
			for (std::size_t i=0; i < ops.size(); i++) {
				std::cout <<  ops [i]   << "\t";
				std::cout << *nums[i+1] << "\t";
			} std::cout << "\n";
		}
	};



	std::optional<Num> evalFromTokens(std::size_t begin, std::size_t end) {
		std::size_t size = end - begin;
		if (size == 0) { return std::nullopt; }
		// if (size == 1 && tokenTypes[begin] != TokenType::num) { return std::nullopt; }
		if (size == 1) { return parseNum(tokens[begin]); }

		/* create expression list */
		ExprList exprList {};

		for (std::size_t i=begin; i < end; ) {
			if (i == begin) { exprList.pushNum(evalExpr(i)); }
			exprList.pushOp(tokens[i++]);
			exprList.pushNum(evalExpr(i));
		}

		if (!exprList.valid(operators)) { return std::nullopt; }

		auto& nums = exprList.nums;
		auto& ops  = exprList.ops;

		std::cout << "evaluating exprList...\n";
		exprList.print();

		/* reduce */
		while (ops.size() > 0) {
			// TODO: respect operator direction
			/* find the highest order, then reduce */
			std::size_t iMax = 0;
			if (!exprList.sameLevel()) {
				for (std::size_t i=0; i < ops.size(); i++) {
					int orderOld = operators.at(ops[iMax]).order;
					int orderNew = operators.at(ops[i])   .order;
					if (orderNew < orderOld) { iMax = i; }
				}
			}
			Operator op = operators.at(ops[iMax]);
			nums[iMax+0] = op(*nums[iMax+0], *nums[iMax+1]);
			nums.erase(nums.begin()+iMax+1);
			ops .erase(ops .begin()+iMax);

			exprList.print();
		}

		return nums[0];
	}

	std::optional<Num> evalExpr(std::size_t& i) {
		std::optional<Num> result {};
		/**/ if (tokenTypes[i] == TokenType::num)   { result = parseNum(tokens[i]); i++; }
		else if (tokenTypes[i] == TokenType::paren) {
			std::size_t j = matchingParen(i);
			result = evalFromTokens(i+1, j);
			i = j+1;
		}
		return result;
	}

	std::size_t matchingParen(std::size_t i) {
		assert(tokens[i] == "(" || tokens[i] == ")");
		int step = (tokens[i] == "(") ? 1 : -1;
		int count = 0;
		while (true) {
			count += (tokens[i] == "(");
			count -= (tokens[i] == ")");
			if (count == 0) { return i; }
			i += step;
		}
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

			// The only way I could think of to parse the maximum size pattern
			// is to check every substring starting from the end. A "max-size"
			// term would be helpful for large strings, but it's fine for now.
			for (std::size_t j = s.size()-i; j > 0; j--) {
				if (auto num = s.substr(i,j); parseNum(num)) {
					tokens.emplace_back(num);
					tokenTypes.push_back(TokenType::num);
					break;
				}
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
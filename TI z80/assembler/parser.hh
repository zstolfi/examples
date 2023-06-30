#pragma once
#include "common.hh"
// #include "opcode table.hh"
#include <span>
#include <optional>
#include <tuple>
#include <array>
#include <stack>

namespace /*detail*/ {
	using Group = std::vector<std::size_t>;
	// Describes "grouping" of tokens
	// i.e. 1 * ( 4 + 2 )
	// =>   0 1 6 3 4 5 2

	integer parseExpression(const std::span<Token>);
	Group   makeGroups     (const std::span<Token>);
}

// assembler directives:
//     .org  .origin (set program counter)
//     .db   .byte   (byte literals)
//     .dw   .word   (word literals)
//     .ds   .space  (reserve bytes)
//     .equ  =       (const assignment)
//     .end          (stop parsing)

auto parse(std::vector<TokenArray>& lines) {
	std::vector<std::byte> result {};

	// pass 1: evaluate all label addresses and put them into context
	// Context ctx;
	/* ... */

	// pass 2: execute all statements (each is just an expression for now)
	for (TokenArray& line : lines) {
		// std::cout << parseExpression(line) << "\n";
		parseExpression(line); std::cout << "\n";
		// if (auto result = parseExpression(line))
		// 	std::cout << *result << "\n";
	}

	return result;
}

namespace /*detail*/ {
	namespace OP {
		enum Type { UnaryLeft, UnaryRight, BinaryLeft, BinaryRight };
		struct UnaryOpInfo {
			TokenType token;
			integer(*fn)(integer);
			integer operator()(integer x) const { return fn(x); }
		};

		struct BinaryOpInfo {
			TokenType token;
			integer(*fn)(integer, integer);
			integer operator()(integer x, integer y) const { return fn(x,y); }
		};

		constexpr BinaryOpInfo
			Add  {TokenType::Plus , [](auto a, auto b) { return a + b; }},
			Sub  {TokenType::Minus, [](auto a, auto b) { return a - b; }},
			Mult {TokenType::Mult , [](auto a, auto b) { return a * b; }};

		constexpr std::tuple Order {
			std::pair{BinaryLeft , std::array{Mult}},
			std::pair{BinaryLeft , std::array{Add, Sub}},
		};
	}



	integer parseExpression(/*Context& ctx, */const std::span<Token> tokens) {
		// group expressions together
		Group groups = makeGroups(tokens), newGroups = groups;
		std::vector<integer> evaluated (tokens.size());
		for (std::size_t i=0, next; i<groups.size(); i = next+1) {
			next = groups[i];
			if (tokens[i].type == TokenType::Integer)
				evaluated[i] = tokens[i].intValue;
		}

		// parentheses first
		for (std::size_t i=0, next; i<groups.size(); i = next+1) {
			next = groups[i];
			if (next-i > 0) {
				std::span<Token> inParens = tokens.subspan(i+1, next-i-1);
				integer parenValue = parseExpression(inParens);
				evaluated[i   ] = parenValue;
				evaluated[next] = parenValue;
			}
		}

		auto applyBinaryLeft = [&](const auto& ops) {
			Group newGroups = groups;
			for (std::size_t i=0, next; i<groups.size(); i = next+1) {
				next = groups[i];
				if (!(0 < i&&i < groups.size()-1)) { continue; }
				
				auto isCurrent = [&](auto op) { return op.token == tokens[i].type; };
				if (auto op = ranges::find_if(ops, isCurrent); op != ops.end()) {
					std::size_t begin = groups[i-1];
					std::size_t end   = groups[i+1];
					integer result = (*op)(evaluated[begin], evaluated[end]);
					evaluated[begin] = result;
					evaluated[end]   = result;
					newGroups[begin] = end;
					newGroups[end] = begin;
				}
			}
			groups = newGroups;
		};

		// multiply
		applyBinaryLeft(std::get<0>(OP::Order).second);
		// add/subtract
		applyBinaryLeft(std::get<1>(OP::Order).second);

		for (integer n : evaluated) { std::cout << n << " "; }
		std::cout << "| ";
		return evaluated[0];
	}

	Group makeGroups(const std::span<Token> tokens) {
		Group result (tokens.size());
		std::stack<std::size_t> parenStack;
		for (std::size_t i=0; i<tokens.size(); i++) {
			switch (tokens[i].type) {
				case TokenType::Paren0 : parenStack.push(i); break;
				case TokenType::Paren1 : result[parenStack.top()] = i;
				/*                    */ result[i] = parenStack.top();
				/*                    */ parenStack.pop(); break;
				default: result[i] = i; break;
		} }
		return result;
	}
}
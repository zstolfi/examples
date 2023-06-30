#pragma once
#include "common.hh"
// #include "opcode table.hh"
#include <span>
#include <optional>
#include <tuple>
#include <array>
#include <stack>

namespace /*detail*/ {
	integer parseExpression(const std::span<Token>);
	std::vector<std::size_t> makeGroups(const std::span<Token>);
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
		auto groups = makeGroups(tokens), newGroups = groups;
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
		// multiply
		newGroups = groups;
		for (std::size_t i=0, next; i<groups.size(); i = next+1) {
			next = groups[i];
			if (tokens[i].type == std::get<0>(OP::Order).second[0].token
			&&  0 < i&&i < groups.size()-1) {
				std::size_t begin = groups[i-1];
				std::size_t end   = groups[i+1];
				integer result = OP::Mult(evaluated[begin], evaluated[end]);
				evaluated[begin] = result;
				evaluated[end]   = result;
				newGroups[begin] = end;
				newGroups[end] = begin;
			}
		}
		groups = newGroups;
		// add/subtract
		newGroups = groups;
		for (std::size_t i=0, next; i<groups.size(); i = next+1) {
			next = groups[i];
			if ((tokens[i].type == std::get<1>(OP::Order).second[0].token
			||   tokens[i].type == std::get<1>(OP::Order).second[1].token)
			&&  0 < i&&i < groups.size()-1) {
				std::size_t begin = groups[i-1];
				std::size_t end   = groups[i+1];
				integer x = evaluated[begin];
				integer y = evaluated[end];
				integer result = 0;
				if (tokens[i].type == TokenType::Plus ) { result = OP::Add(x,y); }
				if (tokens[i].type == TokenType::Minus) { result = OP::Sub(x,y); }
				evaluated[begin] = result;
				evaluated[end]   = result;
				newGroups[begin] = end;
				newGroups[end] = begin;
			}
		}
		groups = newGroups;

		for (integer n : evaluated) { std::cout << n << " "; }
		std::cout << "| ";
		return evaluated[0];
	}

	std::vector<std::size_t> makeGroups(const std::span<Token> tokens) {
		std::vector<std::size_t> result (tokens.size());
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
#pragma once
#include "common.hh"
// #include "opcode table.hh"
#include <span>
#include <optional>
#include <tuple>
#include <array>
#include <stack>

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


	integer parseExpression(std::span<Token>);

	// You know when you use PEMDAS to simplify
	// a math equation, and you show your work?
	// This class is that work.
	struct ExpressionContext {
		std::span<Token> tokens;
		using Grouping = std::vector<std::size_t>;
		// Describes "grouping" of tokens
		// i.e. 1 * ( 4 + 2 )
		// =>   0 1 6 3 4 5 2
		Grouping groups;
		std::vector<integer> evaluated;
		ExpressionContext(std::span<Token>);
	};

	template <OP::Type OpT, typename Fn, std::size_t N>
	void applyOperations(ExpressionContext&, std::array<Fn,N>);
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
	integer parseExpression(/*Context& ctx, */std::span<Token> tokens) {
		auto ctx = ExpressionContext(tokens);

		// parentheses first
		for (std::size_t i=0, next; i<ctx.groups.size(); i = next+1) {
			next = ctx.groups[i];
			if (next-i > 0) {
				std::span<Token> inParens = tokens.subspan(i+1, next-i-1);
				integer parenValue = parseExpression(inParens);
				ctx.evaluated[i]    = parenValue;
				ctx.evaluated[next] = parenValue;
			}
		}

		// multiply
		applyOperations<OP::BinaryLeft>(ctx, std::get<0>(OP::Order).second);
		// add/subtract
		applyOperations<OP::BinaryLeft>(ctx, std::get<1>(OP::Order).second);

		for (integer n : ctx.evaluated) { std::cout << n << " "; }
		std::cout << "| ";
		return ctx.evaluated[0];
	}

	ExpressionContext::ExpressionContext(std::span<Token> tokens)
	: tokens   (tokens)
	, groups   (tokens.size())
	, evaluated(tokens.size()) {
		std::stack<std::size_t> parenStack;
		for (std::size_t i=0; i<tokens.size(); i++) {
			switch (tokens[i].type) {
				case TokenType::Paren0 : parenStack.push(i); break;
				case TokenType::Paren1 : groups[parenStack.top()] = i;
				/*                    */ groups[i] = parenStack.top();
				/*                    */ parenStack.pop(); break;
				case TokenType::Integer: evaluated[i] = tokens[i].intValue;
				default: groups[i] = i; break;
		} }
	}

	template <OP::Type OpT, typename Fn, std::size_t N>
	void applyOperations(ExpressionContext& ctx, std::array<Fn,N> ops) {
		auto newGroups = ctx.groups;
		for (std::size_t i=0, next; i<ctx.groups.size(); i = next+1) {
			next = ctx.groups[i];
			if (!(0 < i&&i < ctx.groups.size()-1)) { continue; }

			auto isCurrent = [&](auto op) { return op.token == ctx.tokens[i].type; };
			if (auto op = ranges::find_if(ops,isCurrent); op != ops.end()) {
				std::size_t begin = ctx.groups[i-1];
				std::size_t end   = ctx.groups[i+1];
				integer result = (*op)(ctx.evaluated[begin], ctx.evaluated[end]);
				ctx.evaluated[begin] = result;
				ctx.evaluated[end]   = result;
				newGroups[begin] = end;
				newGroups[end] = begin;
			}
		}
		ctx.groups = newGroups;
	};
}
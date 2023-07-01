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
		enum Assoc { Left, Right };
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
			Mult {TokenType::Mult , [](auto a, auto b) { return a * b; }},
			Exp  {TokenType::Exp  , [](auto a, auto b) {
				integer result = 1;
				while (b--) { result *= a; }
				return result;
			}};

		constexpr std::tuple Order {
			std::pair{Right, std::array{Exp}},
			std::pair{Left , std::array{Mult}},
			std::pair{Left , std::array{Add, Sub}},
		};
	}



	integer parseExpression(std::span<Token>);

	using Grouping = std::vector<std::size_t>;
	// Describes "grouping" of tokens
	// i.e. 1 * ( 4 + 2 )
	// =>   0 1 6 3 4 5 2
	template <typename Fn>
	void iterateGroups(const Grouping&, Fn);
	template <typename Fn>
	void iterateGroups(OP::Assoc dir, const Grouping&, Fn);

	struct ExpressionContext {
		// You know when you use PEMDAS to simplify
		// a math equation, and you show your work?
		// This class is that work.
		std::span<Token> tokens;
		Grouping groups;
		std::vector<integer> evaluated;
		ExpressionContext(std::span<Token>);
	};

	template <OP::Assoc OpT, typename Fn, std::size_t N>
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
	template <typename Fn>
	void iterateGroups(const Grouping& groups, Fn f) {
		for (std::size_t i=0, next; i<groups.size(); i = next+1)
			next = groups[i], f(i,next);
		}

	template <typename Fn>
	void iterateGroups(OP::Assoc dir, const Grouping& groups, Fn f) {
		if (dir == OP::Left) {
			for (std::size_t i=0, next; i<groups.size(); i = next+1)
				next = groups[i], f(i,next);
		} else {
			auto i = std::ssize(groups); i--;
			for (std::size_t prev; i>=0; i = prev-1)
				prev = groups[i], f(prev,i);
		}
	}

	integer parseExpression(/*Context& ctx, */std::span<Token> tokens) {
		auto ctx = ExpressionContext(tokens);

		// parentheses first
		iterateGroups(ctx.groups, [&](auto i, auto next) {
			if (next-i > 0) {
				std::span<Token> inParens = tokens.subspan(i+1, next-i-1);
				integer parenValue = parseExpression(inParens);
				ctx.evaluated[i]    = parenValue;
				ctx.evaluated[next] = parenValue;
			}
		});

		// exponents
		applyOperations<OP::Right>(ctx, std::get<0>(OP::Order).second);
		// multiply
		applyOperations<OP::Left >(ctx, std::get<1>(OP::Order).second);
		// add/subtract
		applyOperations<OP::Left >(ctx, std::get<2>(OP::Order).second);

		for (integer n : ctx.evaluated) { std::cout << n << " "; }
		std::cout << "| ";
		return ctx.evaluated[0];
	}

	template <OP::Assoc Dir, typename Fn, std::size_t N>
	void applyOperations(ExpressionContext& ctx, std::array<Fn,N> ops) {
		auto newGroups = ctx.groups;
		iterateGroups(Dir, ctx.groups, [&](auto i, auto) {
			if (!(0 < i&&i < ctx.groups.size()-1)) { return; }

			auto isCurrent = [&](auto op) { return op.token == ctx.tokens[i].type; };
			if (auto op = ranges::find_if(ops,isCurrent); op != ops.end()) {
				std::size_t begin = newGroups[i-1];
				std::size_t end   = newGroups[i+1];
				integer result = (*op)(ctx.evaluated[begin], ctx.evaluated[end]);
				ctx.evaluated[begin] = result;
				ctx.evaluated[end]   = result;
				newGroups[begin] = end;
				newGroups[end] = begin;
			}
		});
		ctx.groups = newGroups;
	};

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
}
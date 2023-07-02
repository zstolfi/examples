#pragma once
#include "common.hh"
#include "operators.hh"
// #include "opcode table.hh"
#include <span>
#include <optional>
#include <tuple>
#include <array>
#include <stack>

namespace OP {
	// Order of Operations table! Modify
	// it for the parser of your choice.
	constexpr std::tuple Order {
		std::pair{Right, std::array{BitNot}},
		std::pair{Right, std::array{Exp}},
		std::pair{Left , std::array{Mult, Div}},
		std::pair{Left , std::array{Add, Sub}},
	};
}

namespace /*detail*/ {
	integer parseExpression(std::span<Token>);

	using Grouping = std::vector<std::size_t>;
	// Describes "grouping" of tokens
	// i.e. 1 * ( 4 + 2 )
	// =>   0 1 6 3 4 5 2
	template <OP::Assoc, typename Fn>
	void iterateGroups(const Grouping&, Fn);
	template <typename Fn>
	void iterateGroups(const Grouping&, Fn);

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
	integer parseExpression(/*Context& ctx, */std::span<Token> tokens) {
		auto expr = ExpressionContext(tokens);

		// parentheses first
		iterateGroups(expr.groups, [&](auto i, auto next) {
			if (next-i > 0) {
				std::span<Token> inParens = tokens.subspan(i+1, next-i-1);
				integer parenValue = parseExpression(inParens);
				expr.evaluated[i]    = parenValue;
				expr.evaluated[next] = parenValue;
			}
		});

		// unary ops
		applyOperations<OP::Right>(expr, std::get<0>(OP::Order).second);
		// exponents
		applyOperations<OP::Right>(expr, std::get<1>(OP::Order).second);
		// multiply
		applyOperations<OP::Left >(expr, std::get<2>(OP::Order).second);
		// add/subtract
		applyOperations<OP::Left >(expr, std::get<3>(OP::Order).second);

		for (integer n : expr.evaluated) { std::cout << n << " "; }
		std::cout << "| ";
		return expr.evaluated[0];
	}



	template <OP::Assoc Dir, typename Fn, std::size_t N>
	void applyOperations(ExpressionContext& ctx, std::array<Fn,N> ops) {
		auto matchOp = [&ops](auto token) -> std::optional<Fn> {
			auto result = ranges::find_if(ops,
				[&](auto op) { return op.token == token.type; }
			);
			if (result != ops.end()) { return *result; }
			return {};
		};

		const std::size_t size = ctx.tokens.size();
		auto newGroups = ctx.groups;
		if constexpr (std::is_same_v<Fn, OP::UnaryOpInfo>) {
			iterateGroups<Dir>(ctx.groups, [&](auto i, auto) {
				auto op = matchOp(ctx.tokens[i]); if (!op) { return; }

				std::optional<std::size_t> prev, next;
				if (i   > 0   ) { prev = newGroups[i-1]; }
				if (i+1 < size) { next = newGroups[i+1]; }
				auto& expr = Dir==OP::Left ? prev : next;
				auto& adj  = Dir==OP::Left ? next : prev;
				if (!expr) { return; }
				if (adj && holdsIntValue(ctx.tokens[*adj].type))
					PrintError("Using unary-op like binary-op");

				integer result = (*op)(ctx.evaluated[*expr]);
				ctx.evaluated[*expr] = result;
				ctx.evaluated[i]     = result;
				newGroups[*expr] = i;
				newGroups[i] = *expr;
			});
		} else if constexpr (std::is_same_v<Fn, OP::BinaryOpInfo>) {
			iterateGroups<Dir>(ctx.groups, [&](auto i, auto) {
				auto op = matchOp(ctx.tokens[i]); if (!op) { return; }
				// Apply our operator, if it's in `ops`
				if (!(0 < i&&i < size-1)) { return; }
				// But not if we don't have both operands
				std::size_t begin = newGroups[i-1];
				std::size_t end   = newGroups[i+1];
				integer result = (*op)(ctx.evaluated[begin], ctx.evaluated[end]);
				ctx.evaluated[begin] = result;
				ctx.evaluated[end]   = result;
				newGroups[begin] = end;
				newGroups[end] = begin;
			});
		}
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



	template <OP::Assoc Dir, typename Fn>
	void iterateGroups(const Grouping& groups, Fn f) {
		if constexpr (Dir == OP::Left) {
			for (std::size_t i=0, next; i<groups.size(); i = next+1)
				next = groups[i], f(i,next);
		}
		else if constexpr (Dir == OP::Right) {
			auto i = std::ssize(groups); i--;
			for (std::size_t prev; i>=0; i = prev-1)
				prev = groups[i], f(prev,i);
		}
	}
	// iterate left-first be default
	template <typename Fn>
	void iterateGroups(const Grouping& groups, Fn f) {
		return iterateGroups<OP::Left>(groups,f);
	}
}
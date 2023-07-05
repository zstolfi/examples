#pragma once
#include "common.hh"
#include "operators.hh"
#include "opcode table.hh"
#include <span>
#include <optional>
#include <tuple>
#include <array>
#include <stack>

namespace OP {
	// Order of Operations table! Modify
	// it for the parser of your choice.
	constexpr std::tuple Order {
		std::pair{Right, std::array{Pos, Neg}},
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
		iterateGroups(expr.groups, [&](auto i) {
			auto i0=i, i1=expr.groups[i];
			// if (i0 > i1) { std::swap(i0,i1); }
			if (i0 < i1) {
				std::span<Token> inParens = tokens.subspan(i0+1, i1-i0-1);
				integer parenValue = parseExpression(inParens);
				expr.evaluated[i0] = parenValue;
				expr.evaluated[i1] = parenValue;
			}
		});

		// Compile-time iteration through Order of Operations
		// I could not get std::apply to work, as passing the
		// elements of OP::Order as parameters de-constexpr's
		// them. If I'm revisiting this code in the future of
		// C++26, I'll replace with an 'expansion statement'.
		constexpr auto Size = std::tuple_size_v<decltype(OP::Order)>;
		[&]<std::size_t...I>(std::index_sequence<I...>) {
			(applyOperations<std::get<I>(OP::Order).first>(expr, std::get<I>(OP::Order).second), ... );
		}
		(std::make_index_sequence<Size>{});

		for (integer n : expr.evaluated) { std::cout << n << " "; }
		std::cout << "| ";
		return expr.evaluated[0];
	}



	template <OP::Assoc Dir, typename Fn, std::size_t N>
	void applyOperations(ExpressionContext& ctx, std::array<Fn,N> ops) {
		auto newGroups = ctx.groups;

		auto matchOp = [&ops](auto token) -> std::optional<Fn> {
			auto result = ranges::find_if(ops,
				[&](auto op) { return op.token == token.type; }
			);
			if (result != ops.end()) { return *result; }
			return {};
		};

		auto neighbors = [&g=newGroups](auto i) {
			std::optional<std::size_t> prev, next;
			if (i   > 0       ) { prev = g[i-1]; }
			if (i+1 < g.size()) { next = g[i+1]; }
			return std::pair {prev,next};
		};

		if constexpr (std::is_same_v<Fn, OP::UnaryOpInfo>) {
			iterateGroups<Dir>(ctx.groups, [&](auto i) {
				auto op = matchOp(ctx.tokens[i]); if (!op) { return; }
				auto [prev,next] = neighbors(i);

				auto& expr = Dir==OP::Left ? prev : next;
				auto& adj  = Dir==OP::Left ? next : prev;
				// Break if there's no expr to apply to
				if (!expr) { return; }
				// Or if our op has two expr's like a binary-op
				if (adj && holdsIntValue(ctx.tokens[*adj].type)) { return; }

				integer result = (*op)(ctx.evaluated[*expr]);
				ctx.evaluated[*expr] = result;
				ctx.evaluated[i]     = result;
				newGroups[*expr] = i;
				newGroups[i] = *expr;
			} );
		} else if constexpr (std::is_same_v<Fn, OP::BinaryOpInfo>) {
			iterateGroups<Dir>(ctx.groups, [&](auto i) {
				auto op = matchOp(ctx.tokens[i]); if (!op) { return; }
				auto [prev,next] = neighbors(i);
				// Break if either operands aren't expressions
				if (prev && !holdsIntValue(ctx.tokens[*prev].type)) { return; }
				if (next && !holdsIntValue(ctx.tokens[*next].type)) { return; }

				integer result = (*op)(ctx.evaluated[*prev], ctx.evaluated[*next]);
				ctx.evaluated[*prev] = result;
				ctx.evaluated[*next]   = result;
				newGroups[*prev] = *next;
				newGroups[*next] = *prev;
			} );
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
		// Because indices are not permitted to be negative,
		// we let 0 represent one before the array beginning
		std::size_t begin = Dir==OP::Left ? 1u : groups.size();
		std::size_t end   = Dir==OP::Left ? groups.size()+1 : 0u;
		const int   step  = Dir==OP::Left ? 1 : -1;

		for (std::size_t i=begin; i!=end; ) {
			f(i-1);
			i = groups[i-1]+1 + step;
		}
	}
	// iterate left-first by default
	template <typename Fn>
	void iterateGroups(const Grouping& groups, Fn f) {
		iterateGroups<OP::Left>(groups,f);
	}
}
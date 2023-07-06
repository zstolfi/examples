#pragma once
#include "common.hh"
#include "bytes.hh"
#include "operators.hh"
#include "opcode table.hh"
#include <span>
#include <optional>
#include <tuple>
#include <array>
#include <stack>
using namespace std::literals;

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
	struct Context {
		std::size_t progCounter = 0;
		/* store variables here */
	};

	std::vector<ParamType> getParamTypes(std::span<Token>);
	// All possible ways a parameter can be interpreted
	// i.e. 128 => {n, nn, e}
	//      A   => {A, r}


	using Grouping = std::vector<std::size_t>;
	// Describes "grouping" of tokens
	// i.e. 1 * ( 4 + 2 )
	// =>   0 1 6 3 4 5 2

	struct ExpressionContext {
	// You know when you use PEMDAS to simplify
	// a math equation, and you show your work?
	// This class is that work.
		std::span<Token> tokens;
		Grouping groups;
		std::vector<integer> evaluated;
		ExpressionContext(std::span<Token>);
	};

	integer parseExpression(Context&, std::span<Token>);
	std::vector<std::span<Token>> splitArgs(std::span<Token>);

	template <OP::Assoc, typename Fn>
	void iterateGroups(const Grouping&, Fn);
	template <typename Fn>
	void iterateGroups(const Grouping&, Fn);
	template <OP::Assoc Dir, typename Fn, std::size_t N>
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
	// TODO: Syntax checks
	Context ctx;

	auto append = [&](const auto& bytes) {
		result.insert(result.end(), bytes.begin(), bytes.end());
		ctx.progCounter += bytes.size();
	};

	// pass 1: evaluate all label addresses and put them into context
	//         also determine each TYPE of opcode (to know it's size)
	for (TokenArray& line : lines) {
		if (line[0].type == TokenType::Directive) {
			if (isAny(line[0].strValue, "org","origin")) {
				ctx.progCounter = parseExpression(ctx, {++line.begin(), line.end()});
				continue;
			}
			if (isAny(line[0].strValue, "db","byte")) {
				auto params = splitArgs({++line.begin(), line.end()});
				for (auto param : params) {
					integer val = parseExpression(ctx, param);
					append(Bytes(val));
				}
				continue;
			}
			if (isAny(line[0].strValue, "dw","word")) {
				auto params = splitArgs({++line.begin(), line.end()});
				for (auto param : params) {
					integer val = parseExpression(ctx, param);
					append(Bytes<Word::LSB>(val));
				}
				continue;
			}
		}

		if (line[0].type == TokenType::Identifier) {
			if (OpCodeTable.contains(line[0].strValue)) {
				auto args = splitArgs({++line.begin(), line.end()});
				std::vector<ParamType> paramTypes0, paramTypes1;
				if (args.size() >= 1) { paramTypes0 = getParamTypes(args[0]); }
				if (args.size() >= 2) { paramTypes1 = getParamTypes(args[1]); }
				if (args.size() >= 3) { PrintWarning("too many opcode arguments\n"); }

				auto [begin,end] = OpCodeTable.equal_range(line[0].strValue);
				auto opcode = std::find_if(begin, end,
					// [&](auto op) { return op.second.pt0==param0 && op.second.pt1==param1; }
					// TODO: iterate through all possible pairs of
					//       paramters to find the matching opcode
					[&](auto op) { return true; }
				);
				if (opcode == end) { PrintError("unknown opcode arguments"); }

				append(Bytes(0x00));
				continue;
			}
		}
	}

	// pass 2: execute all statements (each is just an expression for now)
	for (TokenArray& line : lines) {
		// DEBUG(parseExpression(line) << "\n");
		integer value = parseExpression(ctx, line);
		PrintStatus(value << "\n");
		// if (auto result = parseExpression(line))
		// 	DEBUG(*result << "\n");
	}

	return result;
}

namespace /*detail*/ {
	std::vector<std::span<Token>> splitArgs(std::span<Token> tokens) {
	// trailing commas are permitted
		std::vector<std::span<Token>> result {};
		for (auto i=tokens.begin(), j=tokens.begin(); j!=tokens.end(); ++j) {
			if (j->type == TokenType::Comma) {
				result.push_back({i,j});
				i = std::next(j);
			} else if (std::next(j) == tokens.end()) {
				result.push_back({i,tokens.end()});
			}
		}
		return result;
	}

	std::vector<ParamType> getParamTypes(std::span<Token> t) {
		std::vector<ParamType> result {};
		using PT = ParamType;
		if (t.size()==1 && t[0].type == TokenType::Identifier) {
			if (isAny(t[0].strValue, "a","b","c","d","e","h","l"))
				result.push_back(PT::r);
			if (t[0].strValue == "a" ) { result.push_back(PT::A ); }
			if (t[0].strValue == "hl") { result.push_back(PT::HL); }
			if (t[0].strValue == "ix") { result.push_back(PT::IX); }
			if (t[0].strValue == "iy") { result.push_back(PT::IY); }
			if (isAny(t[0].strValue, "nz","z","nc","c","po","pe","p","m"))
				result.push_back(PT::cc);
			if (isAny(t[0].strValue, "bc","de","hl","af")) { result.push_back(PT::qq); }
			if (isAny(t[0].strValue, "bc","de","hl","sp")) { result.push_back(PT::ss); }
			if (isAny(t[0].strValue, "bc","de","ix","sp")) { result.push_back(PT::pp); }
			if (isAny(t[0].strValue, "bc","de","iy","sp")) { result.push_back(PT::rr); }
		}
		else if (t.size()==1 && holdsIntValue(t[0].type)) {
			// integer n = parseExpression(/* ... */);
			integer n = t[0].intValue;
			if (0 <= n&&n <= 7    ) { result.push_back(PT::b ); }
			if (0 <= n&&n <= 255  ) { result.push_back(PT::n ); }
			if (0 <= n&&n <= 65535) { result.push_back(PT::nn); }
			if (-128 <= (signed)n&&n <= 127) { result.push_back(PT::d); }
			if (-126 <= (signed)n&&n <= 129) { result.push_back(PT::e); }
		}
		else {
			if (t.front().type == TokenType::Paren0
			&&  t.back ().type == TokenType::Paren1
			&&  t[1].type == TokenType::Identifier) {
				if (t.size() == 3) {
					if (t[1].strValue == "hl") { result.push_back(PT::HL_d); }
					if (t[1].strValue == "ix") { result.push_back(PT::IX_d); }
					if (t[1].strValue == "iy") { result.push_back(PT::IY_d); }
				}
				if (t[1].strValue == "ix" && t[2].type == TokenType::Plus)
					result.push_back(PT::IX_d);
				if (t[1].strValue == "iy" && t[2].type == TokenType::Plus)
					result.push_back(PT::IY_d);
			}
		}
		return result;
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
				default: groups[i] = i; break;
		} }
	}

	integer parseExpression(Context& ctx, std::span<Token> tokens) {
		auto expr = ExpressionContext(tokens);

		// parentheses / literals first
		iterateGroups(expr.groups, [&](auto i) {
			auto [i0, i1] = std::minmax(i, expr.groups[i]);
			if (i0 < i1) {
				std::span<Token> inParens = tokens.subspan(i0+1, i1-i0-1);
				integer parenValue = parseExpression(ctx, inParens);
				expr.evaluated[i0] = parenValue;
				expr.evaluated[i1] = parenValue;
			} else {
				if (tokens[i].type == TokenType::Integer)
					expr.evaluated[i] = tokens[i].intValue;
				else if (tokens[i].type == TokenType::Dollar)
					expr.evaluated[i] = ctx.progCounter;
			}
		});

		// Compile-time iteration through Order of Operations
		// I could not get std::apply to work, as passing the
		// elements of OP::Order as parameters de-constexpr's
		// them. If I'm revisiting this code in the future of
		// C++26, I'll replace with an 'expansion statement'.
		constexpr auto Size = std::tuple_size_v<decltype(OP::Order)>;
		[&]<std::size_t...I>(std::index_sequence<I...>) {
			(
				applyOperations
				<std::get<I>(OP::Order).first>
				(expr, std::get<I>(OP::Order).second)
			, ... );
		}
		(std::make_index_sequence<Size>{});

		return expr.evaluated[0];
	}



	template <OP::Assoc Dir, typename Fn, std::size_t N>
	void applyOperations(ExpressionContext& expr, std::array<Fn,N> ops) {
		auto newGroups = expr.groups;

		auto matchOp = [&ops](auto token) -> std::optional<Fn> {
			auto result = ranges::find_if(ops,
				[&](auto op) { return op.token == token; }
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
			iterateGroups<Dir>(expr.groups, [&](auto i) {
				auto op = matchOp(expr.tokens[i].type);
				auto [prev,next] = neighbors(i);
				if (!op) { return; }

				auto& num = Dir==OP::Left ? prev : next;
				auto& adj = Dir==OP::Left ? next : prev;
				// Break if there's no expr to apply to
				if (!num) { return; }
				// Or if our op has two expr's like a binary-op
				if (adj && holdsIntValue(expr.tokens[*adj].type)) { return; }

				integer result = (*op)(expr.evaluated[*num]);
				expr.evaluated[*num] = result;
				expr.evaluated[i]    = result;
				newGroups[*num] = i;
				newGroups[i] = *num;
			} );
		} else if constexpr (std::is_same_v<Fn, OP::BinaryOpInfo>) {
			iterateGroups<Dir>(expr.groups, [&](auto i) {
				auto op = matchOp(expr.tokens[i].type);
				auto [prev,next] = neighbors(i);
				if (!op) { return; }
				// Break if either operands aren't expressions
				if (prev && !holdsIntValue(expr.tokens[*prev].type)) { return; }
				if (next && !holdsIntValue(expr.tokens[*next].type)) { return; }

				integer result = (*op)(expr.evaluated[*prev], expr.evaluated[*next]);
				expr.evaluated[*prev] = result;
				expr.evaluated[*next]   = result;
				newGroups[*prev] = *next;
				newGroups[*next] = *prev;
			} );
		}
		expr.groups = newGroups;
	};



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
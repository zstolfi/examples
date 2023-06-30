#pragma once
#include "common.hh"
// #include "opcode table.hh"
#include <span>
#include <optional>
#include <tuple>
#include <array>

namespace /*detail*/ {
	integer parseExpression(std::size_t& i, std::span<Token>);
	integer parseAddSub    (std::size_t& i, std::span<Token>);
	integer parseMult      (std::size_t& i, std::span<Token>);
	integer parsePrimary   (std::size_t& i, std::span<Token>);
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
		std::size_t i=0;
		integer result = parseExpression(i,line);
		if (i != line.size()) { std::cout << "!" << i << " "; }
		std::cout << result << "\n";
	}

	return result;
}

namespace /*detail*/ {
	namespace OP {
		enum Type { UnaryLeft, UnaryRight, BinaryLeft, BinaryRight };

		integer Add (integer a, integer b) { return a + b; }
		integer Sub (integer a, integer b) { return a - b; }
		integer Mult(integer a, integer b) { return a * b; }

		constexpr std::tuple Order {
			std::pair{BinaryLeft , std::array{Add, Sub}},
			std::pair{BinaryLeft , std::array{Mult}},
		};
	}

	integer parseExpression(/*Context& ctx, */std::size_t& i, std::span<Token> tokens) {
		return parseAddSub(i,tokens);
	}

	integer parseAddSub(std::size_t& i, std::span<Token> tokens) {
		integer result = parseMult(i,tokens);
		while (tokens[i].type == TokenType::Plus
		||     tokens[i].type == TokenType::Minus) {
			TokenType oper = tokens[i++].type;
			integer expr = parseMult(i,tokens);
			if (oper == TokenType::Plus ) { result += expr; }
			if (oper == TokenType::Minus) { result -= expr; }
		}
		return result;
	}
	integer parseMult(std::size_t& i, std::span<Token> tokens) {
		integer result = parsePrimary(i,tokens);
		while (tokens[i].type == TokenType::Mult) {
			TokenType oper = tokens[i++].type;
			integer expr = parsePrimary(i,tokens);
			if (oper == TokenType::Mult ) { result *= expr; }
		}
		return result;
	}
	integer parsePrimary(std::size_t& i, std::span<Token> tokens) {
		if (tokens[i].type == TokenType::Integer) { return tokens[i++].intValue; }
		if (tokens[i].type == TokenType::Paren0) { i++;
			integer result = parseExpression(i,tokens);
			if (tokens[i++].type != TokenType::Paren1) { /*Error*/ }
			return result;
		}
		/*Error*/ return {};
	}
}
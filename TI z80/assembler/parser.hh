#pragma once
#include "common.hh"
// #include "opcode table.hh"
#include <span>

namespace /*detail*/ {
	integer parseExpression(std::span<Token>);
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
		std::cout << parseExpression(line) << "\n";
	}

	return result;
}

namespace /*detail*/ {

	integer parseExpression(std::span<Token> tokens) {
		Token t = tokens[0];
		if (t.type == TokenType::Integer)
			return std::get<integer>(t.value);
		return {};
	}
}
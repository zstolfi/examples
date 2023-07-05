#pragma once
#include "common.hh"

namespace /*detail*/ {
	enum ParamType { _ , reg8, reg16, n, nn,/* ... */};
	using ParamVal = unsigned char;

	template <std::size_t N>
	struct OpCode {
		std::string_view name;
		ParamType pt0;
		ParamType pt1;
		using OpFn = std::array<std::byte,N> (ParamVal, ParamVal);
		OpFn* f;
		const std::size_t size;
	};
}

// https://www.zilog.com/docs/z80/um0080.pdf
constexpr OpCodeTable std::array {
	OpCode{"ld", reg8, reg8, [](auto r0, auto r1){ return {0x40|r0<<3|r1}; }},
	OpCode{"ld", reg8, n   , [](auto r , auto n ){ return {0x06|r<<3, n} ; }},
	OpCode{"ld", reg8, HL_d, [](auto r , auto   ){ return {0x46|r<<3}    ; }},
}
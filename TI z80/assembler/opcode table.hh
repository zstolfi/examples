#pragma once
#include "common.hh"
#include "bytes.hh"

namespace /*detail*/ {
	enum ParamType { _ , reg8, reg16, n, nn, HL_d, IX_d, IY_d };
	using ParamVal = unsigned char;
	using OpFn = std::vector<std::byte> (ParamVal, ParamVal);

	struct OpCode {
		std::string_view name;
		ParamType pt0;
		ParamType pt1;
		std::size_t size;
		OpFn* fn;

		std::vector<std::byte> operator()(ParamVal x, ParamVal y) {
			return fn(x,y);
		}
	};

	template <typename... Args>
	auto BytesVec(Args... args) {
	    auto arr = std::array{ std::byte(args) ... };
	    return std::vector(arr.begin(), arr.end());
	}
}

// https://www.zilog.com/docs/z80/um0080.pdf
constexpr auto OpCodeTable = std::array {
	OpCode{"ld"  , reg8, reg8, 1, [](auto r0, auto r1){ return BytesVec(0x40|r0<<3|r1); }},
	OpCode{"ld"  , reg8, n   , 2, [](auto r , auto n ){ return BytesVec(0x06|r<<3, n); }},
	OpCode{"ld"  , reg8, HL_d, 1, [](auto r , auto   ){ return BytesVec(0x46|r<<3); }},
	OpCode{"ld"  , reg8, IX_d, 3, [](auto r , auto d ){ return BytesVec(0xDD, 0x46|r<<3, d); }},
	OpCode{"ld"  , reg8, IY_d, 3, [](auto r , auto d ){ return BytesVec(0xFD, 0x46|r<<3, d); }},
	OpCode{"nop" , _   , _   , 1, [](auto   , auto   ){ return BytesVec(0x00); }},
};
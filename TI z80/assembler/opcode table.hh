#pragma once
#include <map>
#include "common.hh"
#include "bytes.hh"

// Z80 CPU User Manual pg. 39
enum struct ParamType { none, _ = none,
	A, HL, IX, IY,    // exaclt register names
	HL_d, IX_d, IY_d, // indirection
	r,                // 8-bit registers
	qq, ss, pp, rr,   // 16-bit reg. groups
	n, nn, d, b, e,   // number types
	cc,               // conditions
	AF_p, IMn, RSTn   // sporadic few
	};
using ParamVal = unsigned char;

namespace /*detail*/ {
	using enum ParamType;
	using OpFn = std::vector<std::byte> (ParamVal, ParamVal);

	struct OpCode {
		ParamType pt0;
		ParamType pt1;
		std::size_t size;
		OpFn* fn;
		auto operator()(ParamVal x, ParamVal y) const { return fn(x,y); }
	};

	template <typename... Args>
	auto BytesVec(Args... args) {
	    auto arr = std::array{ std::byte(args)... };
	    return std::vector(arr.begin(), arr.end());
	}
}

// Z80 CPU User Manual pg. 69
const std::multimap<std::string_view, OpCode> OpCodeTable  {
	{{"ld"  }, { r   , r   , 1, [](auto r0, auto r1){ return BytesVec(0x40|r0<<3|r1);      } }},
	{{"ld"  }, { r   , n   , 2, [](auto r , auto n ){ return BytesVec(0x06|r<<3, n);       } }},
	{{"ld"  }, { r   , HL_d, 1, [](auto r , auto   ){ return BytesVec(0x46|r<<3);          } }},
	{{"ld"  }, { r   , IX_d, 3, [](auto r , auto d ){ return BytesVec(0xDD, 0x46|r<<3, d); } }},
	{{"ld"  }, { r   , IY_d, 3, [](auto r , auto d ){ return BytesVec(0xFD, 0x46|r<<3, d); } }},
	{{"nop" }, { _   , _   , 1, [](auto   , auto   ){ return BytesVec(0x00);               } }},
};



// Z80 CPU User Manual can be found here:
//     https://www.zilog.com/docs/z80/um0080.pdf
#pragma once
#include <vector>
#include <string_view>
#include <algorithm>
#include <concepts>
#include <iostream>
#include <compare>
#include <ranges>
namespace ranges = std::ranges;

/* Base-256 Integer Class */
class Dense {
	using UINT = unsigned long long;
	using Digit = unsigned char;
	std::vector<Digit> digits;

public:
	/* Construction & Assignment */
	Dense()                            = default;
	Dense(const Dense&)                = default;
	Dense(Dense&&) noexcept            = default;
	Dense& operator=(const Dense&)     = default;
	Dense& operator=(Dense&&) noexcept = default;

	Dense(UINT n) { *this = n; }
	Dense& operator=(UINT n) {
		digits.clear();
		while (n) digits.push_back(n%256), n/=256;
		return *this;
	}

	// Dense(std::string_view s) {
	// 	for (std::size_t i=s.size(); i-- > 0; ) {
	// 		if ('0' <= s[i]&&s[i] <= '9') *this = 10*(*this) + (s[i]-'0');
	// 	}
	// }

	/* Comparison */
	std::strong_ordering operator<=>(const Dense& other) const {
		const auto& a = digits, &b = other.digits;
		if (a.size() != b.size()) return a.size() <=> b.size();
		if (a == b) return std::strong_ordering::equal;
		return ranges::lexicographical_compare(
			ranges::reverse_view {a},
			ranges::reverse_view {b})
		? std::strong_ordering::less
		: std::strong_ordering::greater;
	}

	bool operator==(const Dense& other) const {
		return digits == other.digits;
	}

	explicit operator bool() const { return *this != Dense {}; }

	/* Arithmetic */
	Dense& operator+=(const Dense& other) {
		const auto& a = digits, &b = other.digits;
		const auto size = std::max(a.size(), b.size());
		auto result = Dense {};
		unsigned carry = 0;
		for (std::size_t i=0; i<size; i++) {
			unsigned n = (i < a.size()? a[i]: 0)
			+            (i < b.size()? b[i]: 0)
			+            carry;
			result.digits.push_back(n%256);
			carry = n/256;
		}
		if (carry) result.digits.push_back(carry);
		return *this = std::move(result);
	}

	Dense& operator-=(const Dense& other) {
		if (*this <= other) return *this = Dense {};
		*this += ~other;
		digits.pop_back(), canonicalize();
		return ++*this;
	}

	Dense& operator++() {
		std::size_t i = 0;
		while (i < digits.size() && digits[i] == 255) i++;
		if (i == digits.size()) digits.push_back(1); else digits[i]++;
		while (i > 0) digits[--i] = 0;
		return *this;
	}
	
	Dense& operator--() {
		if (*this == Dense {}) return *this;
		std::size_t i = 0;
		while (i < digits.size() && digits[i] == 0) i++;
		digits[i]--;
		while (i > 0) digits[--i] = 255;
		return canonicalize();
	}

	/* Bitwise */
	Dense& operator^=(const Dense& other) {
		const auto& a = digits, &b = other.digits;
		const auto size = std::max(a.size(), b.size());
		for (std::size_t i=0; i<size; i++) {
			Digit d = (i < a.size()? a[i]: 0) ^ (i < b.size()? b[i]: 0);
			if (i == a.size()) digits.push_back(d); else digits[i] = d;
		}
		return canonicalize();
	}

	Dense& operator&=(const Dense& other) {
		const auto& a = digits, &b = other.digits;
		const auto size = std::max(a.size(), b.size());
		for (std::size_t i=0; i<size; i++) {
			Digit d = (i < a.size()? a[i]: 0) & (i < b.size()? b[i]: 0);
			if (i == a.size()) digits.push_back(d); else digits[i] = d;
		}
		return canonicalize();
	}

	Dense& operator|=(const Dense& other) {
		const auto& a = digits, &b = other.digits;
		const auto size = std::max(a.size(), b.size());
		for (std::size_t i=0; i<size; i++) {
			Digit d = (i < a.size()? a[i]: 0) | (i < b.size()? b[i]: 0);
			if (i == a.size()) digits.push_back(d); else digits[i] = d;
		}
		return canonicalize();
	}

	Dense operator~() const {
		Dense result {*this};
		for (Digit& d : result.digits) d = ~d;
		return result.canonicalize();
	}

	Dense& operator<<=(UINT n) {
		digits.resize(digits.size() + (n+7)/8, 0);
		for (std::size_t i=8*digits.size(); i-- > 0; ) {
			setBit(i, (i>=n)? getBit(i-n): 0);
		}
		return canonicalize();
	}

	Dense& operator>>=(UINT n) {
		for (std::size_t i=0; i<8*digits.size(); i++) {
			setBit(i, getBit(i+n));
		}
		return canonicalize();
	}

	/* Derived */
	friend Dense operator+(Dense a, const Dense& b) { a += b; return a; }
	friend Dense operator-(Dense a, const Dense& b) { a -= b; return a; }
	// friend Dense operator*(Dense a, const Dense& b) { a *= b; return a; }
	// friend Dense operator/(Dense a, const Dense& b) { a /= b; return a; }
	// friend Dense operator%(Dense a, const Dense& b) { a %= b; return a; }
	Dense operator++(int) { Dense old = *this; ++(*this); return old; }
	Dense operator--(int) { Dense old = *this; --(*this); return old; }
	friend Dense operator^(Dense a, const Dense& b) { a ^= b; return a; }
	friend Dense operator&(Dense a, const Dense& b) { a &= b; return a; }
	friend Dense operator|(Dense a, const Dense& b) { a |= b; return a; }
	friend Dense operator<<(Dense a, UINT b) { a <<= b; return a; }
	friend Dense operator>>(Dense a, UINT b) { a >>= b; return a; }

	/* Derived Foreign */
	Dense& operator+=(UINT n) { return *this += Dense {n}; }
	Dense& operator-=(UINT n) { return *this -= Dense {n}; }
	Dense& operator^=(UINT n) { return *this ^= Dense {n}; }
	Dense& operator&=(UINT n) { return *this &= Dense {n}; }
	Dense& operator|=(UINT n) { return *this |= Dense {n}; }

	friend Dense operator+(Dense a, UINT b) { a += b; return a; }
	friend Dense operator-(Dense a, UINT b) { a -= b; return a; }
	// friend Dense operator*(Dense a, UINT b) { a *= b; return a; }
	// friend Dense operator/(Dense a, UINT b) { a /= b; return a; }
	// friend Dense operator%(Dense a, UINT b) { a %= b; return a; }
	friend Dense operator^(Dense a, UINT b) { a ^= b; return a; }
	friend Dense operator&(Dense a, UINT b) { a &= b; return a; }
	friend Dense operator|(Dense a, UINT b) { a |= b; return a; }

	friend Dense operator+(UINT a, const Dense& b) { return Dense {a} + b; }
	friend Dense operator-(UINT a, const Dense& b) { return Dense {a} - b; }
	// friend Dense operator*(UINT a, const Dense& b) { return Dense {a} * b; }
	// friend Dense operator/(UINT a, const Dense& b) { return Dense {a} / b; }
	// friend Dense operator%(UINT a, const Dense& b) { return Dense {a} % b; }
	friend Dense operator^(UINT a, const Dense& b) { return Dense {a} ^ b; }
	friend Dense operator&(UINT a, const Dense& b) { return Dense {a} & b; }
	friend Dense operator|(UINT a, const Dense& b) { return Dense {a} | b; }

	/* IO */
	// TODO: optimize by not copying 'number'.
	friend std::ostream& operator<<(std::ostream& os, Dense number) {
		// while (number) os << number%10, number/=10;
		for (int d : ranges::reverse_view {number.digits}) os << d << " ";
		return os;
	}

	friend std::istream& operator>>(std::istream& is, Dense& number) {
		// auto isDigit = [](auto c) { return '0' <= c&&c <= '9'; };
		// if (!isDigit(is.peek())) is.setstate(std::ios::failbit);
		// number.digits.clear();
		// while (isDigit(is.peek())) number = 10*number + (is.get()-'0');
		return is;
	}

private:
	Dense& canonicalize() {
		while (!digits.empty() && digits.back() == 0) digits.pop_back();
		return *this;
	}

	bool getBit(std::size_t i) {
		return (i/8 < digits.size()) ? digits[i/8] >> (i%8) & 1 : 0;
	}

	Dense& setBit(std::size_t i, bool b) {
		if (i/8 >= digits.size()) digits.resize(i/8 + 1, 0);
		digits[i/8] = digits[i/8] & ~(1<<(i%8)) | b<<(i%8);
		return canonicalize();
	}
};

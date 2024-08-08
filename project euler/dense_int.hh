#pragma once
#include <vector>
#include <string_view>
#include <algorithm>
#include <concepts>
#include <iostream>
#include <compare>
#include <functional>
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

	Dense(std::string_view str) {
		for (char c : str) if ('0' <= c&&c <= '9') {
			*this = 10*(*this) + (c-'0');
		}
	}

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
		unsigned carry = 0;
		for (std::size_t i=0; i<size; i++) {
			unsigned n = getDigit(i) + other.getDigit(i) + carry;
			setDigit(i, n%256);
			carry = n/256;
		}
		if (carry) setDigit(digits.size(), carry);
		return *this;
	}

	Dense& operator-=(const Dense& other) {
		if (*this <= other) return *this = Dense {};
		return *this = ~(~(*this) + other);
	}

	Dense& operator*=(const Dense& other) {
		Dense result {};
		for (std::size_t i=0; Digit d : other.digits) {
			Dense product {};
			for (std::size_t j=0; j<8; j++) if (d>>j & 1) product += *this << j;
			result += product << 8*i++;
		}
		return *this = std::move(result);
	}

	Dense& operator/=(const Dense& d) { return *this = divMod(d).first;  }
	Dense& operator%=(const Dense& d) { return *this = divMod(d).second; }

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
	Dense& operator^=(const Dense& b) { return digitOp<std::bit_xor<void>>(b); }
	Dense& operator&=(const Dense& b) { return digitOp<std::bit_and<void>>(b); }
	Dense& operator|=(const Dense& b) { return digitOp<std::bit_or <void>>(b); }

	Dense operator~() const {
		Dense result {*this};
		for (Digit& d : result.digits) d = ~d;
		return result.canonicalize();
	}

	Dense& operator<<=(UINT n) {
		if (n%8 == 0) for (;n; n-=8) digits.insert(digits.begin(), 0);
		else {
			digits.resize(digits.size() + (n+7)/8, 0);
			for (std::size_t i=8*digits.size(); i-- > 0; ) {
				setBit(i, (i>=n)? getBit(i-n): 0);
			}
		}
		return canonicalize();
	}

	Dense& operator>>=(UINT n) {
		if (n%8 == 0) for (;n; n-=8) digits.erase(digits.begin());
		else {
			for (std::size_t i=0; i<8*digits.size(); i++) {
				setBit(i, getBit(i+n));
			}
		}
		return canonicalize();
	}

	/* Derived */
	friend Dense operator+(Dense a, const Dense& b) { a += b; return a; }
	friend Dense operator-(Dense a, const Dense& b) { a -= b; return a; }
	friend Dense operator*(Dense a, const Dense& b) { a *= b; return a; }
	friend Dense operator/(Dense a, const Dense& b) { a /= b; return a; }
	friend Dense operator%(Dense a, const Dense& b) { a %= b; return a; }
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
	friend Dense operator*(Dense a, UINT b) { a *= b; return a; }
	friend Dense operator/(Dense a, UINT b) { a /= b; return a; }
	friend Dense operator%(Dense a, UINT b) { a %= b; return a; }
	friend Dense operator^(Dense a, UINT b) { a ^= b; return a; }
	friend Dense operator&(Dense a, UINT b) { a &= b; return a; }
	friend Dense operator|(Dense a, UINT b) { a |= b; return a; }

	friend Dense operator+(UINT a, const Dense& b) { return Dense {a} + b; }
	friend Dense operator-(UINT a, const Dense& b) { return Dense {a} - b; }
	friend Dense operator*(UINT a, const Dense& b) { return Dense {a} * b; }
	friend Dense operator/(UINT a, const Dense& b) { return Dense {a} / b; }
	friend Dense operator%(UINT a, const Dense& b) { return Dense {a} % b; }
	friend Dense operator^(UINT a, const Dense& b) { return Dense {a} ^ b; }
	friend Dense operator&(UINT a, const Dense& b) { return Dense {a} & b; }
	friend Dense operator|(UINT a, const Dense& b) { return Dense {a} | b; }

	/* IO */
	template <std::integral I>
	explicit operator I() {
		I result = 0;
		for (Digit d : digits) result = 256*result + d;
		return result;
	}

	// TODO: optimize by not copying 'number'.
	friend std::ostream& operator<<(std::ostream& os, Dense number) {
		// std::vector<char> result {};
		// for (;number; number/=10) result.push_back(char {number%10} + '0');
		// if (result.empty()) result = {'0'};
		// ranges::copy(
		// 	ranges::reverse_view {result},
		// 	std::ostream_iterator<char> {os}
		// );
		os << number.digits.size();
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

	Digit getDigit(std::size_t i) const {
		return i < digits.size()? digits[i]: 0;
	}

	Dense& setDigit(std::size_t i, Digit d) {
		if (i >= digits.size()) digits.resize(i+1, 0);
		digits[i] = d;
		return canonicalize();
	}

	bool getBit(std::size_t i) const {
		return (i/8 < digits.size()) ? digits[i/8] >> (i%8) & 1 : 0;
	}

	Dense& setBit(std::size_t i, bool b) {
		if (i/8 >= digits.size()) digits.resize(i/8 + 1, 0);
		digits[i/8] = digits[i/8] & ~(1<<(i%8)) | b<<(i%8);
		return canonicalize();
	}

	template <class Op>
	Dense& digitOp(const Dense& other) {
		const auto& a = digits, &b = other.digits;
		for (std::size_t i=0; i<a.size() || i<b.size(); i++) {
			setDigit(i, Op{}(getDigit(i), other.getDigit(i)));
		}
		return canonicalize();
	}

	// https://en.wikipedia.org/wiki/Long_division#Algorithm_for_arbitrary_base
	std::pair<Dense, Dense> divMod(const Dense& other) const {
		const auto& a = digits, &b = other.digits;
		if (other == Dense {}) return {};
		if (a.size() < b.size()) return {0, *this};
		std::size_t size = a.size() - b.size() + 1;
		Dense q {}, r = *this >> 8*size;
		for (std::size_t i=size; i-- > 0; ) {
			Dense d = r << 8 | getDigit(i);
			for (unsigned β=0; β<256; β++) {
				Dense rNew = d-other*β;
				if (rNew < other) {
					q = q << 8 | β;
					r = rNew;
					break;
				}
			}
		}
		return {q, r};
	}
};

#pragma once
#include <vector>
#include <algorithm>
#include <concepts>
#include <iostream>
#include <compare>
#include <ranges>
namespace ranges = std::ranges;

/* Base-256 Integer Class */
class Dense {
	using Digit = unsigned char;
	std::vector<Digit> digits;

public:
	/* Construction & Assignment */
	Dense()                            = default;
	Dense(const Dense&)                = default;
	Dense(Dense&&) noexcept            = default;
	Dense& operator=(const Dense&)     = default;
	Dense& operator=(Dense&&) noexcept = default;

	Dense(std::unsigned_integral auto n) { *this = n; }
	Dense& operator=(std::unsigned_integral auto n) {
		digits.clear();
		while (n) digits.push_back(n%256), n/=256;
		return *this;
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
		if (*this == 0u) return *this;
		std::size_t i = 0;
		while (i < digits.size() && digits[i] == 0) i++;
		digits[i]--;
		while (i > 0) digits[--i] = 255;
		return canonicalize();
	}

	friend Dense operator+(Dense a, const Dense& b) { a += b; return a; }
	friend Dense operator-(Dense a, const Dense& b) { a -= b; return a; }
	// friend Dense operator*(Dense a, const Dense& b) { a *= b; return a; }
	// friend Dense operator/(Dense a, const Dense& b) { a /= b; return a; }
	// friend Dense operator%(Dense a, const Dense& b) { a %= b; return a; }
	Dense operator++(int) { Dense old = *this; ++(*this); return old; }
	Dense operator--(int) { Dense old = *this; --(*this); return old; }

private:
	Dense& canonicalize() {
		while (!digits.empty() && digits.back() == 0) digits.pop_back();
		return *this;
	}
};

/* IO */
// TODO: optimize by not copying 'number'.
// std::ostream& operator<<(std::ostream& os, Dense number) {
// 	while (number) os << number%10, number/=10;
// 	return os;
// }

// std::istream& operator>>(std::istream& is, Dense& number) {
// 	auto isDigit = [](auto c) { return '0' <= c&&c <= '9'; };
// 	if (!isDigit(is.peek())) is.setstate(std::ios::failbit);
// 	while (isDigit(is.peek())) number = 10*number + (is.get()-'0');
// 	return is;
// }

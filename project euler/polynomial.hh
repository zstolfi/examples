#pragma once
#include <vector>
#include <algorithm>
#include <ranges>
#include <concepts>
namespace ranges = std::ranges;

template <class Number>
class Polynomial {
	std::vector<Number> coefficients {Number {0}};

public:
	Polynomial(ranges::input_range auto&& v)
	: coefficients{ranges::begin(v), ranges::end(v)} {
		canonicalize();
	}

	Number operator()(Number x) const {
		Number result {0}, power {1};
		for (Number c : coefficients) {
			result += power * c;
			power *= x;
		}
		return result;
	}

	Polynomial() = default;
	Polynomial(const Polynomial&) = default;
	Polynomial& operator=(const Polynomial&) = default;

	const auto& getCoefficients() const { return coefficients; }

private:
	Polynomial& canonicalize() {
		while (coefficients.back() == Number {0}) coefficients.pop_back();
		if (coefficients.empty()) coefficients.push_back(Number {0});
		return *this;
	}
};

template <ranges::input_range IR>
Polynomial(IR&&) -> Polynomial<ranges::range_value_t<IR>>;

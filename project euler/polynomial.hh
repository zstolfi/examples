#pragma once
#include <vector>
#include <algorithm>
#include <ranges>
namespace ranges = std::ranges;

#ifdef LAGRANGE_INTERPOLATION
	#include <Eigen/Core>
	#include <Eigen/LU>
	static struct FromLagrange_tag {} FromLagrange;
#endif

template <class Number>
class Polynomial {
	std::vector<Number> coefficients {Number {0}};

public:
	Polynomial(ranges::input_range auto&& v)
	: coefficients{ranges::begin(v), ranges::end(v)} {
		canonicalize();
	}

	#ifdef LAGRANGE_INTERPOLATION
		Polynomial(FromLagrange_tag, ranges::input_range auto&&);
	#endif

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

#ifdef LAGRANGE_INTERPOLATION
	template <class Number>
	Polynomial<Number>::Polynomial(FromLagrange_tag,
		ranges::input_range auto&& points
	) {
		const std::size_t N = ranges::size(points);
		auto a = [&](auto i) { return points[i][0]; };
		auto b = [&](auto i) { return points[i][1]; };

		// https://math.stackexchange.com/a/3253643
		auto coef = [&](std::size_t ci) -> Number {
			Eigen::MatrixX<Number> num {N,N}, den {N,N};
			for (std::size_t i=0; i<N; i++) {
			for (std::size_t j=0; j<N; j++) {
				num(i,j) = (i==0)? Number {1}: a(j) * num(i-1,j);
				den(i,j) = (i==0)? Number {1}: a(j) * den(i-1,j);
				if      (i == N-1) num(i,j) = b(j);
				else if (i == ci)  num(i,j) *= a(j);
			} }
			return (N-ci & 1? 1: -1) * num.determinant() / den.determinant();
		};

		coefficients.clear();
		for (std::size_t i=0; i<N; i++) coefficients.push_back(coef(i));
		canonicalize();
	}

	template <ranges::input_range IR>
	Polynomial(FromLagrange_tag, IR&&)
	-> Polynomial<ranges::range_value_t<ranges::range_value_t<IR>>>;
#endif

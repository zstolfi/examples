#pragma once
#include <iostream>
#include <cmath>
#include <algorithm>
#include <array>
#include <vector>
#include <concepts>
#include <type_traits>
#include <initializer_list>
#include <cassert>
#include <ranges>
namespace stdr = std::ranges;
namespace stdv = std::views;

/* ~~ API Tags ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

constexpr struct FromSimplex_Arg {} FromSimplex {};

/* ~~ Concepts ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

template <class T>
concept Arithmetic = requires(T a, T b) {
	(double)a;
	a <=> b;
	{-a} -> std::convertible_to<T>;
	{a += b}; {a + b} -> std::convertible_to<T>;
	{a -= b}; {a - b} -> std::convertible_to<T>;
	{a *= b}; {a * b} -> std::convertible_to<T>;
	{a /= b}; {a / b} -> std::convertible_to<double>;
};

template <class T, class U>
concept RangeOf = stdr::input_range<T>
&& std::convertible_to<stdr::range_value_t<T>, U>;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

template <stdr::input_range R>
bool permutationParity(R&& perm) {
	// https://stackoverflow.com/a/79832177
	assert(stdr::size(perm) <= 8*sizeof(unsigned));
	unsigned count {}, mask {};
	for (unsigned i : perm) {
		unsigned bit = 1 << i;
		if (mask & bit) count++;
		mask ^= bit - 1;
	}
	return count % 2;
}

// TODO: Merge this with the Matrix class.
template <
	stdr::input_range R1, stdr::input_range R2, class F,
	class T = std::invoke_result_t<F, std::size_t, std::size_t>
>
T determinant(R1&& rows, R2&& cols, F&& elementAccess) {
	assert(stdr::size(rows) == stdr::size(cols));
	std::size_t const N = stdr::size(rows);
	T sum {0};
	// Iterate permutations.
	std::vector<std::size_t> perm (N);
	for (std::size_t i=0; i<N; i++) perm[i] = i;
	do {
		T product {1};
		for (std::size_t i=0; i<N; i++) {
			product *= elementAccess(rows[i], cols[perm[i]]);
		}
		sum += permutationParity(perm)? -product: product;
	} while (stdr::next_permutation(perm).found);
	return sum;
}

/* ~~ Coordinate Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

// TODO: move the type T to the end and have it default to double.
template <Arithmetic T, std::size_t N>
class Coordinate {
	std::array<T, N> components {};

public:
	auto operator<=>(Coordinate const&) const = default;
	auto const& operator[](std::size_t i) const { return components.at(i); }
	auto /* */& operator[](std::size_t i) /* */ { return components.at(i); }

	template <std::convertible_to<T> ... Ts>
	Coordinate(Ts ... args) requires(sizeof ... (Ts) == N) {
		components = {static_cast<T>(args) ... };
	}

	template <stdr::input_range R=std::initializer_list<T>>
	Coordinate(R&& input) {
		assert(stdr::size(input) == N);
		stdr::copy(input, stdr::data(components));
	}

	Coordinate() = default;
	Coordinate(Coordinate const& ) = default;
	Coordinate(Coordinate /* */&&) = default;
	Coordinate& operator=(Coordinate const& ) = default;
	Coordinate& operator=(Coordinate /* */&&) = default;

	Coordinate operator+() {
		Coordinate result {*this};
		for (std::size_t i=0; i<N; i++) result[i] = +result[i];
		return result;
	}

	Coordinate operator-() {
		Coordinate result {*this};
		for (std::size_t i=0; i<N; i++) result[i] = -result[i];
		return result;
	}

	Coordinate& operator+=(Coordinate const& rhs) {
		for (std::size_t i=0; i<N; i++) (*this)[i] += rhs[i];
		return *this;
	}

	friend Coordinate operator+(Coordinate lhs, Coordinate const& rhs) {
		lhs += rhs;
		return lhs;
	}

	Coordinate& operator-=(Coordinate const& rhs) {
		for (std::size_t i=0; i<N; i++) (*this)[i] -= rhs[i];
		return *this;
	}

	friend Coordinate operator-(Coordinate lhs, Coordinate const& rhs) {
		lhs -= rhs;
		return lhs;
	}

	Coordinate& operator*=(T scalar) {
		for (std::size_t i=0; i<N; i++) (*this)[i] *= scalar;
		return *this;
	}

	friend Coordinate operator*(Coordinate lhs, T scalar) {
		lhs *= scalar;
		return lhs;
	}

	friend Coordinate operator*(T scalar, Coordinate rhs) {
		rhs *= scalar;
		return rhs;
	}

	Coordinate& operator/=(T scalar) {
		for (std::size_t i=0; i<N; i++) (*this)[i] /= scalar;
		return *this;
	}

	friend Coordinate operator/(Coordinate lhs, T scalar) {
		lhs /= scalar;
		return lhs;
	}

	friend T dot(Coordinate const& lhs, Coordinate const& rhs) {
		T result {0};
		for (std::size_t i=0; i<N; i++) result += lhs[i] * rhs[i];
		return result;
	}

	friend T length(Coordinate const& c) {
		// TODO: Figure out sqrt with custom types.
		return std::sqrt(dot(c, c));
	}

	// Gram determinant.
	template <RangeOf<Coordinate> R>
	friend T measureSquared(R&& input) {
		std::size_t k = stdr::size(input);
		assert(k <= N);
		return determinant(
			stdv::iota(0u, k),
			stdv::iota(0u, k),
			[&](auto i, auto j) { return dot(input[i], input[j]); }
		);
	}

	// 1-distance, 2-area, 3-volume ... of parallelotope defined by our input.
	template <RangeOf<Coordinate> R>
	friend T measure(R&& input) {
		return std::sqrt(measureSquared(input));
	}

	// Same as above, but for a simplicial corner of the parallelotope.
	template <RangeOf<Coordinate> R>
	friend T volumeSquared(R&& input) {
		return measureSquared(input) / (factorial(N) * factorial(N));
	}

	// Same as above, but for a simplicial corner of the parallelotope.
	template <RangeOf<Coordinate> R>
	friend T volume(R&& input) {
		return std::sqrt(measureSquared(input)) / factorial(N);
	}

	// Generalized collinearity, coplanarity, etc.
	template <RangeOf<Coordinate> R>
	friend bool independent(R&& input, T tolerance=1e-6) {
		if (stdr::size(input) > N) return false;
		return measureSquared(input) > tolerance*tolerance;
	}

	// Acts like the Hodge star on an (N-1)-vector defined by our input.
	template <RangeOf<Coordinate> R>
	friend Coordinate orthogonal(R&& input) {
		assert(stdr::size(input) == N-1);
		std::vector<Coordinate> matrix {stdr::begin(input), stdr::end(input)};
		// Generalized determinant definition of the cross product.
		Coordinate result {};
		for (std::size_t i=0; i<N; i++) {
			auto skip_i = [=](auto j) { return j + (j>=i); };
			result[i] = determinant(
				stdv::iota(0u, N-1),
				stdv::iota(0u, N-1) | stdv::transform(skip_i),
				[&](auto i, auto j) { return matrix[i][j]; }
			);
			result[i] *= (N%2 == i%2)? -1: 1;
		}
		return result;
	}

	// Gram-Schmidt orthonormalization
	template <RangeOf<Coordinate> R>
	friend std::vector<Coordinate> orthonormalize(R&& input) {
		assert(stdr::size(input) <= N);
		std::vector<Coordinate> result {stdr::begin(input), stdr::end(input)};
		for (std::size_t i=0; i<result.size(); i++) {
			for (std::size_t j=i+1; j<result.size(); j++) {
				result[j] -= project(result[i], result[j]);
			}
		}
		for (Coordinate& basis : result) basis /= length(basis);
		return result;
	}

/* ~~ Simplex API ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

	template <RangeOf<Coordinate> R>
	friend T measureSquared(FromSimplex_Arg, R&& input) {
		return measureSquared(fromSimplex(input));
	}

	template <RangeOf<Coordinate> R>
	friend T measure(FromSimplex_Arg, R&& input) {
		return measure(fromSimplex(input));
	}

	template <RangeOf<Coordinate> R>
	friend T volumeSquared(FromSimplex_Arg, R&& input) {
		return volumeSquared(fromSimplex(input));
	}

	template <RangeOf<Coordinate> R>
	friend T volume(FromSimplex_Arg, R&& input) {
		return volume(fromSimplex(input));
	}

	template <RangeOf<Coordinate> R>
	friend bool independent(FromSimplex_Arg, R&& input, T tolerance=1e-6) {
		return independent(fromSimplex(input), tolerance);
	}

	template <RangeOf<Coordinate> R>
	friend Coordinate orthogonal(FromSimplex_Arg, R&& input) {
		return orthogonal(fromSimplex(input));
	}

	template <RangeOf<Coordinate> R>
	friend std::vector<Coordinate> orthonormalize(FromSimplex_Arg, R&& input) {
		return orthonormalize(fromSimplex(input));
	}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

private:
	template <stdr::input_range R>
	static std::vector<Coordinate> fromSimplex(R&& input) {
		assert(stdr::size(input) <= N+1);
		std::vector<Coordinate> result {};
		for (Coordinate c : input | stdv::drop(1)) {
			result.push_back(c - *stdr::begin(input));
		}
		return result;
	}

	static inline Coordinate project(Coordinate u, Coordinate v) {
		return dot(v, u) / dot(u, u) * u;
	}

	static consteval T factorial(unsigned x) {
		T result {1};
		while (x) result *= x--;
		return result;
	}

public:
	using value_type = T;
	static constexpr std::size_t dimension = N;

	template <std::size_t M>
	using WithDimension = Coordinate<T, M>;

	// TODO: Try and make these get's non-member.
	template <std::size_t I>
	friend auto&& get(Coordinate const&  c) { return c.components[I]; }
	template <std::size_t I>
	friend auto&& get(Coordinate /* */&  c) { return c.components[I]; }
	template <std::size_t I>
	friend auto&& get(Coordinate /* */&& c) { return c.components[I]; }
};

template <Arithmetic T, std::size_t N>
struct std::tuple_size<Coordinate<T, N>>
: std::integral_constant<size_t, N> {};

template <std::size_t I, Arithmetic T, std::size_t N>
struct std::tuple_element<I, Coordinate<T, N>>
: std::type_identity<T> { static_assert(I < N); };

/* ~~ Matrix Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

template <Arithmetic T, std::size_t M, std::size_t N>
class Matrix {
	std::array<Coordinate<T, N>, M> elements {};

public:
	auto operator<=>(Matrix const&) const = default;
	auto const& operator[](std::size_t i) const { return elements.at(i); }
	auto /* */& operator[](std::size_t i) /* */ { return elements.at(i); }

	template <stdr::input_range R>
	Matrix(R&& input) {
		for (std::size_t i=0; i<M; i++)
		for (std::size_t j=0; j<N; j++) {
			elements[i][j] = input[i][j];
		}
	}

	Matrix() = default;
	Matrix(Matrix const& ) = default;
	Matrix(Matrix /* */&&) = default;
	Matrix& operator=(Matrix const& ) = default;
	Matrix& operator=(Matrix /* */&&) = default;

	Matrix<T, N, M> transpose() const {
		Matrix<T, N, M> result {};
		for (std::size_t i=0; i<M; i++)
		for (std::size_t j=0; j<N; j++) {
			result[j][i] = (*this)[i][j];
		}
		return result;
	}

	friend Coordinate<T, M> operator*(Matrix const& m, Coordinate<T, N> c) {
		Coordinate<T, M> result {};
		for (std::size_t i=0; i<M; i++) {
			result[i] = dot(m[i], c);
		}
		return result;
	}

	// This linear algebra class is a stub. You can help zstolfi by expanding it.
};

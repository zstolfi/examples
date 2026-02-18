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

constexpr struct FromSimplex_Arg {} FromSimplex {};

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

template <Arithmetic T, std::size_t N>
class Coordinate {
	std::array<T, N> components {};

public:
	auto operator<=>(Coordinate const&) const = default;
	T const& operator[](std::size_t i) const { return components.at(i); }
	T /* */& operator[](std::size_t i) /* */ { return components.at(i); }

	template <std::convertible_to<T> ... Ts>
	Coordinate(Ts ... args) requires (sizeof ... (Ts) == N) {
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

	friend Coordinate operator/(Coordinate const& lhs, T scalar) {
		lhs /= scalar;
		return lhs;
	}

	friend T dot(Coordinate const& lhs, Coordinate const& rhs) {
		T result {0};
		for (std::size_t i=0; i<N; i++) result += lhs[i] * rhs[i];
		return result;
	}

	// Acts like the Hodge star on an (N-1)-vector defined by our input.
	template <stdr::input_range R=std::initializer_list<Coordinate>>
	friend Coordinate orthogonal(R&& input) {
		assert(stdr::size(input) == N-1);
		// Generalized determinate definition of the cross product.
		Coordinate result {};
		for (std::size_t i=0; i<N; i++) {
			std::array<std::size_t, N-1> indexOrder {};
			for (std::size_t j=0; j<N-1; j++) indexOrder[j] = j + (j>=i);
			T sum {0};
			// Iterate permutations.
			do {
				bool parity = false;
				T product {1};
				for (std::size_t j=0; auto const& coord : input) {
					product *= coord[indexOrder[j++]];
					if (j < N-1) parity ^= indexOrder[j-1] > indexOrder[j];
				}
				sum += parity? -product: product;
			} while (stdr::next_permutation(indexOrder).found);
			result[i] = (N%2 == i%2)? -sum: sum;
		}
		return result;
	}

	// Gram-Schmidt orthonormalization
	template <stdr::input_range R=std::initializer_list<Coordinate>>
	friend std::vector<Coordinate> orthonormalize(R&& input) {
		assert(stdr::size(input) <= N);
		std::vector<Coordinate> result {stdr::begin(input), stdr::end(input)};
		for (std::size_t i=0; i<result.size(); i++) {
			for (std::size_t j=i+1; j<result.size(); j++) {
				result[j] -= project(result[i], result[j]);
			}
		}
//		for (Coordinate& basis : result) basis /= length(basis);
		return result;
	}

	template <stdr::input_range R=std::initializer_list<Coordinate>>
	friend Coordinate orthogonal(FromSimplex_Arg, R&& input) {
		return orthogonal(fromSimplex(input));
	}

	template <stdr::input_range R=std::initializer_list<Coordinate>>
	friend std::vector<Coordinate> orthonormalize(FromSimplex_Arg, R&& input) {
		return orthonormalize(fromSimplex(input));
	}

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

public:
	using value_type = T;
	static constexpr std::size_t dimension = N;

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

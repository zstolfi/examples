#pragma once
#include <vector>
#include <algorithm>
#include <iostream>

template <typename T>
constexpr T pow(T a, unsigned b) {
	T result {1};
	for (unsigned i=0; i < b; i++) {
		result *= a;
	}
	return result;
}

template <typename scalar>
class Polynomial {
	std::vector<scalar> data = {0};

private:
	void clean() { // called when an operator might result in a 0 polynomial
		bool allZeros = true;
		for (auto c : data) { allZeros &= (c == 0); }
		if (allZeros) {
			data = {0};
		} else {
			while (data.back() == 0) { data.pop_back(); }
		}
	}
public:
	Polynomial() : data{0} {}
	Polynomial(scalar c) : data{c} {}
	Polynomial(std::initializer_list<scalar> l) : data{l} {
		if (data.size() == 0) { data = {0}; }
		clean();
	}

	const auto& raw() const { return data; }

	const unsigned degree() const { return data.size()-1; }

#if 0
	// straight forward method
	scalar operator () (scalar x) {
		scalar result = 0;
		for (unsigned i=0; i < data.size(); i++) {
			result += data[i] * pow(x,i);
		}
		return result;
	}
#else
	// Horner's method
	scalar operator () (scalar x) {
		scalar result = 0;
		for (unsigned i = data.size()-1; i > 0; i--) {
			result += data[i];
			result *= x;
		}
		result += data[0];
		return result;
	}
#endif


	/* UNARY MINUS */
	Polynomial<scalar>& operator - () {
		for (auto& c : data) { c = -c; }
		return *this;
	}

	/* ADDITION */
	template <typename T> requires std::convertible_to<T,scalar>
	Polynomial<scalar>& operator += (const T n) {
		data[0] += n;
		clean();
		return *this;
	}

	template <typename T> requires std::convertible_to<T,scalar>
	Polynomial<scalar>& operator += (const Polynomial<T>& that) {
		const auto& thatData = that.raw();
		if (that.degree() > this->degree()) {
			this->data.resize(thatData.size());
		}
		for (unsigned i=0; i < this->data.size(); i++) {
			this->data[i] += thatData[i];
		}
		clean();
		return *this;
	}

	/* SUBTRACTION */
	template <typename T> requires std::convertible_to<T,scalar>
	Polynomial<scalar>& operator -= (const T n) {
		data[0] -= n;
		clean();
		return *this;
	}

	template <typename T> requires std::convertible_to<T,scalar>
	Polynomial<scalar>& operator -= (const Polynomial<T>& that) {
		const auto& thatData = that.raw();
		if (that.degree() > this->degree()) {
			this->data.resize(thatData.size());
		}
		for (unsigned i=0; i < this->data.size(); i++) {
			this->data[i] -= thatData[i];
		}
		clean();
		return *this;
	}

	/* MULTIPLICATION */
	template <typename T> requires std::convertible_to<T,scalar>
	Polynomial<scalar>& operator *= (const T n) {
		for (auto& c : data) {
			c *= n;
		}
		clean();
		return *this;
	}

	template <typename T> requires std::convertible_to<T,scalar>
	Polynomial<scalar>& operator *= (const Polynomial<T>& that) {
		unsigned degA = this->degree(), degB = that.degree();
		auto  thisData = this->data; // must be a copy
		auto& thatData = that.raw();

		unsigned newDegree = degA + degB;
		this->data.resize(newDegree + 1);
		// https://www.desmos.com/calculator/3bioj6t7dh
		for (unsigned i=0; i < this->data.size(); i++) {
			data[i] = 0;
			unsigned j0 = std::max<signed>(0, i - degB);
			unsigned j1 = std::min<signed>(degA, i);
			for (unsigned j=j0; j <= j1; j++) {
				unsigned k = i-j;
				data[i] += thisData[j] * thatData[k];
			}
		}
		clean();
		return *this;
	}

	/* DIVISION */
	template <typename T> requires std::convertible_to<T,scalar>
	Polynomial<scalar>& operator /= (const T n) {
		*this *= 1.0/n;
		return *this;
	}

	// Polynomial& operator /= (const Polynomial& that);
	// left as an exercise to the reader ;)
};

/* BINARY OPERATORS */
// Poly + n
template <typename T, typename U> requires std::convertible_to<U,T>
Polynomial<T> operator + (Polynomial<T> a, const U b)
	{ a += b; return a; }
// n + Poly
template <typename T, typename U> requires std::convertible_to<U,T>
Polynomial<T> operator + (const U b, Polynomial<T> a)
	{ a += b; return a;}
// Poly + Poly
template <typename T, typename U> requires std::convertible_to<U,T>
Polynomial<T> operator + (Polynomial<T> a, const Polynomial<U>& b)
	{ a += b; return a; }

// Poly - n
template <typename T, typename U> requires std::convertible_to<U,T>
Polynomial<T> operator - (Polynomial<T> a, const U b)
	{ a -= b; return a; }
// n - Poly
template <typename T, typename U> requires std::convertible_to<U,T>
Polynomial<T> operator - (const U b, Polynomial<T> a)
	{ a -= b; return a;}
// Poly - Poly
template <typename T, typename U> requires std::convertible_to<U,T>
Polynomial<T> operator - (Polynomial<T> a, const Polynomial<U>& b)
	{ a -= b; return a; }

// Poly * n
template <typename T, typename U> requires std::convertible_to<U,T>
Polynomial<T> operator * (Polynomial<T> a, const U b)
	{ a *= b; return a; }
// n * Poly
template <typename T, typename U> requires std::convertible_to<U,T>
Polynomial<T> operator * (const U b, Polynomial<T> a)
	{ a *= b; return a;}
// Poly * Poly
template <typename T, typename U> requires std::convertible_to<U,T>
Polynomial<T> operator * (Polynomial<T> a, const Polynomial<U>& b)
	{ a *= b; return a; }

// Poly / n
template <typename T, typename U> requires std::convertible_to<U,T>
Polynomial<T> operator / (Polynomial<T> a, const U b)
	{ a /= b; return a; }

/* PRINTING */
template <typename T>
std::ostream& operator << (std::ostream& os, const Polynomial<T>& p) {
	const auto& data = p.raw();

	if (data.size() == 1) { os << data[0]; return os; }
	for (unsigned i=0; i < data.size(); i++) {
		if (data[i] == 0) { continue; }
		if (i == 0) {
			os << data[i];
		} else {
			if (data[i] != 1) { os << data[i] << "*"; }
			if (i > 0) { os << "x"; }
			if (i > 1) { os << "^" << i; }
		}
		if (i+1 < data.size()) { os << " + "; }
	}
	return os;
}

Polynomial<double> x = {0,1};
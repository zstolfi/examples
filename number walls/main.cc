#include <iostream>
#include <vector>
#include <functional>
#include <optional>
// #include <mdspan>
#include <cassert>

template <typename T>
class NumberWall {
	using brick = std::optional<T>; // haha, bricks, get it? :^)
	std::function<T(int)> sequence {};
	std::vector<brick> wall {};

	const unsigned w, h;	// with, height
	const int x0, x1;   	// x bounds
	const int y0, y1;   	// y bounds

public:
	NumberWall(std::function<T(int)> s, int x, unsigned w, unsigned h)
	: sequence{s}, w{w}, h{h}
	, x0{x}, x1 {x+w}
	, y0{0}, y1 {0+h}  {
		wall.resize(w*h, std::nullopt);
		
		
	}

	const std::vector<T>& data() { return wall; }
	const unsigned width () { return w; }
	const unsigned height() { return h; }



	/* Calculation */
	void calculate() {
		auto get = [&](int m, int n) -> brick {
			const unsigned i = m, j = n-x0;
			// if (m >= 0) { std::cout << m << "," << n << "\t" << i << ", " << j << "\n"; }
			/**/ if (m <= -2) { return 0; }
			else if (m == -1) { return 1; }
			else if (0 <= j&&j < w && i < h) { return wall[i*w+j]; }
			else { return std::nullopt; }
		};

		auto set = [&](int m, int n, brick value) {
			const unsigned i = m, j = n-x0;
			// assert(0 <= i&&i < h);
			// assert(0 <= j&&j < w);
			wall[i*w+j] = value;
		};

		for (int n=x0; n < x1; n++) {
			wall[n-x0] = sequence(n);
		}

		for (int m=y0; m < y1; m++) {
		for (int n=x0; n < x1; n++) {
			// https://files.catbox.moe/vlh72e.png
			brick a0 = get(m-1, n);
			brick a1 = get(m+1, n);
			brick b0 = get(m, n-1);
			brick b1 = get(m, n+1);
			brick c  = get(m, n);

			if (!a0 + !a1 + !b0 + !b1 + !c == 1) { // 1 unknown
				/**/ if (!a0 && a1 != 0) { set(m-1, n, ((*c)*(*c) - (*b0)*(*b1)) / (*a1)); }
				else if (!a1 && a0 != 0) { set(	m+1, n, ((*c)*(*c) - (*b0)*(*b1)) / (*a0)); }
				else if (!b0 && b1 != 0) { set(m, n-1, ((*c)*(*c) - (*a0)*(*a1)) / (*b1)); }
				else if (!b1 && b0 != 0) { set(m, n+1, ((*c)*(*c) - (*a0)*(*a1)) / (*b0)); }
			}
		} }
	}


	/* Print Functions */
	void print() {
		for (unsigned i=0; i < h; i++) {
			for (unsigned j=0; j < w; j++) {
				if (auto value = wall[i*w+j]) {
					std::cout << *value;
				} else {
					std::cout << "*";
				}
				if (j+1 <= w) { std::cout << "\t"; }
			}
			std::cout << "\n";
		}
	}

	void printPattern(std::function<bool(T)> f) {
		for (unsigned i=0; i < h; i++) {
			for (unsigned j=0; j < w; j++) {
				if (auto value = wall[i*w+j]; value || f(*value)) {
					std::cout << "##";
				} else {
					std::cout << "  ";
				}
			}
			std::cout << "\n";
		}
	}
};



#include <gmpxx.h>
#include "fib.hh"
using INT = mpz_class;


int main() {
	NumberWall<INT> squares {
		// [](auto n) -> INT { return n*n; } ,
		[](auto n) -> INT { return Fib(n); } ,
		5 ,
		10, 5
	};

	squares.calculate();
	squares.print();

	return 0;
}
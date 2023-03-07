#include <iostream>
#include <vector>
#include <functional>
#include <optional>
// #include <mdspan>
#include <cassert>

template <typename T>
class NumberWall {
private:
	using brick = std::optional<T>; // haha, bricks, get it? :^)
	struct window { int size, m, n; };
	std::function<T(int)> sequence {};
	std::vector<brick> wall {};

	const unsigned w, h;	// with, height
	const int x0, x1;   	// x bounds
	const int y0, y1;   	// y bounds

	std::vector<window> zeroWindows {};
	// std::vector<window> partialZeroWindows {};

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



public:
	/* Calculation */
	void calculate() {
		// setup
		for (int n=x0; n < x1; n++)
			wall[n-x0] = sequence(n);

		//run the cross rule
		crossRule();
		findZeroWindows();
		fillZeroWindows();
	}



private:
	/* Calculation Methods */
	void crossRule() {
		for (int m=y0; m < y1; m++) {
		for (int n=x0; n < x1; n++) {
			// https://files.catbox.moe/vlh72e.png
			brick a0 = get(m-1, n);
			brick a1 = get(m+1, n);
			brick b0 = get(m, n-1);
			brick b1 = get(m, n+1);
			brick c  = get(m, n);

			if (!a0 + !a1 + !b0 + !b1 + !c == 1) { // 1 unknown
				/**/ if (!a0 && a1 != 0) { set(m-1, n, T{((*c)*(*c) - (*b0)*(*b1)) / (*a1)}); }
				else if (!a1 && a0 != 0) { set(m+1, n, T{((*c)*(*c) - (*b0)*(*b1)) / (*a0)}); }
				else if (!b0 && b1 != 0) { set(m, n-1, T{((*c)*(*c) - (*a0)*(*a1)) / (*b1)}); }
				else if (!b1 && b0 != 0) { set(m, n+1, T{((*c)*(*c) - (*a0)*(*a1)) / (*b0)}); }
			}
		} }
	}

	void findZeroWindows() {
		for (int m=y0; m < y1; m++) {
		for (int n=x0; n < x1; n++) {
			brick c  = get(m,n);
			brick a  = get(m-1, n);
			brick b0 = get(m, n-1);
			brick b1 = get(m, n+1);
			if (( c  && *c  == 0)
			&&  ( a  && *a  != 0)
			&&  (!b0 || *b0 != 0)) {
				window win {1, m, n};
				while (b1 && b1 == 0) {
					win.size++;
					n++;
					b1 = get(m, n+1);
				}
				zeroWindows.push_back(win);
			}
		} }
	}

	void fillZeroWindows() {
		for (auto& win : zeroWindows) {
			for (int m =win.m; m < win.m + win.size; m++) {
			for (int n =win.n; n < win.n + win.size; n++) {
				set(m, n, T{0});
			} }
		}
	}



private:
	/* Internal Functions */
	brick get(int m, int n) {
		const unsigned i = m, j = n-x0;
		/**/ if (m <= -2) { return T{0}; }
		else if (m == -1) { return T{1}; }
		else if (0 <= j&&j < w && i < h) { return wall[i*w+j]; }
		else { return std::nullopt; }
	};

	void set(int m, int n, brick value) {
		const unsigned i = m, j = n-x0;
		if ((0 <= i&&i < h) && (0 <= j&&j < w)) {
			wall[i*w+j] = value;
		}
	};



public:
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
				if (auto value = wall[i*w+j]; value && f(*value)) {
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
#include "sequences.hh"
using INT = mpz_class;


int main() {
	NumberWall<INT> squares {
		// [](auto n) -> INT { return n*n; } ,
		// [](auto n) -> INT { return n*n*n; } ,
		// Fib ,
		// seq_ZeroWindows ,
		[](auto n) {
			const std::vector<INT> seq = {1,0,0,0,0,0,0,0,1};
			return (0 <= n&&n < seq.size()) ? seq[n] : 0;
		} ,
		0 ,
		14, 10
	};

	squares.calculate();
	squares.print();
	squares.printPattern([](auto n) { return true; });

	return 0;
}
#include <iostream>
#include <vector>
#include <functional>
#include <optional>
// #include <mdspan>
#include <cassert>

#include <cmath>
#include <algorithm>

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
		#define Print_Check() \
			std::cout << numCount() << "\t" << zeroWindows.size() << "\n"; \
			print();
		Print_Check();
		// setup
		for (int n=x0; n < x1; n++)
			wall[n-x0] = sequence(n);
		Print_Check();

		int prevCount = 0;
		int count = numCount();
		do {
			prevCount = count;

			iterate([&](int m, int n) {
				crossRule(m, n);
			});

			findZeroWindows();
			fillZeroWindows();

			for (auto& win : zeroWindows) {
				if (win.size == 1) {
					longCrossRule(win.m, win.n);
				}
			}

			Print_Check();
			count = numCount();
		} while (count > prevCount);
		#undef Print_Check
	}



private:
	/* Calculation Methods */
	void crossRule(int m, int n) {
		// https://files.catbox.moe/vlh72e.png
		brick _a0 = get(m-1, n);
		brick _a1 = get(m+1, n);
		brick _b0 = get(m, n-1);
		brick _b1 = get(m, n+1);
		brick _c  = get(m, n);
		#define a0 (*_a0) // these defines make it much less
		#define a1 (*_a1) // awkward when doing math with the
		#define b0 (*_b0) // optional values.
		#define b1 (*_b1) // The tradeoff is they take a lot
		#define c (*_c)   // of vertical space though

		if (!_a0 + !_a1 + !_b0 + !_b1 + !_c == 1) { // 1 unknown
			/**/ if (!_a0 && a1 != 0) { set(m-1, n, T{ (c*c - b0*b1) / a1 }); }
			else if (!_a1 && a0 != 0) { set(m+1, n, T{ (c*c - b0*b1) / a0 }); }
			else if (!_b0 && b1 != 0) { set(m, n-1, T{ (c*c - a0*a1) / b1 }); }
			else if (!_b1 && b0 != 0) { set(m, n+1, T{ (c*c - a0*a1) / b0 }); }
		}

		#undef a0
		#undef a1
		#undef b0
		#undef b1
		#undef c
	}

	void longCrossRule(int m, int n) {
		brick _a0 = get(m-1, n);	brick _a1 = get(m+2, n);
		brick _b0 = get(m+1, n);	brick _b1 = get(m-2, n);
		brick _c0 = get(m, n-1);	brick _c1 = get(m, n+2);
		brick _d0 = get(m, n+1);	brick _d1 = get(m, n-2);
		#define a0 (*_a0)
		#define a1 (*_a1)
		#define b0 (*_b0)
		#define b1 (*_b1)
		#define c0 (*_c0)
		#define c1 (*_c1)
		#define d0 (*_d0)
		#define d1 (*_d1)

		if (!_a0 + !_a1 + !_b0 + !_b1
		+   !_c0 + !_c1 + !_d0 + !_d1 == 1) {
			/**/ if (!_a1 && a0 != 0) { set(m+2, n, T{ (-b0*b0*b1 + c0*c0*c1 + d0*d0*d1) / (a0*a0) }); }
			else if (!_b1 && b0 != 0) { set(m-2, n, T{ (-a0*a0*a1 + c0*c0*c1 + d0*d0*d1) / (b0*b0) }); }
			else if (!_c1 && c0 != 0) { set(m, n+2, T{ (-d0*d0*d1 + a0*a0*a1 + b0*b0*b1) / (c0*c0) }); }
			else if (!_d1 && d0 != 0) { set(m, n-2, T{ (-c0*c0*c1 + a0*a0*a1 + b0*b0*b1) / (d0*d0) }); }
		}

		#undef a0
		#undef a1
		#undef b0
		#undef b1
		#undef c0
		#undef c1
		#undef d0
		#undef d1
	}

	void findZeroWindows() {
		zeroWindows.clear();
		iterate([&](int& m, int& n) {
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
		});
	}

	void fillZeroWindows() {
		for (auto& win : zeroWindows) {
			iterate([&](int m, int n) {
				if ((win.m <= m&&m < win.m + win.size)
				&&  (win.n <= n&&n < win.n + win.size)) {
					set(m, n, T{0});
				}
			});
		}
	}



private:
	/* Internal Functions */
	void iterate(auto&& f) {
		for (int m=y0; m < y1; m++) {
		for (int n=x0; n < x1; n++) {
			f(m,n);
		} }
	}

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

	int numCount() { // number of known bricks
		int result = 0;
		iterate([&](int m, int n) {
			if (get(m, n)) { result++; }
		});
		return result;
	}



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
	NumberWall<INT> wall {
		// [](auto n) -> INT { return n*n; } ,
		// [](auto n) -> INT { return n*n*n; } ,
		Fib ,
		// seq_ZeroWindows ,
		// [](auto n) {
		// 	const std::vector<INT> seq = {1,0,0,0,0,0,0,0,1};
		// 	return (0 <= n&&n < seq.size()) ? seq[n] : 0;
		// } ,
		-10 ,
		21, 10
	};

	wall.calculate();
	// wall.print();
	wall.printPattern([](auto n) { return true; });

	return 0;
}
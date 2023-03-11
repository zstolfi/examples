#include <iostream>
#include <vector>
#include <functional>
#include <optional>
#include <cassert>

#include <cmath>
#include <algorithm>
#include <gmpxx.h> // Use GMP ratio type for now
                   // For generic number walls, I'll
                   // need my own ratio type

template <typename T>
class NumberWall {
private:
	using brick = std::optional<T>; // haha, bricks, get it? :^)
	using RATIO = mpq_class;
	std::function<T(int)> sequence {};
	std::vector<brick> wall {};

	const unsigned w, h;	// internal with & height
	const int x0, x1;   	// x bounds
	const int y0, y1;   	// y bounds

	// https://files.catbox.moe/a9bia6.png
	struct window {
		int size, m, n;
		struct { std::optional<RATIO> a,b,c,d; } ratios;
		brick v0, v1;
	};
	std::vector<window> zeroWindows {};

public:
	// The width & height in the constructor is the space we want
	// to be filled, to get there we need to start wider, and the
	// edges will diagonally shrink, ending in the desired width.

	NumberWall(std::function<T(int)> s, int x, unsigned w_in, unsigned h_in)
	: sequence{s}, w{2*h_in + w_in}, h{h_in}
	, x0 {static_cast<int>(x-h_in)}, x1 {static_cast<int>(x+w_in+h_in)}
	, y0 {static_cast<int>(0     )}, y1 {static_cast<int>(0+h_in     )}  {
		// std::cout << "Constructing with desired width of " << w_in << "\n"
		//           << "Internal width calculated as " << w << "\n";
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
					longCrossRule(win);
				} else {
					zeroGeometricRule(win);
					brokenCrossRule(win);
				}
			}

			count = numCount();
		} while (count > prevCount);
		#undef Print_Check
	}



private:
	/* Calculation Methods */
	void crossRule(int m, int n) {
		brick _a0 = get(m-1, n);
		brick _a1 = get(m+1, n);
		brick _b0 = get(m, n-1);
		brick _b1 = get(m, n+1);
		brick _x  = get(m, n);
		#define a0 (*_a0) // these defines make it much less
		#define a1 (*_a1) // awkward when doing math with the
		#define b0 (*_b0) // optional values.
		#define b1 (*_b1) // The tradeoff is they take a lot
		#define x (*_x)   // of vertical space though

		if (!_a0 + !_a1 + !_b0 + !_b1 + !_x == 1) { // 1 unknown
			/**/ if (!_a0 && a1 != 0) { set(m-1, n, T{ (x*x - b0*b1) / a1 }); }
			else if (!_a1 && a0 != 0) { set(m+1, n, T{ (x*x - b0*b1) / a0 }); }
			else if (!_b0 && b1 != 0) { set(m, n-1, T{ (x*x - a0*a1) / b1 }); }
			else if (!_b1 && b0 != 0) { set(m, n+1, T{ (x*x - a0*a1) / b0 }); }
		}

		#undef a0
		#undef a1
		#undef b0
		#undef b1
		#undef c
	}

	// The horseshoe rule is really just the next 2 rules but combined into one.
	// In my code they're independent of one another. Hoever the long cross rule
	// seems to be a specialization of the broken cross rule, where the ratio of
	// the sides of the zero-window are not needed. Haven't confirmed it though.

	void longCrossRule(window& win) {
		if (win.size > 1) { return; }
		auto m = win.m, n = win.n;
		brick _a0 = get(m-1, n);    brick _a1 = get(m+2, n);
		brick _b0 = get(m+1, n);    brick _b1 = get(m-2, n);
		brick _c0 = get(m, n-1);    brick _c1 = get(m, n+2);
		brick _d0 = get(m, n+1);    brick _d1 = get(m, n-2);
		#define a0 (*_a0)
		#define a1 (*_a1)
		#define b0 (*_b0)
		#define b1 (*_b1)
		#define c0 (*_c0)
		#define c1 (*_c1)
		#define d0 (*_d0)
		#define d1 (*_d1)

		if (!_a0 + !_a1 + !_b0 + !_b1
		+   !_c0 + !_c1 + !_d0 + !_d1 == 1) { // 1 unknown
			/**/ if (!_a1) { set(m+2, n, T{ (-b0*b0*b1 + c0*c0*c1 + d0*d0*d1) / (a0*a0) }); }
			else if (!_b1) { set(m-2, n, T{ (-a0*a0*a1 + c0*c0*c1 + d0*d0*d1) / (b0*b0) }); }
			else if (!_c1) { set(m, n+2, T{ (-d0*d0*d1 + a0*a0*a1 + b0*b0*b1) / (c0*c0) }); }
			else if (!_d1) { set(m, n-2, T{ (-c0*c0*c1 + a0*a0*a1 + b0*b0*b1) / (d0*d0) }); }
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

	void brokenCrossRule(window& win) {
		// This rule has the biggest formula yet, I'll only solve
		// for the case when the b-row is unknown. I could extend
		// it in the future if needed. Though it'd be kinda huge.
		int m = win.m, n = win.n, size = win.size;
		auto& r = win.ratios;

		bool valid = true;
		valid &= r.a && r.b && r.c && r.d;
		for (int i=1; valid && i <= size; i++) {
			valid &= /*a*/ get(m-1 +i, n-1) && get(m-1 +i, n-2)
			      && /*c*/ get(m-1, n-1 +i) && get(m-2, n-1 +i)
			      && /*b*/ get(m+size -i, n+size) && get(m+size -i, n+size+1)
			      && /*d*/ get(m+size, n+size -i) /********* unkown *********/;
		}
		if (!valid) { return; }

		for (int i=1; i <= size; i++) {
			int sign = (i%2 == 0) ? 1 : -1;

			T a0 = *get(m-1 +i, n-1), a1 = *get(m-1 +i, n-2);
			T c0 = *get(m-1, n-1 +i), c1 = *get(m-2, n-1 +i);
			T b0 = *get(m+size -i, n+size), b1 = *get(m+size -i, n+size+1);
			T d0 = *get(m+size, n+size -i);

			set(m+size+1, n+size -i, T {
				(       *r.c * a0*b0*c1*d0
				+ sign*(*r.a * a1*b0*c0*d0
				-       *r.b * a0*b1*c0*d0)) / (*r.d * a0*b0*c0)
			});
		}
	}

	void zeroGeometricRule(window& win) {
		int m = win.m, n = win.n, size = win.size;
		auto& r = win.ratios;

		if (!r.a + !r.b + !r.c + !r.d == 1) { // 1 unknown
			int sign = (size%2 == 0) ? 1 : -1;
			/**/ if (!r.a) { r.a = sign * (*r.c) * (*r.d) / (*r.b); }
			else if (!r.b) { r.b = sign * (*r.c) * (*r.d) / (*r.a); }
			else if (!r.c) { r.c = sign * (*r.a) * (*r.b) / (*r.d); }
			else if (!r.d) { r.d = sign * (*r.a) * (*r.b) / (*r.c); }
		}

		#define Apply_Side(R, V, M, N) \
			if ((R) && (V)) { \
				T val = *(V); \
				for (int i=1; i <= size; i++) { \
					val = val * *(R); \
					/*if (auto orig = get(M,N); orig && *orig != val) {*/ \
					/*	std::cout << "ERR!\t" << M << " " << N << "\t" << *orig << " <-> " << val << "\n";*/ \
					/*}*/ \
					set(M, N, val); \
				} \
			}

		// apply the known ratios
		Apply_Side(win.ratios.a, win.v0, m-1      , n-1    +i);
		Apply_Side(win.ratios.c, win.v0, m-1    +i, n-1      );
		Apply_Side(win.ratios.b, win.v1, m+size   , n+size -i);
		Apply_Side(win.ratios.d, win.v1, m+size -i, n+size   );

		#undef Apply_Side
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

		// find the ratios for any of the 4 sides of the window
		for (auto& win : zeroWindows) {
			int m = win.m, n = win.n, size = win.size;
			if (auto corner = get(m-1, n-1)) {
				win.v0 = *corner;
				if (auto a = get(m-1, n)) { win.ratios.a = RATIO {*a,*corner}; }
				if (auto c = get(m, n-1)) { win.ratios.c = RATIO {*c,*corner}; }
			}

			if (auto corner = get(m+size, n+size)) {
				win.v1 = *corner;
				if (auto b = get(m+size, n+size-1)) { win.ratios.b = RATIO {*b,*corner}; }
				if (auto d = get(m+size-1, n+size)) { win.ratios.d = RATIO {*d,*corner}; }
			}
			if (auto& v = win.ratios.a) { v->canonicalize(); }
			if (auto& v = win.ratios.b) { v->canonicalize(); }
			if (auto& v = win.ratios.c) { v->canonicalize(); }
			if (auto& v = win.ratios.d) { v->canonicalize(); }

		}
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
			if (auto prev = wall[i*w+j]; prev && *prev != *value) {
				std::cout << "ERR!\t"
				          << m << ", " << n << "\t"
				          << *prev << " <- " << *value << "\n";
				print();
			}
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
			for (unsigned j=h; j < w-h; j++) {
				if (auto value = wall[i*w+j]) {
					std::cout << *value;
				} else {
					// std::cout << ".";
				}
				if (j+1 <= w) { std::cout << "\t"; }
			}
			std::cout << "\n";
		}
	}

	void printPattern(std::function<bool(T)> f) {
		for (unsigned i=0; i < h; i++) {
			for (unsigned j=h; j < w-h; j++) {
				if (auto value = wall[i*w+j]; value && f(*value)) {
					std::cout << "[]";
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
		// [](auto n) -> INT { return Fib(n) - 1; } ,
		// [](auto n) -> INT { return std::sin(1.0 / (n/10000000.0)) > 0.5; } , // pseudo-random

		// seq_ZeroWindows ,
		// seq_Rueppel ,
		// seq_Rook ,
		seq_Knight ,
		0 ,
		256, 128
	};

	wall.calculate();
	// wall.print();
	wall.printPattern([](auto n) { return n%2 == 0; });

	return 0;
}
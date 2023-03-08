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
	struct window { int size, m, n; struct { std::optional<RATIO> a,b,c,d; } ratios; };
	std::function<T(int)> sequence {};
	std::vector<brick> wall {};

	const unsigned w, h;	// with, height
	const int x0, x1;   	// x bounds
	const int y0, y1;   	// y bounds

	std::vector<window> zeroWindows {};
	// std::vector<window> partialZeroWindows {};

public:
	// The width & height in the constructor is the space we want
	// to be filled, to get there we need to start wider, and the
	// edges will diagonally shrink, ending in the desired width.

	NumberWall(std::function<T(int)> s, int x, unsigned _w, unsigned _h)
	: sequence{s}, w{2*_h + _w}, h{_h}
	, x0{static_cast<int>(x-_h)}, x1{static_cast<int>(x+_w+_h)}
	, y0{static_cast<int>(0   )}, y1{static_cast<int>(0+_h   )}  {
		std::cout << "Constructing with desired width of " << _w << "\n"
		          << "Internal width calculated as " << w << "\n";
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

		// setup
		for (int n=x0; n < x1; n++)
			wall[n-x0] = sequence(n);

		int prevCount = 0;
		int count = numCount();
		do {
			prevCount = count;
			Print_Check();

			iterate([&](int m, int n) {
				crossRule(m, n);
			});

			findZeroWindows();
			fillZeroWindows();

			for (auto& win : zeroWindows) {
				if (win.size == 1) {
					zeroCrossRule(win);
				}
				zeroGeometricRule(win);
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
	// In my code they're independent of one another. And the long cross rule is
	// really just a specialization of the zero cross rule. (no extra code then)

	void zeroCrossRule(window win) {
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
		+   !_c0 + !_c1 + !_d0 + !_d1 == 1) {
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

	void zeroGeometricRule(window win) {
		auto m = win.m, n = win.n;
		unsigned vecSize = win.size + 2;
		std::vector<brick> a {};    a.resize(vecSize);
		std::vector<brick> b {};    b.resize(vecSize);
		std::vector<brick> c {};    c.resize(vecSize);
		std::vector<brick> d {};    d.resize(vecSize);
		for (unsigned i=0; i < vecSize; i++) {
			a[i] = get(m-1          , n-1        +i);
			c[i] = get(m-1        +i, n-1          );
			b[i] = get(m+win.size   , n+win.size -i);
			d[i] = get(m+win.size -i, n+win.size   );
		}

		std::vector loop {make_pair(&a, &win.ratios.a),
		                  make_pair(&b, &win.ratios.b),
		                  make_pair(&c, &win.ratios.c),
		                  make_pair(&d, &win.ratios.d)};

		for (auto [vec, ratio] : loop) {
			// Iterate through odd-spaced pairs, even-spaced
			// pairs would give us 2 roots, as opposed to 1.
			bool found = false;
			unsigned pow = 1;
			RATIO r {1,1};
			if (*ratio) { found = true; r = *(*ratio); }

			for (unsigned i=0;   !found && i < vec->size()-1; i++) {
			for (unsigned j=i+1; !found && j < vec->size()  ; j+=2) {
				if ((*vec)[i] && (*vec)[j]) {
					pow = j-i;
					r = RATIO {*(*vec)[j], *(*vec)[i]}; // dereference, subscript, opt_dereference
					r.canonicalize();
					found = true;
				}
			} }
			if (!found) { break; }

			mpz_root(r.get_num_mpz_t(), r.get_num_mpz_t(), pow);
			mpz_root(r.get_den_mpz_t(), r.get_den_mpz_t(), pow);

			// Now that we have the per-step ratios
			// we can calculate rest of the values.
			T val = *(*vec)[0]; // TODO: fill vec without relying on element 0
			for (unsigned i=0; i < vec->size(); i++) {
				(*vec)[i] = val;
				val = val * r;
			}
		}

		for (unsigned i=0; i < vecSize; i++) {
			set(m-1          , n-1        +i, a[i]);
			set(m-1        +i, n-1          , c[i]);
			set(m+win.size   , n+win.size -i, b[i]);
			set(m+win.size -i, n+win.size   , d[i]);
		}
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
		// Fib ,
		seq_ZeroWindows ,
		0 ,
		14, 8
	};

	wall.calculate();
	// wall.print();
	wall.printPattern([](auto n) { return true; });

	return 0;
}
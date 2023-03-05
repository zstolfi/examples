#include <iostream>
#include <gmpxx.h>
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
	int x;        	// starting x pos
	unsigned w, h;	// with, height

public:
	NumberWall(std::function<T(int)> s, int x, unsigned w, unsigned h)
	: sequence{s}, x{x}, w{w}, h{h} {
		wall.resize(w*h, std::nullopt);
	}

	const std::vector<T>& data() { return wall; }
	const unsigned width () { return w; }
	const unsigned height() { return h; }

	void calculate() {
		auto get = [&](int m, int n) -> brick {
			const unsigned i = m, j = n+x;
			/**/ if (m <  -1) { return 0; }
			else if (m == -1) { return 1; }
			else if (0 <= j&&j < w && i < h) { return wall[i*w+j]; }
			else { return std::nullopt; }
		};

		for (unsigned j=0; j < w; j++) {
			wall[j] = sequence(j+x);
		}
	}

	void print() {
		for (unsigned i=0; i < h; i++) {
			for (unsigned j=0; j < w; j++) {
				if (auto value = wall[i*w+j]) {
					std::cout << *value;
				} else {
					std::cout << "*";
				}
				if (j+1 <= x+w) { std::cout << "\t"; }
			}
			std::cout << "\n";
		}
	}

	// void printPattern(std::function<bool(T)>) {
	// 	for ()
	// }
};



using INT = mpz_class;

int main() {
	NumberWall<INT> squares {
		[](auto n) -> INT { return n*n; } ,
		5 ,
		10, 5
	};

	squares.calculate();
	squares.print();

	return 0;
}
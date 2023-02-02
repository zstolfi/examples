#include "Polynomial.hh"
#include <iostream>

int main() {
	std::cout << (5 + 3*x*x*x*x) << "\n";
	std::cout << (1 + -2*x + 3*x*x) << "\n";

	Polynomial f = 1 + -2*x + 3*x*x;

	f *= 5 + x;

	f = 200 + f;

	f /= 2;

	std::cout << f << "\n";

	// The most famous long polynomial I can think of
	Polynomial g = { - 6, + 3, - 6, +12, - 4, + 7, - 7, + 1,   0, + 5,
	                 - 2, - 4, -12, + 2, + 7, +12, - 7, -10, - 4, + 3,
	                 + 9, - 7,   0, - 8, +14, - 3, + 9, + 2, - 3, -10,
	                 - 2, - 6, + 1, +10, - 3, + 1, + 7, - 7, + 7, -12,
	                 - 5, + 8, + 6, +10, - 8, - 8, - 7, - 3, + 9, + 1,
	                 + 6, + 6, - 2, - 3, -10, - 2, + 3, + 5, + 2, - 1,
	                 - 1, - 1, - 1, - 1, + 1, + 2, + 2, - 1, - 2, - 1,
	                   0, + 1 };

	std::cout << g(1.3035772690342964) << "\n";
}
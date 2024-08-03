#include <iostream>
#include "polynomial.hh"
#include "numbers.hh"
using UINT = unsigned long long;

int main() {
	auto triangleNumber = [](UINT i) -> UINT {
		static const Polynomial f {(double [3]) {0.0, 0.5, 0.5}};
		return f(i);
	};

	UINT i = 1;
	for (;; i++) {
		UINT n = triangleNumber(i);
		if ((Factors {n}).divisorCount() > 500) break;
	}
	// Number<UINT> i = 1;
	// while (triangleNumber(i).factors().divisorCount() <= 500) i++;
	std::cout << "Triangle[" << i << "] = " << triangleNumber(i) << "\n";
}

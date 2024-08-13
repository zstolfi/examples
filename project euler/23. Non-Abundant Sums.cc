#include <iostream>
#include "numbers.hh"
#include "tabulate.hh"
using UINT = unsigned;

UINT d(UINT n) {
	return Factors{n}.divisorSum() - n;
}

bool isAbundant(UINT n) { return d(n) > n; }

int main() {
	UINT sum = 0;
	for (UINT i=1; i<28'123; i++) {
		bool expressable = true;
		for (UINT j=1; j<=i/2; j++) {
			if (isAbundant(j) && isAbundant(i-j)) {
				expressable = false;
				break;
			}
		}
		if (expressable) sum += i;
	}
	std::cout << "Sum:\n" << sum << "\n";
}

#include <iostream>
#include "numbers.hh"
using UINT = unsigned;

UINT d(UINT n) {
	UINT sum = 0;
	Factors{n}.iterateDivisors([&](UINT d) {
		if (d != n) sum += d;
	});
	return sum;
}

int main() {
	UINT sum = 0;
	for (UINT i=1; i<10'000; i++) {
		if (i != d(i) && i == d(d(i))) {
			sum += i;
			std::cout << i << "\t->\t" << d(i) << "\n";
		}
	}
	std::cout << "sum:\n" << sum << "\n";
}

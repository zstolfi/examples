#include <iostream>
#include <map>
#include "tabulate.hh"
using UINT = unsigned long long;

Tab<UINT(UINT, UINT)> choose {[](UINT n, UINT k) -> UINT {
	if (n < 0 || k < 0 || n < k) return 0;
	if (k == 0 || n == k) return 1;
	return choose(n-1, k-1) + choose(n-1, k);
}};

int main() {
	// Equivalent problem: Find the number of
	// 40-bit integers with a pop_count = 20.
	std::cout << choose(40, 20) << "\n";
}

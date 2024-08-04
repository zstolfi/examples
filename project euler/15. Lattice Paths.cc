#include <iostream>
#include <map>
using UINT = unsigned long long;

UINT choose (UINT n, UINT k) {
	static auto tab = std::map<std::pair<UINT, UINT>, UINT> {};
	if (n < 0 || k < 0 || n < k) return 0;
	if (k == 0 || n == k) return 1;
	if (tab.contains({n,k})) return tab.at({n,k});
	return tab[{n,k}] = choose(n-1, k-1) + choose(n-1, k);
}

int main() {
	// Equivalent problem: Find the number of
	// 40-bit integers with a pop_count = 20.
	std::cout << choose(40, 20) << "\n";
}

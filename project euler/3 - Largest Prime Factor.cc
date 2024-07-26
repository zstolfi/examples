#include <iostream>
#include <utility>
#include <tuple>
using UINT = unsigned long long;

std::pair<UINT, UINT> removeSmallestFactor(UINT n) {
	for (UINT p=2; p*p < n; p++) {
		if (n%p == 0) return {p, n/p};
	}
	return {n, 1};
}

int main() {
	const UINT in = 600'851'475'143;
	UINT n = in, factor {};
	std::cout << "Factors of " << in << ":\n";
	while (n > 1) {
		std::tie(factor, n) = removeSmallestFactor(n);
		std::cout << factor << "\n";
	}
}
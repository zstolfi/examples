#include <iostream>
#include <map>
#include <limits>
#include "extrema.hh"
using UINT = unsigned long long;

UINT chainLength(UINT n) {
	static auto tab = std::map<UINT, UINT> {{1, 1}};
	return tab.contains(n)
		? tab.at(n)
		: tab[n] = 1 + chainLength(n&1 ? 3*n+1 : n/2);
}

int main() {
	// Using a std::pair doesn't let me confirm
	// the max value is unique. In the future I
	// could allow a 'less' template parameter.
	auto extrema = Extrema<std::pair<UINT, UINT>> {};
	for (UINT i=1; i<1'000'000; i++) {
		extrema.add({chainLength(i), i});
	}
	auto max = extrema.getMax();
	std::cout << "ChainLength(" << max.second << ") = " << max.first << "\n";
	std::cout << max.second << "\n";
}

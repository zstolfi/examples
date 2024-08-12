#include <iostream>
#include <map>
using UINT = unsigned long long;

UINT fib(UINT n) {
	static auto tab = std::map<UINT, UINT> {{0,1}, {1,2}};
	return tab.contains(n) ? tab.at(n) : tab[n] = fib(n-1) + fib(n-2);
}

int main() {
	UINT sum = 0;
	for (UINT i=0; fib(i) <= 4'000'000; i++) {
		if (fib(i) % 2 == 0) sum += fib(i);
	}
	std::cout << "Sum:\n" << sum << "\n";
}

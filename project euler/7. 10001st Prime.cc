#include <iostream>
#include <vector>
#include <algorithm>
namespace ranges = std::ranges;
using UINT = unsigned long long;

int main() {
	const UINT N = 10'001;
	auto primes = std::vector<UINT> {};
	primes.reserve(N);

	for (UINT n=2; ; n++) {
		auto dividesN = [n](UINT p) { return n%p == 0; };
		if (ranges::none_of(primes, dividesN)) {
			primes.push_back(n);
			if (primes.size() >= N) break;
		}
	}
	std::cout << "Prime #" << N << ":\n";
	std::cout << primes[N-1] << "\n";
}

#include <iostream>
#include "prime_sieve.hh"
using UINT = unsigned long long;

int main() {
	PrimeSieve sieve {};
	UINT sum = 0;
	for (UINT i=0; sieve.getPrime(i) < 2'000'000; i++) {
		sum += sieve.getPrime(i);
	}
	std::cout << "Sum: " << sum << "\n";
}

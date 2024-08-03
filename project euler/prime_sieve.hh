#pragma once
#include <vector>

class PrimeSieve {
	std::vector<bool> sieve {false, false};
	std::vector<unsigned> primes {2, 3};

public:
	PrimeSieve() {}

	unsigned getPrime(unsigned i) {
		while (primes.size() <= i) calculateUpTo(2*sieve.size());
		return primes[i];
	}

	bool isPrime(unsigned n) {
		if (sieve.size() <= n+1) calculateUpTo(n+1);
		return sieve[n];
	}

private:
	void calculateUpTo(unsigned size) {
		unsigned oldSize = sieve.size();
		if (oldSize >= size) return;
		sieve.resize(size, true);

		// TODO: Optimize.
		for (unsigned p=2; p<size; p++) if (sieve[p]) {
			if (p > primes.back()) primes.push_back(p);
			for (unsigned i=2; i*p < size; i++) {
				sieve[i*p] = false;
			}
		}
	}
};

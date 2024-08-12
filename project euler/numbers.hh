#pragma once
#include <map>
#include <concepts>

template <std::unsigned_integral UINT>
class Factors {
	std::map<UINT/*prime*/, UINT/*exp*/> m_factors;

public:
	Factors(UINT n) {
		while (n > 1) {
			auto [p, nextN] = splitSmallestFactor(n);
			m_factors[p]++, n = nextN;
		}
	}

	const auto& getPrimes() const { return m_factors; }

	std::size_t divisorCount() const {
		if (m_factors.empty()) return 0;
		
		UINT result = 1;
		for (auto [prime, exp] : m_factors) {
			result *= exp+1;
		}
		return result;
	}

	// Not guaranteed to be in order.
	void iterateDivisors(auto&& f) {
		for (std::size_t i=0; i<divisorCount(); i++) {
			UINT divisor = 1;
			for (std::size_t j=i; auto [prime, exp] : m_factors) {
				for (UINT k=1; k <= j % (exp+1); k++) divisor *= prime;
				j /= exp+1;
			}
			f(divisor);
		}
	}

private:
	std::pair<UINT, UINT> splitSmallestFactor(UINT n) {
		for (UINT p=2; p*p<=n; p++) if (n%p == 0) {
			return {p, n/p};
		}
		return {n, 1};
	}
};

Factors(std::unsigned_integral auto n) -> Factors<decltype(n)>;

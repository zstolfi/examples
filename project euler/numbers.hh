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

	UINT divisorCount() const {
		if (m_factors.empty()) return 0;
		
		UINT result = 1;
		for (auto [prime, exp] : m_factors) {
			result *= exp+1;
		}
		return result;
	}

	UINT divisorSum() const {
		UINT result = 1;
		for (auto [prime, exp] : m_factors) {
			UINT sum = 0;
			for (UINT q=1, i=0; i<=exp; q*=prime, i++) {
				sum += q;
			}
			result *= sum;
		}
		return result;
	}

	// Not guaranteed to be in order.
	void iterateDivisors(auto&& f) const {
		for (UINT i=0; i<divisorCount(); i++) {
			UINT divisor = 1;
			for (UINT j=i; auto [prime, exp] : m_factors) {
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

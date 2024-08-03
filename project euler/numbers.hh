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
		UINT result = 1;
		for (auto [prime, exp] : m_factors) {
			result *= exp + 1;
		}
		return result;
	}

	// void iterateDivisors(auto&& f);

private:
	std::pair<UINT, UINT> splitSmallestFactor(UINT n) {
		for (UINT p=2; p*p<=n; p++) if (n%p == 0) {
			return {p, n/p};
		}
		return {n, 1};
	}
};

Factors(std::unsigned_integral auto n) -> Factors<decltype(n)>;

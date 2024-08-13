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
//		UINT result = 0;
//		iterateDivisors([&](UINT d) { result += d; });
//		return result;
		UINT result = 1;
		for (auto [prime, exp] : m_factors) {
			result *= (pow(prime, exp+1)-1) / (prime-1);
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
	static std::pair<UINT, UINT> splitSmallestFactor(UINT n) {
		for (UINT p=2; p*p<=n; p++) if (n%p == 0) {
			return {p, n/p};
		}
		return {n, 1};
	}

	static UINT pow(UINT a, UINT b) {
		UINT result = 1;
		while (b-- > 0) result *= a;
		return result;
	}
};

Factors(std::unsigned_integral auto n) -> Factors<decltype(n)>;

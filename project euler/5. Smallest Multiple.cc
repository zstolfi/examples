#include <iostream>
#include <map>
#include <utility>
#include <numeric>
using UINT = unsigned long long;

UINT pow(UINT b, UINT n) {
	UINT result = 1;
	while (n-- > 0) result *= b;
	return result;
}

std::pair<UINT, UINT> splitSmallestFactor(UINT n) {
	for (UINT p=2; p*p <= n; p++) {
		if (n%p == 0) return {p, n/p};
	}
	return {n, 1};
}

class FactoredNumber {
	std::map<UINT, UINT> m_map;
public:
	FactoredNumber() = default;
	FactoredNumber(UINT n) {
		for (UINT factor{}; n>1; ) {
			std::tie(factor, n) = splitSmallestFactor(n);
			m_map[factor]++;
		}
	}	
	
	const auto& getFactors() const { return m_map; }
	
	operator UINT() const {
		UINT result = 1;
		for (auto [prime, exp] : m_map) result *= pow(prime, exp);
		return result;
	}

	FactoredNumber& combine(FactoredNumber other) {
		for (auto [prime, exp] : other.getFactors()) {
			UINT ourExp = m_map.contains(prime) ? m_map.at(prime) : 0;
			m_map[prime] = std::max(ourExp, exp);
		}
		return *this;
	}
};

int main() {
	FactoredNumber fac {};
	for (UINT i=1; i<=20; i++) fac.combine(FactoredNumber {i});

	std::cout << UINT {fac};
	for (int i=0; auto [prime, exp] : fac.getFactors()) {
		std::cout << (i++? " * ": " = ") << prime << "^" << exp;
	}
	std::cout << "\n";
}

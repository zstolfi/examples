#include <iostream>
using UINT = unsigned long long;
constexpr UINT Base = 10;

consteval UINT P(UINT n) { return (n == 0) ? 1 : Base * P(n-1); }
constexpr UINT pow[] = {P(0), P(1), P(2), P(3), P(4), P(5), P(6), P(7), P(8), P(9)};

UINT digitAt(UINT n, UINT i) {
	return n / pow[i] % Base;
}

UINT flip(UINT n, UINT i) {
	UINT result = 0;
	for (UINT j=0; j<=i; j++) result += pow[i-j] * digitAt(n, j);
	return result;
}

UINT makePalindrome(UINT n, UINT i) {
	return pow[i/2]*n + flip(n, (i-1)/2) % pow[i/2];
}

int main() {
	std::cout << "All palindromes of length 4:\n";
	for (UINT n=10; n<=99; n++) std::cout << "\t" << makePalindrome(n, 4);
	std::cout << "\n";
}

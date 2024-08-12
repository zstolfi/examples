#include <iostream>
using UINT = unsigned long long;
constexpr UINT Base = 10;
// https://www.desmos.com/calculator/oqukwzyuc9

consteval UINT P(UINT n) { return (n == 0)? 1: Base * P(n-1); }
constexpr UINT pow[] = {P(0), P(1), P(2), P(3), P(4), P(5), P(6), P(7), P(8)};

// Digit of n at i:
UINT digitAt(UINT n, UINT i) {
	return n / pow[i] % Base;
}

// Flip n about i:
UINT flip(UINT n, UINT i) {
	UINT result = 0;
	for (UINT j=0; j<=i; j++) result += pow[i-j] * digitAt(n, j);
	return result;
}

// Palindrome of n of size i:
UINT makePalindrome(UINT n, UINT i) {
	return pow[i/2]*n + flip(n, (i-1)/2) % pow[i/2];
}

int main() {
	for (UINT n=999; n >= 100; n--) {
		UINT pal = makePalindrome(n, 6);
		for (UINT fac=999; fac*fac > pal; fac--) {
			if (pal % fac != 0) continue;
			std::cout << fac << " * " << pal/fac << " =\n" << pal << "\n";
			return 0;
		}
	}
	std::cout << "No factors found.\n";
}

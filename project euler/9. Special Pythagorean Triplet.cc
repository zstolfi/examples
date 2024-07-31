#include <iostream>
using UINT = unsigned long long;
// https://www.desmos.com/calculator/hbbykdxrem

int main() {
	constexpr UINT N = 1000;
	UINT a {}, b {}, c {};
	for (a=1; a<N/2; a++) {
		UINT num = N*N - 2*N*a;
		UINT den = 2*N - 2*a;
		if (num % den == 0) {
			b = num / den;
			c = N - a - b;
			break;
		}
	}
	std::cout << a << "^2 + " << b << "^2 = " << c << "^2\n";
	std::cout << "Product: " << a*b*c << "\n";
}

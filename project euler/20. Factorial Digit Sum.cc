#include <iostream>
#include "dense_int.hh"
using UINT = unsigned;

int main() {
	Dense product {1};
	for (UINT i=1; i<=100; i++) product *= i;
	std::cout << "100! = " << product << "\n";

	UINT sum = 0;
	for (;product; product/=10) sum += UINT {product%10};
	std::cout << "Sum:\n" << sum << "\n";
}

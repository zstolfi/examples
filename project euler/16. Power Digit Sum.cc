#include <iostream>
#include "dense_int.hh"
using UINT = unsigned;

int main() {
	auto n = Dense{1} << 1000;
	std::cout << "2^1000 = " << n << "\n";

	UINT sum = 0;
	for (;n; n/=10) sum += UINT {n%10};
	std::cout << "Sum:\n" << sum << "\n";
}

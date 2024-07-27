#include <iostream>
using UINT = unsigned long long;

int main() {
	UINT sum = 0;
	for (std::size_t i=0; i<1000; i++) {
		if (i%3 == 0 or i%5 == 0) sum += i;
	}
	std::cout << "Sum:\t" << sum << "\n";
}

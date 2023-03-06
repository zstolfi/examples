#include <gmpxx.h>

mpz_class Fib(int n) { // valid for n = [-100 ... 100]
	const mpf_class phi      {"1.61803398874989484820458", 80};
	const mpf_class invsqrt5 {"0.44721359549995793928183", 80};

	mpf_class result {invsqrt5};
	for (int i=0; i < std::abs(n); i++)
		result *= phi;
	result *= (n >= 0 || n%2 == 0) ? 1 : -1;
	result = floor(0.5 + result);
	return mpz_class{result};
}

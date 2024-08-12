#include <iostream>
#include <string>
using UINT = unsigned;

std::string inWords_impl(const UINT n, std::string join="") {
	if (n == 0) return "";
	std::string result = "";
	if (1 <= n&&n <= 19) switch (n) {
		break; case  1: result = "one";
		break; case  2: result = "two";
		break; case  3: result = "three";
		break; case  4: result = "four";
		break; case  5: result = "five";
		break; case  6: result = "six";
		break; case  7: result = "seven";
		break; case  8: result = "eight";
		break; case  9: result = "nine";
		break; case 10: result = "ten";
		break; case 11: result = "eleven";
		break; case 12: result = "twelve";
		break; case 13: result = "thirteen";
		break; case 14: result = "fourteen";
		break; case 15: result = "fifteen";
		break; case 16: result = "sixteen";
		break; case 17: result = "seventeen";
		break; case 18: result = "eighteen";
		break; case 19: result = "nineteen";
	}
	if (20 <= n&&n <= 99) switch (n/10) {
		break; case 2: result = "twenty"  + inWords_impl(n%10, "-");
		break; case 3: result = "thirty"  + inWords_impl(n%10, "-");
		break; case 4: result = "forty"   + inWords_impl(n%10, "-");
		break; case 5: result = "fifty"   + inWords_impl(n%10, "-");
		break; case 6: result = "sixty"   + inWords_impl(n%10, "-");
		break; case 7: result = "seventy" + inWords_impl(n%10, "-");
		break; case 8: result = "eighty"  + inWords_impl(n%10, "-");
		break; case 9: result = "ninety"  + inWords_impl(n%10, "-");
	}
	if (100 <= n&&n <= 999) {
		result = inWords_impl(n/100) + " hundred" + inWords_impl(n%100, " and ");
	}
	if (1'000 <= n&&n <= 999'999) {
		result = inWords_impl(n/1000) + " thousand" + inWords_impl(n%1000, " ");
	}
	return join + result;
}

std::string inWords(UINT n) {
	if (n == 0) return "zero";
	return inWords_impl(n);
}

int main() {
	UINT sum = 0;
	for (UINT i=1; i<=1000; i++) {
		for (char c : inWords(i)) {
			if (c!=' ' && c!='-') sum++;
		}
	}
	std::cout << "Sum:\n" << sum << "\n";
}

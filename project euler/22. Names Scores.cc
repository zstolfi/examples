#include <iostream>
#include <vector>
#include <string_view>
#include <algorithm>
using UINT = unsigned long long;

int main() {
	std::vector<std::string_view> names {
		#include "22. names.txt"
	};

	UINT sum = 0;
	std::sort(names.begin(), names.end());
	for (UINT i=0; i<names.size(); i++) {
		UINT score = 0;
		for (char c : names[i]) score += c-'A' + 1;
		score *= i+1;
		sum += score;
	}
	std::cout << "Sum of scores:\n" << sum << "\n";
}

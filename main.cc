#include "parse val.hh"
#include <string>
#include <charconv>
#include <optional>
#include <iostream>

struct parseArguments {
	std::string input = "6/2*(1+2)";

	parseArguments(int argc, char* argv[]) {
		if (argc > 1) {
			input = argv[1];
		}
	}
};

int main(int argc, char* argv[]) {
	auto settings = parseArguments {argc, argv};

	MathParser<double> parser {
		/* parse double */
		[&](std::string s) -> std::optional<double> {
			double result;
			const char* first = s.data();
			const char* last = first + s.size();
			const auto [parseEnd, error] = std::from_chars(first, last, result);
			if (parseEnd != last || error != std::errc()) { return std::nullopt; }
			return result;
		}
	};

	std::optional<double> result = parser.eval(settings.input);

	if (result) {
		std::cout << settings.input << " = " << *result << "\n";
	} else {
		std::cout << "Parse error!\n"
		          << settings.input << " could not be evaluated.\n";
	}

	return 0;
}
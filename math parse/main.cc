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
			std::size_t n = 0;
			std::optional<double> result {};
			try {
				result = std::stod(s, &n);
				if (n < s.size()) { result = std::nullopt; }
			} catch (const std::invalid_argument&) {
				result = std::nullopt;
			}
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
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
namespace fls = std::filesystem;
using namespace std::literals;

void printStatus (const char*);
void printWarning(const char*);
void printError  (const char*);

struct Arguments {
	fls::path sourcePath;
	fls::path outputPath;
	std::ifstream sourceStream;
	std::ofstream outputStream;

	Arguments(int argc, char* argv[]) {
		for (int i=1; i<argc; i++) {
			if (argv[i] == "-i"s) { i++; sourcePath = argv[i]; continue; }
			if (argv[i] == "-o"s) { i++; outputPath = argv[i]; continue; }
		}
		if (!sourcePath.empty()) { sourceStream = std::ifstream(sourcePath); }
		if (!outputPath.empty()) { outputStream = std::ofstream(outputPath); }
	}

	std::istream& getSource() { return sourcePath.empty() ? std::cin  : sourceStream; }
	std::ostream& getOutput() { return outputPath.empty() ? std::cout : outputStream; }
};

int main(int argc, char* argv[]) {
	auto args = Arguments(argc, argv);
	auto& source = args.getSource();
	auto& output = args.getOutput();

	for (std::string line; std::getline(source, line); )
		output << line << "\n";
}
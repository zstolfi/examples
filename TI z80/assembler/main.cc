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
			if (argv[i][0] != '-') { printError("unexpected cmd arg"); }
			char flag = argv[i++][1];
			if (i >= argc || argv[i][0] == '-') { printError("missing cmd arg"); }

			/**/ if (flag == 'i') { sourcePath = argv[i]; }
			else if (flag == 'o') { outputPath = argv[i]; }
			else    { printError("unknown cmd flag"); }
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



void printStatus (const char* msg) { std::cout                 << msg; }
void printWarning(const char* msg) { std::cout << "Warning:\t" << msg; }
void printError  (const char* msg) { std::cout << "Error:\t"   << msg; exit(1); }
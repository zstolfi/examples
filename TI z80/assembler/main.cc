#include "common.hh"
#include "bytes.hh"
#include "checksum.hh"
#include "preprocesser.hh"
#include "reducer.hh"
#include "assembler.hh"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
namespace fls = std::filesystem;


struct Arguments {
	fls::path sourcePath;
	fls::path outputPath;
	std::ifstream sourceStream;
	std::ofstream outputStream;
	char programName[8] = "A"; // doensn't need to be null-terminated
	bool hasSource=0, hasOutput=0, hasPrgmName=0;

	Arguments(int argc, char* argv[]) {
		for (int i=1; i<argc; i++) {
			if (argv[i][0] != '-') { PrintError("unexpected cmd arg\n"); }
			char flag = argv[i++][1];
			if (i >= argc || argv[i][0] == '-') { PrintError("missing cmd arg\n"); }

			/**/ if (flag == 'i') { sourcePath = argv[i]   , hasSource   = true; }
			else if (flag == 'o') { outputPath = argv[i]   , hasOutput   = true; }
			else if (flag == 'n') { setProgramName(argv[i]), hasPrgmName = true; }
			else    { PrintError("unknown cmd flag\n"); }
		}
		if (hasSource) { sourceStream = std::ifstream(sourcePath  /*            */); }
		if (hasOutput) { outputStream = std::ofstream(outputPath, std::ios::binary); }
		// TODO:
		// if (!hasPrgmName && hasOutput) { setProgramName(outputPath.stem().c_str()) }
	}

	std::istream& getSource() { return hasSource ? sourceStream : std::cin ; }
	std::ostream& getOutput() { return hasOutput ? outputStream : std::cout; }

private: // TODO: test this function
	void setProgramName(const char* str) {
		bool validChars = true;
		for (int i=0; i<8 && *str!='\0'; i++) {
			if (char c = *str++; 'A' <= c&&c <= 'Z'+1) { programName[i] = c; }
			else if (c == '0') { programName[i] = 'Z'+1; } // 0 for theta
			else { validChars = false; i--; }
		}
		if (*str) { PrintWarning("prgm name was truncated\n"); }
		if (!validChars) { PrintWarning("prgm name ignores invalid chars\n"); }
	}
};


int main(int argc, char* argv[]) {
	auto args = Arguments(argc, argv);
	auto& source = args.getSource();
	auto& output = args.getOutput();
	// warn on Windows: "0x0a may output incorrectly"

	/* preprocess */
	std::vector<Line> lines = preprocess(source); // remove comments, and parse strings
	
	/* reduce */
	// as I test, I'm assuming EVERY line is a integer expression
	PrintStatus("~~~ START ~~~\n");
	std::vector<TokenList> asmLines = reduce(lines); // lex, parse expressions, and substitute addresses
	for (TokenList l : asmLines) {
		for (Token t : l) { output << (int)t.type << ":" << t.value << " "; }
		output << "\n";
	}
	PrintStatus("~~~ FINISH ~~~\n");
	return 0;
	/* assemble */
	std::vector<std::byte> byteCode = assemble(asmLines); // look up op-codes

	// Output .8xp file    (http://merthsoft.com/linkguide/ti83+/fformat.html)
	output << Bytes<11>("**TI83F*\x1A\x0A");
	output << Bytes<42>("Assembled by Zander Stolfi");
	output << Bytes<Word::LSB>(byteCode.size() + 19);
	// begin checksum
	Checksum cs;
	output << cs << Bytes<Word::LSB>(13);
	output << cs << Bytes<Word::LSB>(byteCode.size() + 2 );
	output << cs << Bytes(6);
	output << cs << Bytes<8>(args.programName);
	output << cs << Bytes<Word::LSB>(0);
	output << cs << Bytes<Word::LSB>(byteCode.size() + 2 );
	output << cs << Bytes<Word::LSB>(byteCode.size()     );
	output << cs << byteCode;
	// end checksum
	output << Bytes<Word::LSB>(cs.value);
	return 0;
}
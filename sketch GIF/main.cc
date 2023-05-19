#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

std::vector<std::byte> output;

/* print character */
void printchar(uint8_t n) {
	output.push_back(std::byte{n});
}

/* print 16-bit number */
void printnumber(uint16_t n) {
	output.insert(output.end(), {std::byte{n&0xFF}, std::byte{n>>8}});
}

/* print bytes as sub-blocks */
void printdata(std::vector<uint8_t> data) {
	std::size_t i = 0;
	while (data.size() - i > 255) {
		printchar(255);
		for (std::size_t j=0; j<255; j++) {
			printchar(data[i++]);
		}
	}
	if (data.size() - i > 0) {
		printchar(data.size() - i);
		while (i < data.size()) {
			printchar(data[i++]);
		}
	}
	printchar(0);
}

/* print string as sub-blocks */
void printdata(const char* str) {
	std::vector<uint8_t> v;
	for (std::size_t i=0; str[i] != '\0'; i++) {
		v.push_back(str[i]);
	}
	printdata(v);
}

/* print string as-is */
void printstring(const char* str) {
	for (std::size_t i=0; str[i] != '\0'; i++) {
		printchar(str[i]);
	}
}



struct Code {
	uint16_t val; /* at least 13 bits */
	uint8_t bitLen;
};

std::vector<uint8_t> packcodes(std::vector<Code> codes) {
	std::vector<uint8_t> result;
	int pos = 0;
	for (Code c : codes) {
		for (int i=0; i<c.bitLen; i++) {
			if (pos == 0) { result.push_back(0x00); }
			result.back() &= ~(1 << pos);
			result.back() |= (c.val >> i & 1) << pos;
			if (++pos == 8) { pos = 0; }
		}
	}
	return result;
}



int main(int argc, char* argv[]) {
	/* header */
	printstring("GIF89a");
	printnumber(800); /* width */
	printnumber(600); /* height */
	printchar(0b1'111'0'111); /* color table flags */
	printchar(255); /* background color */
	printchar(0); /* pixel aspect ratio (ignore) */
	for (std::size_t i=0; i<=255; i++) {
		printchar(i); printchar(i); printchar(i); /* global color table (grayscale) */
	}

	/* loop extension */
	printchar(0x21); printchar(0xFF);
	printchar(11); /* size */
	printstring("NETSCAPE2.0");
	printchar(3); /* size */
	printchar(1); /* idk?? */
	const bool loop = false;
	printnumber(!loop); /* 0 = loop forever, 1 = no loop */
	printchar(0); /* block end */

	/* comment */
	printchar(0x21); printchar(0xFE);
	printdata("Zander was here!");

	/* graphic control extension */
	printchar(0x21); printchar(0xF9);
	printchar(4); /* size */
	printchar(0b000'001'0'0); /* flags */
	printnumber(4); /* 25 fps : 0.04 sec/frame */
	printchar(0); /* alpha color index (ignore) */
	printchar(0); /* block end */

	/* image descriptor */
	printchar(0x2C);
	printnumber(0); /* x0 */
	printnumber(0); /* y0 */
	printnumber(800); /* width */
	printnumber(600); /* height */
	printchar(0b0'0'0'00'000); /* flags */

	/* image data (LZW) */
	printchar(8); /* starting bitLen-1 */
	std::vector<Code> bkgData {};
	bkgData.push_back({0x100, 9}); /* clear code */
	bkgData.push_back({0x0FF, 9}); /* color index */
	for (uint16_t i=0x102; i<=0x1FF; i++) { bkgData.push_back({i, 9}); }
	for (uint16_t i=0x200; i<=0x3FF; i++) { bkgData.push_back({i,10}); }
	for (uint16_t i=0x400; i<=0x4D3; i++) { bkgData.push_back({i,11}); }
	bkgData.push_back({0x222, 11});
	bkgData.push_back({0x101, 11}); /* end code */
	printdata(packcodes(bkgData));
	
	printchar(0x3B); /* GIF end */

	/* I would directly output the file through cout   
	   but on my machine that will always print /r/n   
	   instead of just /n. Probably known behaviour. */
	if (argc > 1) {
		std::ofstream file {argv[1], std::ios::binary};
		for (std::byte b : output) {
			file.put(static_cast<char>(b));
		}
	}
}
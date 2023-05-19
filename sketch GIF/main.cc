#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <map>

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

std::vector<Code> LZWencode(std::vector<uint8_t> data) {
	if (data.empty()) { return {{0x101, 9}}; }
	std::vector<Code> result {};
	const uint16_t clearCode = 0x100;
	const uint16_t endCode = 0x101;
	uint16_t val = 0;
	uint8_t bitLen = 9;

	std::map<
		std::vector<uint8_t>,
		uint16_t
	> dict;

	auto init = [&]() {
		dict.clear();
		std::vector <uint8_t> i = {0};
		for (val=0; val<=255; val++) {
			i[0] = val;
			dict[i] = val;
		}
		val += 2;
		bitLen = 9;
	};

	auto insert = [&](uint16_t code) {
		result.push_back({code, bitLen});
		if (val-1 == 1 << bitLen) { bitLen++; }
		if (bitLen > 12) {
			result.push_back({clearCode, 12});
			init();
		}
	};

    auto subvector = [](const std::vector<uint8_t>& v) {
        return std::vector<uint8_t> (v.begin(), v.end()-1);
    };

	insert(clearCode);
	init();

	auto it = data.begin();
	std::vector<uint8_t> indexBuffer = {*it++};

	for (; it != data.end(); it++) {
		indexBuffer.push_back(*it);
		if (!dict.contains(indexBuffer)) {
			dict[indexBuffer] = val++;
			insert(dict.at(subvector(indexBuffer)));
			indexBuffer = {indexBuffer.back()};
		}
        if (it+1 == data.end()) {
			insert(dict.at(indexBuffer));
        }
	}

	result.push_back({endCode, bitLen});
	return result;
}



struct Bounds {
	unsigned x0, y0, x1, y1;
};

void GIFheader(uint16_t W, uint16_t H) {
	/* Header */
	printstring("GIF89a");
	printnumber(W); /* width */
	printnumber(H); /* height */
	printchar(0b1'111'0'111); /* color table flags */
	printchar(255); /* background color */
	printchar(0); /* pixel aspect ratio (ignore) */
	for (std::size_t i=0; i<=255; i++) {
		printchar(i); printchar(i); printchar(i); /* global color table (grayscale) */
	}
}

void GIFtrailer() {
	/* Trailer */
	printchar(0x3B);
}

void GIFloopheader() {
	/* Application Extension */
	printchar(0x21); printchar(0xFF);
	printchar(11); /* size */
	printstring("NETSCAPE2.0");
	printchar(3); /* size */
	printchar(1); /* idk?? */
	printnumber(0); /* 0 = loop forever */
	printchar(0); /* block end */
}

void GIFcomment(const char* str) {
	/* Comment Extension */
	printchar(0x21); printchar(0xFE);
	printdata(str);
}

void GIFimageduration(uint16_t duration) {
	/* Graphic Control Extension */
	printchar(0x21); printchar(0xF9);
	printchar(4); /* size */
	printchar(0b000'001'0'0); /* flags */
	printnumber(duration);
	printchar(0); /* alpha color index (ignore) */
	printchar(0); /* block end */
}

void GIFimage(Bounds b, const std::vector<uint8_t>& data) {
	/* Image Descriptor */
	printchar(0x2C);
	printnumber(b.x0); /* left pos. */
	printnumber(b.y0); /* top pos. */
	printnumber(b.x1 - b.x0); /* width */
	printnumber(b.y1 - b.y0); /* height */
	printchar(0b0'0'0'00'000); /* flags */

	/* Table Based Image Data */
	auto dataOut = LZWencode(data);
	printchar(dataOut[0].bitLen-1); /* starting size */
	printdata(packcodes(dataOut));
}



int main(int argc, char* argv[]) {
	GIFheader(800, 600);

	const bool loop = true;
	if (loop) { GIFloopheader(); }

	GIFcomment("Zander was here!");

	GIFimageduration(100/25); /* 25 fps */

	std::vector<uint8_t> bkgData {};
	bkgData.resize(800*600, 0xFF);

	GIFimage({0, 0, 800, 600}, bkgData);

	GIFtrailer();

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
#include <cstdio>
#include <cstdint>
#include <span>

using std::putchar;
using std::printf;

void putnumber(uint16_t n) {
	printf("%c%c", n&0xFF, n>>8);
}

void printdata_iter(std::span<char> data) {
	if (data.size() == 0) { return; }
	if (data.size() < 256) {
		putchar(data.size());
		for (char c : data) {
			putchar(c);
		}
	} else {
		printdata_iter(data.subspan(0, 255));
		printdata_iter(data.subspan(256));
	}
	return;
}

void printdata(std::span<char> data) {
	printdata_iter(data);
	putchar(0);
}

int main() {
	/* header */
	printf("GIF89a");
	putnumber(800); /* width */
	putnumber(600); /* height */
	putchar(0b1'111'0'111); /* color table flags */
	putchar(200); /* background color */
	putchar(0); /* pixel aspect ratio info (ignore) */
	for (int i=0; i<=255; i++) {
		printf("%c%c%c", i,i,i); /* global color table (grayscale) */
	}

	/* comment */
	printf("%c%c", 0x21, 0xFE);
	char str[] = "Zander was here!";
	printdata(str);

	/* timing info */
	printf("%c%c", 0x21, 0xF9);
	putchar(4);
	putchar(0b000'001'0'0);
	putnumber(4); /* 25 fps */
	putchar(0);
	putchar(0);
	/* image meta info */
	putchar(0x2C);
	putnumber(10); /* x0 */
	putnumber(10); /* y0 */
	putnumber(5); /* width */
	putnumber(3); /* height */
	putchar(0b0'0'0'00'000);
	/* image data */
	// putchar(8);
	// char d[] = {
	// 	(char)0b00000000,
	// 	(char)0b00000001,
	// 	(char)0b11111100,
	// 	(char)0b00000111,
	// 	(char)0b11110000,
	// 	(char)0b00011111,
	// 	(char)0b11000000,
	// 	(char)0b01111111,
	// 	(char)0b00000000,
	// 	(char)0b11111111,
	// 	(char)0b00000001,
	// 	(char)0b11111100,
	// 	(char)0b00000111,
	// 	(char)0b11110000,
	// 	(char)0b00011111,
	// 	(char)0b11000000,
	// 	(char)0b01111111,
	// 	(char)0b00000000,
	// 	(char)0b00000001,
	// 	(char)0b00000001,
	// };
	// printdata(d);

	std::putchar(0x3B); /* trailer */
}
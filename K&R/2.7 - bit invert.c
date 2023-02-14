/* Exercise 2-7.
	Write a function invert(x,p,n) that returns x with the n
	bits that begin at position p inverted (i.e., 1 changed
	into 0 and vice versa), leaving the others unchanged.
*/

#include <stdio.h>
#include <inttypes.h>

#define BIT_LEN 32
typedef uint32_t number;
typedef unsigned bitPosition;
typedef unsigned bitCount;

number invert(number x, bitPosition p, bitCount n) {
	number bitMask = ~(~0u << n) << p ;
	return (~bitMask & x) | (bitMask & (~x));
}

void printBinary(number x) {
	for (int i=BIT_LEN-1; i >= 0; i--) {
		putchar((x >> i & 1) + '0');
		if (i > 0 && i%4 == 0) { putchar('\''); }
	}
}



void printTest(number x, bitPosition p, bitCount n) {
	printBinary(x);
	printf("\n");
	printBinary(invert(x,p,n));
	printf("\n\n");
}

int main() {
	printTest(0x00000000, 8, 16);
	printTest(0xAAAAAAAA, 0, 16);
	printTest(0x12345678, 24, 8);
	printTest(0xD15EA5ED, 0, 32); // bitshift 32 is undefined, sadly :(

	return 0;
}
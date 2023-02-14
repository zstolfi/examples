/* Exercise 2-6.
	Write a function setbits(x,p,n,y) that returns x with the n
	bits that begin at position p set to the rightmost n bits
	of y, leaving the other bits unchanged.
*/

#include <stdio.h>
#include <inttypes.h>

#define BIT_LEN 32
typedef uint32_t number;
typedef unsigned bitPosition;
typedef unsigned bitCount;

number setbits(number x, bitPosition p, bitCount n, number y) {
	number bitMask = ~(~0u << n) << p ;
	return (~bitMask & x) | (bitMask & (y << n));
}

void printBinary(number x) {
	for (int i=BIT_LEN-1; i >= 0; i--) {
		putchar((x >> i & 1) + '0');
		if (i > 0 && i%4 == 0) { putchar('\''); }
	}
}

int main() {
	printBinary( setbits(0x12345678, 8, 4, 0xFFFFFFFF) );

	return 0;
}
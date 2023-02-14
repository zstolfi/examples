/* Exercise 2-8.
	Write a function rightrot(x,n) that returns the value of
	the integer x rotated to the right by n bit positions.
*/

#include <stdio.h>
#include <inttypes.h>

#define BIT_LEN 32
typedef uint32_t number;
typedef unsigned bitCount;

number rightrot(number x, bitCount n) {
	n %= BIT_LEN;
	number wrap = (n == 0) ? 0 : (x << (BIT_LEN-n));
	return (x >> n) | (wrap);
}

void printBinary(number x) {
	for (int i=BIT_LEN-1; i >= 0; i--) {
		putchar((x >> i & 1) + '0');
		if (i > 0 && i%4 == 0) { putchar('\''); }
	}
}



void printTest(number x, bitCount n) {
	printBinary(x);
	printf("\n");
	printBinary(rightrot(x,n));
	printf("\n\n");
}

int main() {
	printTest(0x12345678, 0);
	printTest(0x12345678, 4);
	printTest(0x12345678, 31);
	printTest(0x12345678, 32);

	return 0;
}
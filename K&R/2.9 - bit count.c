/* Exercise 2-9.
	In a two's complement number system, x &= (x-1) deleted the
	rightmost 1-bit in x. Explain why. Use this observation to
	write a faster version of bitcount.
*/

/*  For any non-zero unsigned int, there is a right-most 1 bit.
 *  Subtracting by 1 will turn the right-most 1 into a 0, and
 *  turn the string of 0's that follow, into 1's.
 *  Ex:        10010000 -> 10001111
 *  Because the digits that changed will always be differnet,
 *  &'ing them together results in all 0's after the last 1.
 */

#include <stdio.h>
#include <inttypes.h>

#define BIT_LEN 32

int bitcount(uint32_t x) {
	int result = 0;
	for (; x; x &= (x-1)) { result++; }
	return result;
}



void printTest(uint32_t x);

int main() {
	printTest(0x00000000);
	printTest(0x00211200);
	printTest(0xFFFFFFFF);
	printTest(0xBEEEEEEF);
	printTest(0xAA55AA55);

	return 0;
}



void printTest(uint32_t x) {
	for (int i=BIT_LEN-1; i >= 0; i--) {
		putchar((x >> i & 1) ? '1' : '0');
	}
	printf(" - %d\n", bitcount(x));	
}
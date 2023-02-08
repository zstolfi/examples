/* Exercise 1-14.
	Write  a program to print a histogram of the frequencies of
	different characters in its input.
*/

#include <stdio.h>
#include <math.h>

#define MAX_VALUE 20
/* allow for different plotting functions */
#define plot(x) (x)
//#define plot(x) (4.4*log(2*x)-2)

void printHistogram(int h[128]);

/* display a histogram of the 95 printable ascii chars */
main() {
	// count all ascii values, even if we don't print them all
	int histogram[128] = {0};

	for (int c; (c = getchar()) != EOF; )
		histogram[c]++;
		
	printHistogram(histogram);
}



void printHistogram(int h[128]) {
	// vertical histogram
	for (int y=MAX_VALUE; y >= 1; y--) {
		for (int x=32; x <= 126; x++) {
			putchar((y <= plot(h[x])) ? '#' : ' ');
		}
		putchar('\n');
	}
	// bottom text
	for (int i=32; i <= 126; i++) {
		putchar(i);
	}
}
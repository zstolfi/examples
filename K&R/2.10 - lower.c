/* Exercise 2-10.
	Rewrite the function lower, which converts upper case
	letters to lower case, with a conditional expression
	instead of if-else.
*/

#include <stdio.h>

int lower(char c) {
	return ('A' <= c&&c <= 'Z') ? (c - 'A' + 'a') : c;
}

int main() {
	/* print stuff but lowercase */
	for (int c; (c = getchar()) != EOF; ) {
		putchar(lower(c));
	}

	return 0;
}
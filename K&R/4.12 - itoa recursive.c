/* Exercise 4-12.
	Adapt the ideas of printd to write a recursive version of
	itoa; that is, convert an integer into a string by calling
	a recursive routine.
*/

#include <string.h>
#include <math.h>    /* for abs() */

void reverse(char s[]) {
	int slen = strlen(s);
	for (int i=0; i < slen/2; i++) {
		int j = slen-1 - i;
		char temp = s[i];
		s[i] = s[j];
		s[j] = temp;
	}
}

void itoa_iter(int n, char s[], int* len) {
	if ( 0 <= n&&n <=  9) { s[(*len)++] = '0' + n;                    return; }
	if (-9 <= n&&n <= -1) { s[(*len)++] = '0' - n; s[(*len)++] = '-'; return; }
	s[(*len)++] = '0' + abs(n%10);
	itoa_iter(n/10, s, len);
}

void itoa(int n, char s[]) {
	int len = 0;
	itoa_iter(n, s, &len);
	s[len] = '\0';
	reverse(s);
}



#include <stdio.h>
#include <limits.h>

void printTest(int n) {
	static char str[64];
	itoa(n, str);
	printf("%s\n", str);
}

int main(void) {
	printTest(0);
	printTest(-1);
	printTest(1234);
	printTest(-1234);
	printTest(INT_MAX);
	printTest(INT_MIN);

	return 0;
}
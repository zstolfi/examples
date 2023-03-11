/* Exercise 3-4.
	In a two's complement number representation, our version of
	itoa does not handle the largest negative number, that is,
	the value of n equal to -(2^wordsize-1). Explain why not.
	Modify it to print that value correctly, regardless of the
	machine on which it runs.
*/

void reverse(char s[], int len) {
	for (int i=0; i < len/2; i++) {
		int j = len-i-1;
		/* swap */
		char temp = s[j];
		s[j] = s[i];
		s[i] = temp;
	}
}

// The itoa function fails for INT_MIN because
// the number has no positive counterpart. The
// negation operatoration is likely undefined.

void itoa_old(int n, char s[]) {
	int i, sign;

	if ((sign = n) < 0)
		n = -n;
	i = 0;
	do {
		s[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s, i);
}

void itoa_new(int n, char s[]) {

}



#include <stdio.h>
#include <limits.h>
#define MAX_CHAR 100

int main() {
	char str[MAX_CHAR];
	int a;

	a = 0;
	itoa_old(a, str);
	printf("%s\n", str);
	a = 32;
	itoa_old(a, str);
	printf("%s\n", str);
	a = -2000;
	itoa_old(a, str);
	printf("%s\n", str);
	a = 123456789;
	itoa_old(a, str);
	printf("%s\n", str);
	/* largest neg. */
	a = INT_MIN;
	itoa_old(a, str);
	printf("%s\n", str);
	itoa_new(a, str);
	printf("%s\n", str);

	return 0;
}
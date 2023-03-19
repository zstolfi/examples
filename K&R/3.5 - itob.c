/* Exercise 3-5.
	Write the function itob(n,s,b) that converts the integer n
	into a base b character representation in the string s. In
	particular, itob(n,s,16) formats n as a hexadecimal integer
	in s.
*/

#include <stdbool.h>
#include <stdlib.h>

void reverse(char s[], int len) {
	for (int i=0; i < len/2; i++) {
		int j = len-i-1;
		/* swap */
		char temp = s[j];
		s[j] = s[i];
		s[i] = temp;
	}
}

char digitToChar(int n) {
	if (n < 10) { return '0' + n; }
	if (10 <= n&&n < 36) { return 'A' + (n-10); }
	if (36 <= n&&n < 62) { return 'a' + (n-36); }
	return '!' + (n-62); // after that you're kinda on your own
}

void itob(int n, char s[], int b) {
	int i = 0;
	bool sign = (n < 0);
	do {
		int digit = n - b*(n/b);
		if (digit < 0) { digit = -digit; }
		s[i++] = digitToChar(digit);
	} while ((n /= b) != 0);

	if (sign) { s[i++] = '-'; }
	s[i] = '\0';
	reverse(s, i);
}




#include <stdio.h>
#define MAX_LINE 1000
bool eofFlag = true;
bool getline(char str[], int lim, int* len);


int main(void) {
	int len;
	char line[MAX_LINE];
	char numOut[MAX_LINE];

	int base;
	printf("Give me a base: ");
	getline(line, MAX_LINE, &len);
	base = atoi(line);

	if (base < 2) {
		printf("\nHey that's no good!\n"); return 0;
	} else {
		printf("\nNow enter some numbers.\n");
	}

	while (getline(line, MAX_LINE, &len)) {
		if (len == 0) { continue; }
		int num = atoi(line);
		itob(num, numOut, base);
		printf("%s\n", numOut);
	}

	return 0;
}



bool getline(char str[], int lim, int* length) {
	if (!eofFlag) { return false; }
	int c, i = 0;
	while ((c = getchar()) != EOF && c != '\n') {
		if (i+1 < lim) { str[i] = c; }
		i++;
	}
	eofFlag = (c != EOF);
	str[i] = '\0';
	*length = i;
	return true;
}
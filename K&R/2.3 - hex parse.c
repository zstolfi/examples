/* Exercise 2-3.
	Write the function htoi(s), which converts a string of
	hexadecimal digits (including an optional 0x or 0X) into
	its equivalent integer value. The allowable digits are 0
	through 9, a through f, and A through F.
*/

#include <stdio.h>
#include <stdbool.h>

bool getline(char str[], int lim, int* length);

unsigned htoi(char s[]) {
	unsigned result = 0;
	#define Parse_Fail() return result;
	#define is_Hex_Digit(c) (('0' <= c&&c <= '9') || ('a' <= c&&c <= 'f') || ('A' <= c&&c <= 'F'))


	enum { TAG, DIGITS } state = TAG;
	for (int i=0; s[i] != '\0'; ) {
		if (state == TAG) {
			if (s[i] == '0' && (s[i+1] == 'x' || s[i+1] == 'X')) {
				i += 2;
			}
			state = DIGITS;
		} else if (state == DIGITS) {
			if (!is_Hex_Digit(s[i])) { Parse_Fail(); }
			char offset;
			if ('0' <= s[i]&&s[i] <= '9') { offset = '0'; }
			if ('a' <= s[i]&&s[i] <= 'f') { offset = 'a'-10; }
			if ('A' <= s[i]&&s[i] <= 'F') { offset = 'A'-10; }
			result = 16 * result + (s[i] - offset);
			i++;
		}
	}

	return result;
	#undef Parse_Fail
	#undef is_Hex_Digit
}



#define MAX_LINE 1000

int main() {
	int len;
	char line[MAX_LINE];
	for (bool done; !done; ) {
		done = getline(line, MAX_LINE, &len);

		if (len > 0) { printf("%d\n", htoi(line)); }
	}

	return 0;
}



bool getline(char str[], int lim, int* length) {
	int c, i = 0;
	while ((c = getchar()) != EOF && c != '\n') {
		if (i+1 < lim) { str[i] = c; }
		i++;
	}

	str[i] = '\0';
	*length = i;
	return c == EOF;
}
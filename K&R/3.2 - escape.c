/* Exercise 3-2.
	Write a function escape(s,t) that converts characters like
	newline and tab into visible escape sequences like \n and
	\t as it copies the string t to s. Use a switch. Write a
	function for the other direction as well, converting escape
	sequences into the real characters.
*/

void escape(char s[], char t[]) {
	int i, j=0;
	for (int i=0; t[i] != '\0'; i++) {
		switch (t[i]) {
		/* only support n,t,r,b... seems fair enough */
		case '\n':
			s[j++] = '\\'; s[j++] = 'n';
			break;
		case '\t':
			s[j++] = '\\'; s[j++] = 't';
			break;
		case '\r':
			s[j++] = '\\'; s[j++] = 'r';
			break;
		case '\b':
			s[j++] = '\\'; s[j++] = 'b';
			break;

		default: s[j++] = t[i];
		}
	}
	s[j] = '\0';
}

void unescape(char s[], char t[]) {
	
}



#include <stdio.h>
#define MAX_LINE 1000

typedef char testStr[2][MAX_LINE];

void printTest(testStr s) {
	escape(s[1], s[0]);

	printf("raw:    \t'%s'\n"
	       "escaped:\t'%s'\n",
	       s[0], s[1]);
}

int main() {
	testStr a = { "Test C string\t\tThis  text\nhas one or two lines." };

	printTest(a);

	return 0;
}
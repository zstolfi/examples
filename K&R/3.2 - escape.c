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
		/* only support \,n,t,r,b... seems fair enough */
		case '\\':
			s[j++] = '\\'; s[j++] = '\\';
			break;
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

typedef char testStr[3][MAX_LINE];

void printTest1(testStr s) {
	escape(s[1], s[0]);

	printf("RAW:    \n%s\n"
	       "ESCAPED:\n%s\n\n",
	       s[0], s[1]);
}


void printTest2(testStr s) {
	escape(s[1], s[0]);
	unescape(s[2], s[0]);

	printf("RAW:      \n%s\n"
	       "ESCAPED:  \n%s\n"
	       "UNESCAPED:\n%s\n\n",
	       s[0], s[1], s[2]);
}

int main() {
	testStr a = { "Test C string\t\tThis  text\nhas one or two lines." };
	testStr b = { "1: \\ 2: \\\\ 3: \\\\\\ 4: \\\\\\\\" };
	testStr c = { "       /\\       \n"
	              "      /\\/\\      \n"
	              "     /\\  /\\     \n"
	              "    /\\/\\/\\/\\    \n"
	              "   /\\      /\\   \n"
	              "  /\\/\\    /\\/\\  \n"
	              " /\\  /\\  /\\  /\\ \n"
	              "/\\/\\/\\/\\/\\/\\/\\/\\" };
	testStr d = { "\tThis program takes strings (ex: \"Hello,\\tworld!\")\n"
	              "\tand decodes the slashes into their un-escaped meaning\n"
	              "\tor it prints the string as it appears in the source file." };
	testStr e = { "\"Hello,\\tworld!\\n\" ... \"Backticks (\\\\) are amazing!\"" };
	testStr f = { "\\s is not a valid escape sequence." };

	printTest1(a);
	printTest1(b);
	printTest1(c);

	printTest2(d);
	printTest2(e);
	printTest2(f);

	return 0;
}
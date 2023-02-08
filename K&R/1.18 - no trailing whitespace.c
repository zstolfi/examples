/* Exercise 1-18.
	Write a program to remove trailing blanks and tabs from
	each line of input, and to delete entirely blank lines.
*/

#include <stdio.h>
#define MAXLINE 1000	/* maximum input line size */

#define isWhitespace(c) (c == ' ' || c == '\t' || c == '\n')

int getline(char line[], int maxline);

int main() {
	int len;	/* current line length */
	char line[MAXLINE];   	/* current input line */

	while ((len = getline(line, MAXLINE)) > 0) {
		// -1 for the null character
		int lastWS = len-1;
		while (lastWS > 0 && isWhitespace(line[lastWS])) {
			lastWS--;
		}
		if (lastWS == 0) { continue; } /* blank line */
		printf("%.*s\n", lastWS+1, line);
	}
	return 0;
}

/* getline:  read a line into s, return length */
int getline(char s[], int lim) {
	int c, i;

	for (i=0; (c=getchar()) != EOF && c != '\n'; i++) {
		if (i+1 < lim) { s[i] = c; }
	}
	if (c == '\n') {
		if (i+1 < lim) { s[i] = c; }
		i++;
	}
	s[i] = '\0';
	return i;
}
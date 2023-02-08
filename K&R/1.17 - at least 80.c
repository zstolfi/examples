/* Exercise 1-17.
	Write a program to print all input lines that are longer
	than 80 characters.
*/

#include <stdio.h>
#define MAXLINE 1000	/* maximum input line size */
#define THRESHOLD 80	/* only lines longer than this are printed */

int getline(char line[], int maxline);

int main() {
	int len;	/* current line length */
	char line[MAXLINE];   	/* current input line */

	while ((len = getline(line, MAXLINE)) > 0) {
		// -1 for the null character
		if (len-1 > THRESHOLD) {
			printf("%s", line);
		}
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
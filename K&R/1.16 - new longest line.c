/* Exercise 1-16.
	Revise the main routine of the longest-line program so it
	will correctly print the length of arbitrarily long input
	lines, and as much as possible of the text.
*/

/*
	I'm going to assume the prompt meant to allow counting past
	MAXLINE, but still only display at most MAXLINE characters. 
*/
#include <stdio.h>
#define MAXLINE 1000	/* maximum input line size */

int getline(char line[], int maxline);
void copy(char to[], char from[]);

/* print longest input line */
int main() {
	int len;	/* current line length */
	int max;	/* maximuim length seen so far */
	char line[MAXLINE];   	/* current input line */
	char longest[MAXLINE];	/*  longest line saved here */

	max = 0;
	while ((len = getline(line, MAXLINE)) > 0) {
		if (len > max) {
			max = len;
			copy(longest, line);
		}
	}
	if (max > 0)	/* there was a line */
		printf("%s", longest);
	return 0;
}

/* getline:  read a line into s, return length */
int getline(char s[], int lim) {
	int c, i;

	for (i=0;(c=getchar()) != EOF && c != '\n'; i++) {
		if (i+1 < lim) { s[i] = c; }
	}
	if (c == '\n') {
		if (i+1 < lim) { s[i] = c; }
		i++;
	}
	s[i] = '\0';
	return i;
}

/* copy:  copy 'from' int' to'; assume to is big enough */
void copy(char to[], char from[]) {
	int i;

	i = 0;
	while ((to[i] = from[i]) != '\0') {
		i++;
	}
}
/* Exercise 1-19.
	Write a function reverse(s) that reverses the character
	string s. Use it to write a program that reverses its input
	a line at a time.
*/

#include <stdio.h>
#define MAXLINE 1000

int getline(char line[], int maxline);
void swap(char* a, char * b);

// string reverse function, modifies string in-place
void reverse(char s[]) {
	int len;
	for (len=0; s[len] != '\0'; len++) {}
	for (int i=0; i < len/2; i++) {
		int j = len-i-1;
		swap(&s[i], &s[j]);
	}
}

int main() {
	char line[MAXLINE];
	for (int len; (len = getline(line, MAXLINE)) > 0; ) {
		// remove possible ending \n
		if (line[len-1] == '\n') { line[len-1] = '\0'; }
		reverse(line);
		printf("%s\n", line);
	}

	return 0;
}

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

void swap(char* a, char* b) {
	char temp = *a;
	*a = *b;
	*b = temp;
}
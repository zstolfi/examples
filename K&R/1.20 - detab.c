/* Exercise 1-20.
	Write a program detab that replaces tabs in the input with
	the proper number of blanks to space to the next tab stop.
	Assume a fixed set of tab stops, say every n columns.
	Should n be a variable or a symbolic parameter?
*/

#include <stdio.h>
#define MAX_LINE 1000

#define TAB_SIZE 4

typedef enum { false = 0, true = 1 } bool;

/* Get Line: doesn't include '\n' at the end of strings */
/*           and returns true iff EOF is detected       */
bool getline(char str[], int lim, int* length);
void clearline(char str[], int len);

void detab(char in[], int len, char out[], int lim, const int n) {
	for (int i=0, j=0; i < len; i++, j++) {
		if (in[i] != '\t') {
			out[j] = in[i];
		} else {
			int tabWidth = n - j % n;
			while (tabWidth--) { out[j++] = ' '; }
			j--;
		}
	}
}



/* Main Program */
int main () {
	int len;
	char line[MAX_LINE], lineOutput[MAX_LINE];

	for (bool done = false; !done; ) {
		done = getline(line, MAX_LINE, &len);

		clearline(lineOutput, MAX_LINE);
		detab(line, len, lineOutput, MAX_LINE, TAB_SIZE);
		printf("%s\n", lineOutput);
	}

	return 0;
}

void clearline(char str[], int len) {
	for (int i=0; i < len; i++) { str[i] = '\0'; }
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
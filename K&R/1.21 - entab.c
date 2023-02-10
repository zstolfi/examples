/* Exercise 1-20.
	Write a program entab that replaces strings of blanks by
	the minimum number of tabs and blanks to achieve the same
	spacing. Use the same tab stops as for detab. When either a
	tab or a single blank would suffice to reach a tab stop,
	which should be given preference?
*/

#include <stdio.h>
#define MAX_LINE 1000	/* maximum chars per line */
#define MAX_BLANK 500	/* maximum number of whitespace ranges */

#define TAB_SIZE 4

typedef enum { false = 0, true = 1 } bool;
typedef struct { int i0, i1; } charRange;

/* Get Line: doesn't include '\n' at the end of strings */
/*           and returns true iff EOF is detected       */
bool getline(char str[], int lim, int* length);
void clearline(char str[], int len);

/* Find Whitespaces: returns array length */
int findWhitespaces(charRange blanks[], char str[], int len) {
	enum { WORDS, WHITESPACE } state = WORDS, prevState = WORDS;
	int j = 0;
	for (int i=0; i < len; i++) {
		// update state
		state = (str[i] == ' ' || str[i] == '\t') ? WHITESPACE : WORDS;

		// first space
		if (state == WHITESPACE && prevState == WORDS)
			blanks[j  ].i0 = i;
		// first word char
		if (state == WORDS && prevState == WHITESPACE)
			blanks[j++].i1 = i;

		prevState = state;
	}
	if (state == WHITESPACE)
		blanks[j++].i1 = len;
	return j;
}

/* Previous Tab Stop: helper funciton which gives previous tab space*/
int prevTabStop(int i, const int n) {
	return n * (i/n);
}

/* Entab: finds ranges of whitespaces first, then replaces */
void entab(char in[], int len, char out[], int lim, const int n) {
	charRange blanks[MAX_BLANK];
	int blanksLen = findWhitespaces(blanks, in, len);
	int blankIndex = 0;
	for (int i=0, j=0; i < len; i++, j++) {
		charRange* curBlank = &blanks[blankIndex];
		if (curBlank->i0 == i) {
			int tabSpace0 = prevTabStop(curBlank->i0, n);
			int tabSpace1 = prevTabStop(curBlank->i1, n);
			// insert 0 or more tabs into out string
			int numTabs = (tabSpace1 - tabSpace0) / n;
			while (numTabs--) { out[j++] = '\t'; }
			// insert the rest of the spaces to pad it out
			int numSpaces = curBlank->i1 - tabSpace1;
			while (numSpaces--) { out[j++] = ' '; }
			j--;
			i = curBlank->i1 - 1;
			blankIndex++;
		} else {
			out[j] = in[i];
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
		entab(line, len, lineOutput, MAX_LINE, TAB_SIZE);
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
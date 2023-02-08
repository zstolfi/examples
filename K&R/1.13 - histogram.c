/* Exercise 1-13.
	Write  a program to print a histogram of the length of
	words in its input. It is easy to draw the histogram with
	the bars horizontal; a vertical orientation is more
	challenging.
*/

#include <stdio.h>

#define MAX_LENGTH 16

void addHistogram(int h[], int len, int n) {
	int i = (n <= len) ? n : len + 1;
	h[i]++;
}
void printHistogram(int h[], int len);

enum state { WORDS_FIRST, WORDS, SPACES_FIRST, SPACES };
void updateState(enum state* s, char c) {
	#define isWhitespace(c) (c == ' ' || c == '\t' || c == '\n')
	/**/ if (*s == WORDS_FIRST ) { *s = isWhitespace(c) ? SPACES_FIRST : WORDS; }
	else if (*s == WORDS       ) { *s = isWhitespace(c) ? SPACES_FIRST : WORDS; }
	else if (*s == SPACES_FIRST) { *s = isWhitespace(c) ? SPACES : WORDS_FIRST; }
	else if (*s == SPACES      ) { *s = isWhitespace(c) ? SPACES : WORDS_FIRST; }
}

main () {
	// 2 extra, for first and last term
	int histogram[MAX_LENGTH + 2] = {0};

	int wordLen = 0;

	enum state parseState = WORDS_FIRST;
	for (int c; (c = getchar()) != EOF; ) {
		updateState(&parseState, c);
		switch (parseState) {
			case WORDS_FIRST:  wordLen = 1; break;
			case WORDS:        wordLen++;   break;
			case SPACES_FIRST: addHistogram(histogram, MAX_LENGTH, wordLen); break;
		}
	}
	// last word (EOF terminated)
	if (parseState == WORDS_FIRST || parseState == WORDS)
		addHistogram(histogram, MAX_LENGTH, wordLen);

	printHistogram(histogram, MAX_LENGTH + 2);
}

void printHistogram(int h[], int len) {
	for (int i=0; i < len; i++) {
		if (i+1 != len)
			printf("%d\t:\t", i);
		else
			printf(">%d\t:\t", i-1);
		for (int j=0; j < h[i]; j++) { putchar('#'); }
		printf("\n");
	}
}
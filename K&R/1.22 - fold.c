/* Exercise 1-22.
	Write a program to "fold" long input lines into two or more
	shorter lines after the last non-blank character that
	occurs before the n-th column of input. Make sure your
	program does something intelligent with very long lines,
	and if there are no blanks or tabs before the specified
	column.
*/

#include <stdio.h>
#define WIDTH 64    	/* characters per row */
#define MAX_WORD 100	/* max word length */

/*  Storing the next character gives us room to make decisions
 *  without eating up the character. 
 */
int nextChar;
int get(); 	/* acts like getchar() */
int peek();	/* reads the 'next' char, without modifying */

/*  In this program, character input is categorized by 'tokens'
 *  a string of non-whitespace is a WORD token, a string of all
 *  whitespace is a SPACE token. NEW_LINE is any string of 1 or
 *  more new-lines. END acts like EOF, where has a length of 0.
 */
enum token { WORD, SPACE, NEW_LINE, END };
enum token getWord(char str[], int lim, int* lengthOut );

int main() {
	nextChar = getchar();

	/* page width, for reference */
	for (int i=0; i < WIDTH; i++) { putchar('#'); }
	printf("\n\n");

	int x=0;
	int len;
	enum token type;
	char word[MAX_WORD];
	while ((type = getWord(word, MAX_WORD, &len)) != END) {
		if (type == NEW_LINE) {
			printf("%s", word);
			x = 0;
			continue;
		}

		x += len;
		if (x <= WIDTH) {
			printf("%s", word);

		} else /*x > WIDTH*/ {
			if (type == WORD) {
				printf("\n%s", word);
				x = len;
			} else if (type == SPACE) {
				printf("\n");
				x = 0;
			}
		}
	}

	return 0;
}



int get() {
	int prev = nextChar;
	nextChar = getchar();
	return prev;
}

int peek() {
	return nextChar;
}

enum token getType(char c) {
	if (c == ' ' || c == '\t') { return SPACE;    }
	else if (c == '\n')        { return NEW_LINE; }
	else                       { return WORD;     }
}

enum token getWord(char str[], int lim, int* lengthOut) {
	if (peek() == EOF) { lengthOut = 0; return END; }

	enum token type = getType(peek());

	int c, i=0;
	while (peek() != EOF && type == getType(peek())) {
		c = get();
		if (i+1 < lim) { str[i] = c; }
		i++;
	}

	int last = (lim < i) ? lim : i;
	str[last] = '\0';
	*lengthOut = i;
	return type;
}
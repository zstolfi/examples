#include <stdio.h>

#define WORDS       0
#define FIRST_SPACE 1
#define SPACES      2

#define isWhitespace(c) (c == ' ' || c == '\t' || c == '\n')

/* print input one word per line */
main () {
	int c, state;

	state = WORDS;
	while ((c = getchar()) != EOF) {
		/**/ if (state == WORDS      ) { state = isWhitespace(c) ? FIRST_SPACE : WORDS; }
		else if (state == FIRST_SPACE) { state = isWhitespace(c) ? SPACES      : WORDS; }
		else if (state == SPACES     ) { state = isWhitespace(c) ? SPACES      : WORDS; }

		if (state == WORDS)       { putchar(c); }
		if (state == FIRST_SPACE) { putchar('\n'); }
		if (state == SPACES)      { /*do nothing*/ }
	}
}
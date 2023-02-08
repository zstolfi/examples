#include <stdio.h>

main() {
	enum {
		WORDS, FIRST_SPACE, SPACES
	} state = WORDS;

	int c;
	while ((c = getchar()) != EOF) {
		switch (state) {
		case WORDS:
			if (c == ' ') { state = FIRST_SPACE; }
			putchar(c);
			break;
		case FIRST_SPACE:
			if (c == ' ') {
				state = SPACES;
				/* print nothing */
			} else {
				state = WORDS;
				putchar(c);
			}
			break;
		case SPACES:
			if (c == ' ') {
				/* print nothing */
			} else {
				state = WORDS;
				putchar(c);
			}
			break;
		}
	}
}
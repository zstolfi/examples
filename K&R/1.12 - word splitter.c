#include <stdio.h>

/* print input one word per line */
main () {
	int c;
	while ((c = getchar()) != EOF) {
		/**/ if (c == ' ' ) { putchar('\n'); }
		else if (c == '\n') { putchar('\n'); }
		else /*          */ { putchar(c);    }
	}
}
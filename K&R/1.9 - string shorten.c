#include <stdio.h>

main() {
	int c;
	int prev = -1;
	while ((c = getchar()) != EOF) {
		if ((c == ' ' && prev != ' ') || c != ' ') {
			putchar(c);
		}

		prev = c;
	}
}
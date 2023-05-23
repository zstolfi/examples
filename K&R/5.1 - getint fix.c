/* Exercise 5.1
	As written, getint treats a + or - not followed by a digit
	as a valid representation of zero. Fix it to push such a
	character back on the input.
*/

#include <stdio.h>
#include <ctype.h>

int getch(void);
void ungetch(int);

/* getint:  get next integer from input into *pn */
int getint(int* pn) {
	int c, sign;

	while (isspace(c = getch())) /* skip whitespace */
		;

	if (!isdigit(c) && c != EOF && c != '+' && c != '-') {
		ungetch(c); /* it's not a number */
		return 0;
	}
	sign = (c == '-') ? -1 : 1;
	if (c == '+' || c == '-') {
		if (c = getch(), !isdigit(c)) {
			ungetch(c); return 0;
		}
	}

	for (*pn = 0; isdigit(c); c = getch())
		*pn = 10 * *pn + (c - '0');

	*pn *= sign;

	if (c != EOF)
		ungetch(c);
	return c;
}



#define SIZE 10

int main(void) {
	int getint(int*);
	int n, numread, array[SIZE];

	printf("Enter 10 integers:\n");
	for (n = numread = 0; n < SIZE && getint(&array[n]) > 0; n++)
		numread++;

	for (int i = 0; i < numread; i++)
		printf("%d\n", array[i]);

	return 0;
}



static char heldchar;
static int isheld = 0;

int getch(void) {
	return (isheld) ? (isheld = 0, heldchar) : getchar();
}

void ungetch(int c) {
	/* EOF case not handled (I have
	   not gotten to that exercise) */
	if (isheld)
		perror("ungetch: too many characters\n");
	else
		isheld = 1, heldchar = c;
}
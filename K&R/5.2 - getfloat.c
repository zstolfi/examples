/* Exercise 5-2.
	Write getfloat, the floating-point analog of getint. What
	type does getfloat return as its function value?
*/

#include <stdio.h>
#include <ctype.h>

int getch(void);
void ungetch(int);

/* getfloat directly returns an int to allow for EOF
   values to be detected (same reason getint returns
   an int). Instead *pn will be a floating point val
   assigned to by the function, much like returning. */
int getfloat(float* pn) {
	int c, sign;
	float frac, power = 1;
	int intpart = 0;

	while (isspace(c = getch())) /* skip whitespace */
		;

	if (!isdigit(c) && c != EOF && c != '+' && c != '-' && c != '.') {
		ungetch(c); /* it's not a number */
		return 0;
	}
	sign = (c == '-') ? -1 : 1;
	if (c == '+' || c == '-') {
		if (c = getch(), !isdigit(c) && c != '.') {
			ungetch(c); return 0;
		}
	}

	for (*pn = 0; isdigit(c); c = getch()) {
		intpart = 1;
		*pn = 10 * *pn + (c - '0');
	}

	if (c == '.') {
		if (c = getch(), !intpart && !isdigit(c)) {
			ungetch(c); return 0;
		}
		for (power = 1; isdigit(c); c = getch())
			*pn += (power /= 10) * (c - '0');
	}

	*pn *= sign;

	if (c != EOF)
		ungetch(c);
	return c;
}



#define SIZE 10

int main(void) {
	int getfloat(float*);
	int n, numread;
	float array[SIZE];

	printf("Enter 10 numbers:\n");
	for (n = numread = 0; n < SIZE && getfloat(&array[n]) > 0; n++)
		numread++;

	for (int i = 0; i < numread; i++)
		printf("%g\n", array[i]);

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
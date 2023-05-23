/* Exercise 4-8.
	Suppose that there will never be more than one character of
	pushback. Modify getch and ungetch accordingly.
*/


char heldchar;
int isheld = 0;

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
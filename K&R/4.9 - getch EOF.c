/* Exercise 4-9.
	Our getch and ungetch do not handle a pushed-back EOF
	correctly. Decide what their properties ought to be if an
	EOF is pushed back, then implement your design.
*/


/* I think the buffer should still hold only char vals
   but support for EOF should be allowed (the original
   getchar() does after all). For my design, I added a
   boolean variable for if the last char was EOF. This
   however, doesn't work when there are >= 2 EOF chars
   in a row. That would be weird so I think it's fine. */

#define BUFSIZE 100

char buf[BUFSIZE];
int bufp = 0;
int bufhasEOF = 0;

int getch(void) {
	if (bufhasEOF) {
		bufhasEOF = 0;
		return EOF;
	} else if (bufp > 0)
		return buf[--bufp];
	else
		return getchar();
}

void ungetch(int c) {
	if (bufp >= BUFSIZE)
		perror("ungetch: too many characters\n");
	else if (c == EOF)
		bufhasEOF = 1;
	else
		buf[bufp++] = c;
}



/* TODO: add test */
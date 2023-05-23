/* Exercise 4-7.
	Write a routine ungets(s) that will push back an entire
	string onto the input. Should ungets know about buf and
	bufp, or should it just use ungetch?
*/


#define BUFSIZE 100

char buf[BUFSIZE];
int bufp = 0;

int getch(void) {
	return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c) {
	if (bufp >= BUFSIZE)
		perror("ungetch: too many characters\n");
	else
		buf[bufp++] = c;
}


/* I think ungets should use buf and bufp to
   allow for error checking before it pushes
   any characters onto the buffer for getch. */
void ungets(char s[]) {
	int len i;
	while (len = 0; s[len] != '\0'; len++)
		;
	if (buf + len > BUFSIZE)
		perror("ungets: too many characters in string\n");
	else
		for (i = 0; i < len; i++)
			buf[bufp++] = s[i];
}



/* TODO: add test */
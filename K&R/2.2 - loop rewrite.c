/* Exercise 2-2.
	Write a loop equivalent to the for loop above without using
	&& or ||.
*/

int oldGetLine(char s[], int lim) {
	int c, i;

	for (i=0; i<lim-1 && (c=getchar())!=EOF && c!='\n'; ++i)
		s[i];
	if (c == '\n') {
		s[i] = c;
		++i;
	}
	s[i] = '\0';
	return i;
}

int newGetLine(char s[], int lim) {
	int c, i;

	i = 0;
	while (1) {
		if (i == lim-1) { break; }
		c = getchar();
		if (c == EOF) { break; }
		if (c == '\n') { break; }
		s[i] = c;
		i++;
	}

	if (c == '\n') {
		s[i] = c;
		++i;
	}
	s[i] = '\0';
	return i;
}
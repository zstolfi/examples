/* Exercise 5-5.
	Write versions of the library functions strncpy, strncat,
	and strncmp, which operate on at most the first n
	characters of their argument strings. For example,
	strncpy(s,t,n) copies at most n characters of t to s. Full
	descriptions are in Appendix B.
*/

/* It wasn't said whether or not to use
   pointers but I will use them anyway. */

void strncpy(char* s, char* t, int n) {
	while (--n > 0 && *t != '\0')
		*s++ = *t++;
}

void strncat(char* s, char* t, int n) {
	while (*s != '\0')
		s++;
	while (--n > 0 && *t != '\0')
		*s++ = *t++;
}

int strncmp(char* s, char* t, int n) {
	while (--n > 0 && *s && *t)
		if (*s != *t)
			return *s - *t;
		else
			s++, t++;
	return 0;
}



/* TODO: add test */
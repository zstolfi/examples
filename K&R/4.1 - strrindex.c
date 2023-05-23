/* Exercise 4-1.
	Write the function strrindex(s,t), which returns the
	position of the rightmost occurrence of t in s, or -1 if
	there is none.
*/

#include <string.h>

#if 0 /* naive approach, this doesn't work on example 4 */
int strrindex(char s[], char t[]) {
	int slen = strlen(s);
	int tlen = strlen(t);
	int result = -1;
	if (slen == 0 || tlen == 0) { return result; }

	int i, j;
	for (i=j=0; i < slen; i++) {
		if (s[i] == t[j]) j++; else j = 0;
		if (j == tlen)
			result = i-tlen+1, j = 0;
	}
	return result;
}
#else /* correct behavior */
int strrindex(char s[], char t[]) {
	int slen = strlen(s);
	int tlen = strlen(t);
	int result = -1;
	if (slen == 0 || tlen == 0) { return result; }

	int i, j;
	for (i=0; i < slen; i++) {
		for (j=0; j < tlen; j++)
			if (s[i+j] != t[j]) break;
		if (j == tlen)
			result = i;
	}
	return result;
}
#endif

#include <stdio.h>

int main(void) {
	/*1*/ printf("%d\n", strrindex("could should would", "ould"));
	/*2*/ printf("%d\n", strrindex("12345678987654321" , "20"));
	/*3*/ printf("%d\n", strrindex("12345678987654321" , "21"));
	/*4*/ printf("%d\n", strrindex("aaaaaaaaaaa"       , "aaaa"));
	/*5*/ printf("%d\n", strrindex("Example string."   , ""));

	return 0;
}
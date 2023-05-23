/* Exercise 4-13.
	Write the recursive version of the function reverse(s),
	which reverses the string s in place.
*/

void reverse_iter(char s[], int len) {
	if (len < 2)
		return;
	/* keep track of first */
	char first = s[0];
	/* shift down */
	for (int i=1; i<len; i++)
		s[i-1] = s[i];
	/* move the first to the end */
	s[len-1] = first;
	/* repeat with sub-string */
	reverse_iter(s, len-1);
}

void reverse(char s[]) {
	int len = 0;
	while (s[len] != '\0')
		len++;
	reverse_iter(s, len);
}



#include <stdio.h>

void printtest(char str[]) {
	printf("'%s'\n", str);
	reverse(str);
	printf("'%s'\n\n", str);
}

int main(void) {
	char test1[] = "123456789";
	char test2[] = ".";
	char test3[] = "";
	printtest(test1);
	printtest(test2);
	printtest(test3);

	return 0;
}
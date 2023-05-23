/* Exercise 5-4.
	Write the function strend(s,t), which returns 1 if the
	string t occurs at the end of the string s, and zero
	otherwise.
*/

#include <stdbool.h>

bool strend(char* s, char* t) {
	char* s0 = s;
	char* t0 = t;
	while (*s != '\0') s++;
	while (*t != '\0') t++;
	if (t-t0 > s-s0)  /* t is longer than s, meaning */
		return false; /* s cannot possibly contain t */

	while (t >= t0) /* iterate and check backwards */
		if (*s-- != *t--) return false;

	return true;
}



#include <stdio.h>

void test(char* str1, char* str2) {
	printf("(%d) '%s' '%s'\n",
	       strend(str1,str2),
	       str1,
	       str2);
}

int main(void) {
	test("I know a thing or two about her.",
	     "about");
	test("e = 2.718281828",
	     "1828");
	test("trivial",
	     "");
	/* It's important to remember that str1 could be
	   be a substring, meaning it might match beyond
	   its intended begining, meaning the size check
	   at lines 14-15 are necessary to the function. */
	char* s = "full string";
	test(&s[5],
	     &s[0]);

	return 0;
}
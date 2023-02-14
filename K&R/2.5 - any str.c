/* Exercise 2-5.
	Write the function any(s1,s2), which returns the first
	location in the string s1 where any character from the
	string s2 occurs, or -1 if s1 contains no characters from
	s2. (The standard library function strpbrk does the same
	job but returns a pointer to the location.)
*/

#include <stdio.h>

char* find(char c, char s[]) {
	for (int i=0; s[i] != '\0'; i++) {
		if (c == s[i]) { return &s[i]; }
	}
	return NULL;
}

int any(char s1[], char s2[]) {
	for (int i=0; s1[i] != '\0'; i++) {
		if (find(s1[i], s2)) { return i; }
	}
	return -1;
}

void printTest(char s1[], char s2[]) {
	int pos = any(s1, s2);
	if (pos != -1)
		printf("\"%s\" <-> \"%s\"\n" "The first match is the character '%c' at index %d\n", s1, s2, s1[pos], pos);
	else
		printf("\"%s\" <-> \"%s\"\n" "There was no match.\n", s1, s2);
}

int main() {
	typedef char* testParam[2];
	testParam a = {"This is an example sentence.", "aeo"};
	testParam b = {"3.1415926", "-7"};

	printTest(a[0], a[1]);
	printTest(b[0], b[1]);

	return 0;
}
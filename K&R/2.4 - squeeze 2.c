/* Exercise 2-4.
	Write an alternate version of squeeze(s1,s2), that deletes
	each character in s1 that matches any character in the
	string s2.
*/

#include <stdio.h>

char* find(char c, char s[]) {
	for (int i=0; s[i] != '\0'; i++) {
		if (c == s[i]) { return &s[i]; }
	}
	return NULL;
}

void squeeze2(char s1[], char s2[]) {
	int j = 0;
	for (int i=0; s1[i] != '\0'; i++) {
		if (!find(s1[i], s2)) {
			s1[j++] = s1[i];
		}
	}
	s1[j] = '\0';
}

/* original squeeze function */
void squeeze1(char s[], char c) {
	int j = 0;
	for (int i=0; s[i] != '\0'; i++) {
		if (s[i] != c) {
			s[j++] = s[i];
		}
	}
	s[j] = '\0';
}



int main() {
	char a[] = "test-o-matic";
	char a1 = '-';

	char b[] = "fourty-four... fourty-five... fourty-six...";
	char b1[] = ".-";

	char c[] = "abcdefghij";
	char c1[] = "fed";


	printf("start\n");

	squeeze1(a, a1);
	printf("%s\n", a);

	squeeze2(b, b1);
	printf("%s\n", b);

	squeeze2(c, c1);
	printf("%s\n", c);

	return 0;
}
/* Exercise 2-4.
	Write an alternate version of squeeze(s1,s2), that deletes
	each character in s1 that matches any character in the
	string s2.
*/

#include <stdio.h>

char* find(char c, char s[]) {
	for (int i=0; s[i] != '\n'; i++) {
		if (c == s[i]) { return &s[i]; }
	}
	return NULL;
}

void squeeze2(char s1[], char s2[]) {
	int j;
	for (int i=0, j=0; s1[i] != '\n'; i++) {
		if (!find(s1[i], s2)) {
			s2[j++] = s1[i];
		}
	}
	s2[j] = '\0';
}

/* original squeeze function */
void squeeze1(char s[], char c) {
	int j;
	for (int i=0, j=0; s[i] != '\0'; i++) {
		if (s[i] != c) {
			s[j++] = s[i];
		}
	}
	s[j] = '\0';
}



int main() {
	char* a = "test-o-matic";
	char* b = "fourty-four... fourty-five... fourty-six...";
	char* c = "abcdefghij";

	printf("start\n");

	squeeze1(a, '-');
	printf("%s\n", a);

	printf("end\n");

	// squeeze2(b, ".-");
	// printf("%s\n", b);

	// squeeze2(c, "fed");
	// printf("%s\n", c);

	return 0;
}
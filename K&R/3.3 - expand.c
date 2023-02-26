/* Exercise 3-3.
	Write a function expand(s1,s2) that expands shorthand
	notations like a-z in the string s1 into the equivalent
	complete list abc...xyz in s2. Allow for letters of either
	case and digits, and be prepared to handle cases like a-b-c
	and a-z0-9 and -a-z. Arrange that a leading or trailing -
	is taken literally.
*/

#define Is_Lower(c)  ('a' <= c&&c <= 'z')
#define Is_Upper(c)  ('A' <= c&&c <= 'Z')
#define Is_Number(c) ('0' <= c&&c <= '9')

void expand(char s1[], char s2[]) {
	int i, j=0;
	for (i=0; s1[i] != '\0'; i++) {
		/* acceptable positions for expanding '-' */
		if (s1[i] == '-' && i >= 1 && s1[i+1] != '\0') {
			char arg1 = s1[i-1];
			char arg2 = s1[i+1];
			/* acceptable character ranges */
			if (arg1 < arg2
			&& ((Is_Lower (arg1) && Is_Lower (arg2))
			||  (Is_Upper (arg1) && Is_Upper (arg2))
			||  (Is_Number(arg1) && Is_Number(arg2)))) {
				i++; /* skip over the '-' itself */
				for (char c = arg1+1; c < arg2; c++) {
					s2[j++] = c;
				}
			}
		}

		s2[j++] = s1[i];
	}
	s2[j] = '\0';
}



#include <stdio.h>
#include <stdbool.h>
#define MAX_LINE 1000

bool EOFreached = false;
bool getline(char str[], int lim, int* length) {
	if (EOFreached) { *length = 0; return false; }

	int c, i = 0;
	while ((c = getchar()) != EOF && c != '\n') {
		if (i+1 < lim) { str[i] = c; }
		i++;
	}
	if (c == EOF) { EOFreached = true; }

	str[i] = '\0';
	*length = i;
	return true;
}

int main() {
	int len;
	char linein[MAX_LINE];
	char lineout[MAX_LINE];
	while (getline(linein, MAX_LINE, &len)) {
		expand(linein, lineout);
		if (len > 0) { printf("%s\n", lineout); }
	}

	return 0;
}
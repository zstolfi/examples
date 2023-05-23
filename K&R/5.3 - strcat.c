/* Exercise 5-3.
	Write a pointer version of the function strcat that we
	showed in Chaper 2: strcat(s,t) copies the string t to the
	end of s.
*/

#if 0 /* array version */
void strcat(char s[], char t[]) {
	int i, j;

	i = j = 0;
	while (s[i] != '\0')
		i++;
	while ((s[i++] = t[j++]) != '\0')
		;
}
#else /* pointer version */
void strcat(char* s, char* t) {
	while (*s != '\0')
		s++;
	while ((*s++ = *t++) != '\0')
		;
}
#endif



#include <stdio.h>

int main(void) {
	char str1[64] = "[This is string 1]";
	char str2[64] = "[This is string 2]";
	strcat(str1, str2);
	printf("%s\n", str1);

	return 0;
}
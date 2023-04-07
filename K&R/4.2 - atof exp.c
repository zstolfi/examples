/* Exercise 4-2.
	Extend atof to handle scientific notation of the form
	123.45e-6 where a floating-point number may be followed by
	e or E and an optionally signed exponent.
*/

#include <ctype.h>

#if 0	/* old atof */

double atof(char s[]) {
	double val, power;
	int i, sign;

	for (i = 0; isspace(s[i]); i++) {}

	sign = (s[i] == '-') ? -1 : 1;
	if (s[i] == '+' || s[i] == '-')
		i++;
	for (val = 0.0; isdigit(s[i]); i++)
		val = 10.0 * val + (s[i] - '0');
	if (s[i] == '.')
		i++;
	for (power = 1.0; isdigit(s[i]); i++) {
		val = 10.0 * val + (s[i] - '0');
		power *= 10.0;
	}
	return sign * val / power;
}

#else	/* new atof */

double atof(char s[]) {
	double val, power;
	int i, j, sign, exp, expsign;

	for (i = 0; isspace(s[i]); i++) {}

	sign = (s[i] == '-') ? -1 : 1;
	if (s[i] == '+' || s[i] == '-')
		i++;
	for (val = 0.0; isdigit(s[i]); i++)
		val = 10.0 * val + (s[i] - '0');
	if (s[i] == '.')
		i++;
	for (power = 1.0; isdigit(s[i]); i++) {
		val = 10.0 * val + (s[i] - '0');
		power /= 10.0;
	}
	if (s[i] == 'e' || s[i] == 'E') {
		i++;
		expsign = (s[i] == '-') ? -1 : 1;
		if (s[i] == '+' || s[i] == '-')
			i++;
		for (exp = 0; isdigit(s[i]); i++)
			exp = 10 * exp + (s[i] - '0');
		for (j = 0; j < exp; j++)
			power = (expsign == 1) ? power*10 : power/10;
	}
	return sign * val * power;
}

#endif



#include <stdio.h>

int main(void) {
	char s1[] = "3.14159";
	char s2[] = "49";
	char s3[] = "-66.";
	char s4[] = ".007";
	char s5[] = "123.45e-6";
	char s6[] = "1e+6";

	printf("'%s' = %g\n", s1, atof(s1));
	printf("'%s' = %g\n", s2, atof(s2));
	printf("'%s' = %g\n", s3, atof(s3));
	printf("'%s' = %g\n", s4, atof(s4));
	printf("'%s' = %g\n", s5, atof(s5));
	printf("'%s' = %g\n", s6, atof(s6));

	return 0;
}
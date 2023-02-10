/* Exercise 2-1.
	Write a program to determine the ranges of char, short,
	int, and long variables, both signed and unsigned, by
	printing appropriate values from standard headers and by
	direct computation. Harder if you compute them: determine
	the ranges of the various floating point types.
*/

#include <stdio.h>
#include <limits.h>

int main() {
	// OK... naive way. (works for small values)
	// O(n) time
	unsigned char ucharMin = 0, ucharMax = 0, next_uchar;
	signed char charMin = 0, charMax = 0, next_char;
	next_uchar = ucharMax; while (++next_uchar >  0) { ucharMax++; }
	next_char  = charMin;  while (--next_char  <  0) { charMin--;  }
	next_char  = charMax;  while (++next_char  >= 0) { charMax++;  }

	unsigned short ushortMin = 0, ushortMax = 0, next_ushort;
	signed short shortMin = 0, shortMax = 0, next_short;
	next_ushort = ushortMax; while (++next_ushort >  0) { ushortMax++; }
	next_short  = shortMin;  while (--next_short  <  0) { shortMin--;  }
	next_short  = shortMax;  while (++next_short  >= 0) { shortMax++;  }



	printf("x~~~~~~~~~~~~x\n");
	printf("|    CHAR    |\n");
	printf("x~~~~~~~~~~~~x\n");
	printf("unsigned         :\t%hhu - %hhu\n", (unsigned char)0, UCHAR_MAX);
	printf("unsigned computed:\t%hhu - %hhu\n", ucharMin, ucharMax);
	printf("signed           :\t%hhd - %hhd\n", SCHAR_MIN, SCHAR_MAX);
	printf("signed computed  :\t%hhd - %hhd\n", charMin, charMax);
	printf("\n");

	printf("x~~~~~~~~~~~~~x\n");
	printf("|    SHORT    |\n");
	printf("x~~~~~~~~~~~~~x\n");
	printf("unsigned         :\t%hu - %hu\n", (unsigned short)0, USHRT_MAX);
	printf("unsigned computed:\t%hu - %hu\n", ushortMin, ushortMax);
	printf("signed           :\t%hd - %hd\n", SHRT_MIN, SHRT_MAX);
	printf("signed computed  :\t%hd - %hd\n", shortMin, shortMax);
	printf("\n");

	// printf("x~~~~~~~~~~~x\n");
	// printf("|    INT    |\n");
	// printf("x~~~~~~~~~~~x\n");
	// printf("unsigned         :\t%u - %u\n", (unsigned int)0, UINT_MAX);
	// printf("unsigned computed:\t%u - %u\n", uintMin, uintMax);
	// printf("signed           :\t%d - %d\n", INT_MIN, INT_MAX);
	// printf("signed computed  :\t%d - %d\n", intMin, intMax);
	// printf("\n");

	// printf("x~~~~~~~~~~~~x\n");
	// printf("|    LONG    |\n");
	// printf("x~~~~~~~~~~~~x\n");
	// printf("unsigned         :\t%lu - %lu\n", (unsigned long)0, ULONG_MAX);
	// printf("unsigned computed:\t%lu - %lu\n", ulongMin, ulongMax);
	// printf("signed           :\t%ld - %ld\n", LONG_MIN, LONG_MAX);
	// printf("signed computed  :\t%ld - %ld\n", longMin, longMax);
	// printf("\n");

	return 0;
}
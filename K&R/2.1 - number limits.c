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
	// 25 as a placeholder
	#define Temp_Init(TYPE) \
		unsigned TYPE u##TYPE##Min = 25, u##TYPE##Max = 25; \
		signed TYPE TYPE##Min = 25, TYPE##Max = 25;

	Temp_Init(char);
	Temp_Init(short);
	Temp_Init(int);
	Temp_Init(long);


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

	printf("x~~~~~~~~~~~x\n");
	printf("|    INT    |\n");
	printf("x~~~~~~~~~~~x\n");
	printf("unsigned         :\t%u - %u\n", (unsigned int)0, UINT_MAX);
	printf("unsigned computed:\t%u - %u\n", uintMin, uintMax);
	printf("signed           :\t%d - %d\n", INT_MIN, INT_MAX);
	printf("signed computed  :\t%d - %d\n", intMin, intMax);
	printf("\n");

	printf("x~~~~~~~~~~~~x\n");
	printf("|    LONG    |\n");
	printf("x~~~~~~~~~~~~x\n");
	printf("unsigned         :\t%lu - %lu\n", (unsigned long)0, ULONG_MAX);
	printf("unsigned computed:\t%lu - %lu\n", ulongMin, ulongMax);
	printf("signed           :\t%ld - %ld\n", LONG_MIN, LONG_MAX);
	printf("signed computed  :\t%ld - %ld\n", longMin, longMax);
	printf("\n");

	return 0;
}
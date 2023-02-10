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

	/* CHAR */
	unsigned char ucharMin = 0, ucharMax = 0, next_uchar;
	signed char charMin = 0, charMax = 0, next_char;
	next_uchar = 0; while (++next_uchar >  0) { ucharMax++; }
	next_char  = 0; while (--next_char  <  0) { charMin--;  }
	next_char  = 0; while (++next_char  >= 0) { charMax++;  }

	/* SHORT */
	unsigned short ushortMin = 0, ushortMax = 0, next_ushort;
	signed short shortMin = 0, shortMax = 0, next_short;
	next_ushort = 0; while (++next_ushort >  0) { ushortMax++; }
	next_short  = 0; while (--next_short  <  0) { shortMin--;  }
	next_short  = 0; while (++next_short  >= 0) { shortMax++;  }

	// Better method, increase then decrease the step size
	// O(log(n)) time
	long step = 1;
	long s2;

	#define increaseStep() \
	    s2 = step*2; \
	    step = (s2 > step) ? s2 : LONG_MAX;

	#define decreaseStep() \
	    s2 = step/2; \
	    step = (s2 < step) ? s2 : 0;

	/* INT */
	unsigned int uintMin = 0, uintMax = 0, next_uint;
	signed int intMin = 0, intMax = 0, next_int;
	next_uint = 0;
	step = 1;
	do { // widening steps
		uintMax = next_uint;
		next_uint += step;
		increaseStep();
	} while (next_uint > uintMax);
	do { // diminishing steps
		next_uint = uintMax;
		while ((next_uint += step) > uintMax) {
			uintMax = next_uint;
		}
		decreaseStep();
	} while (step > 0);

	next_int = 0;
	step = 1;
	do {
		intMax = next_int;
		next_int += step;
		increaseStep();
	} while (next_int > intMax);
	do {
		next_int = intMax;
		while ((next_int += step) > intMax) {
			intMax = next_int;
		}
		decreaseStep();
	} while (step > 0);

	next_int = 0;
	step = 1;
	do {
		intMin = next_int;
		next_int -= step;
		increaseStep();
	} while (next_int < intMin);
	do {
		next_int = intMin;
		while ((next_int -= step) < intMin) {
			intMin = next_int;
		}
		decreaseStep();
	} while (step > 0);

	/* LONG */
	unsigned long ulongMin = 0, ulongMax = 0, next_ulong;
	signed long longMin = 0, longMax = 0, next_long;
	next_ulong = 0;
	step = 1;
	do { // widening steps
		ulongMax = next_ulong;
		next_ulong += step;
		increaseStep();
	} while (next_ulong > ulongMax);
	do { // diminishing steps
		next_ulong = ulongMax;
		while ((next_ulong += step) > ulongMax) {
			ulongMax = next_ulong;
		}
		decreaseStep();
	} while (step > 0);

	// bleh, duplicate code :(
	next_long = 0;
	step = 1;
	do {
		longMax = next_long;
		next_long += step;
		increaseStep();
	} while (next_long > longMax);
	do {
		next_long = longMax;
		while ((next_long += step) > longMax) {
			longMax = next_long;
		}
		decreaseStep();
	} while (step > 0);

	next_long = 0;
	step = 1;
	do {
		longMin = next_long;
		next_long -= step;
		increaseStep();
	} while (next_long < longMin);
	do {
		next_long = longMin;
		while ((next_long -= step) < longMin) {
			longMin = next_long;
		}
		decreaseStep();
	} while (step > 0);


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
/* Exercise 3-1.
	Our binary search makes two tests inside the loops, when
	one would suffice (at the price of more tests outside).
	Write a version with only one test inside the loop and
	measure the difference in run-time.
*/

#include <stdio.h>

int binsearch1(int x, int v[], int len) {
	int low, high, mid;

	low = 0;
	high = len-1;
	while (low <= high) {
		mid = (low + high) / 2;
		if (x < v[mid])
			high = mid-1;
		else if (x > v[mid])
			low = mid+1;
		else /* found match */
			return mid;
	}
	return -1; /* no match */
}

int binsearch2(int x, int v[], int len) {
	int low, high, mid;

	low = 0;
	high = len-1;
	while (low <= high) {
		mid = (low + high) / 2;
		if (x < v[mid])
			high = mid-1;
		else
			low = mid+1;
	}
	if (v[mid] == x) { return mid; } /* ended on match */
	return -1; /* no match */
}



int main() {
	int list[12] = { 0, 4, 5, 6, 12, 152, 156, 350, 643, 690, 800, 2000 };

	printf("start\n");
	// I don't know the right way to measure time in C yet, so I'll
	// just do enough of them to see the difference.

	// ~ 26 seconds
	printf("binsearch1...\n");
	for (int i=0; i < 1000000000; i++) {
		binsearch1(i % 10000, list, 12);
	}

	// ~ 31 seconds
	printf("binsearch2...\n");
	for (int i=0; i < 1000000000; i++) {
		binsearch2(i % 10000, list, 12);
	}
	printf("end\n");

	return 0;
}
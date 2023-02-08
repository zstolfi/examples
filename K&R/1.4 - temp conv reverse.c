#include <stdio.h>

/* print Farenheit-Celsius table
    for fahr = 0, 20, ..., 300; floating-point version*/
main() {
	float fahr, celsius;
	int lower, upper, step;

	lower = -20;	/* lower limit of temperature table */
	upper = 160;	/* upper limit */
	step  = 10; 	/* step size */

	printf("  C  -  F \n\n");

	celsius = lower;
	while (celsius <= upper) {
		fahr = (9.0/5.0) * celsius + 32.0;
		printf("%3.0f %6.1f\n", celsius, fahr);
		celsius = celsius + step;
	}
}
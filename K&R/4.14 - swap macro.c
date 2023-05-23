/* Exercise 4-14.
	Define a meacro swap(t,x,y) that interchamges two arguments
	of type t. (Block structure will help.)
*/

#define swap(T,x,y) { \
	T temp = x; \
	x = y; \
	y = temp; \
}

#include <stdio.h>

int main(void) {
	int a=3, b=5;
	float x=1.41, y=-5.2;

	printf("a == %d, b == %d\n", a, b);

	/* Even though the semicolon is not required
	   I still think it looks nicest to have it. */
	swap(int, a, b);

	printf("a == %d, b == %d\n", a, b);

	printf("x == %g, y == %g\n", x, y);
	swap(float, x, y);
	printf("x == %g, y == %g\n", x, y);

	return 0;
}
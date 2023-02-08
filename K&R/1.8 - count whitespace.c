/* Exercise 1-8.
	Write a program to count blanks, tabs, and newlines.
*/

#include <stdio.h>

main() {
	int numBlanks = 0, numTabs = 0, numNewlines = 0;
	for (int c=0; (c = getchar()) != EOF; ) {
		switch (c) {
		case ' ' : ++numBlanks;   break;
		case '\t': ++numTabs;     break;
		case '\n': ++numNewlines; break;
		}
	}

	printf("blanks:  \t%d\n"
	       "tabs:    \t%d\n"
	       "newlines:\t%d\n" ,
	       numBlanks, numTabs, numNewlines);
}
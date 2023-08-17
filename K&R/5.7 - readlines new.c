/* Exercise 5-7.
	Rewrite readlines to store lines in an array supplied by
	main, rather than calling alloc to maintain storage. How
	much faster is the program?
*/

#include <stdio.h>
#include <string.h>
#define MAXLINES 1000

char *lineptr[MAXLINES];

int  readlines (char *lineptr[], int nlines);
void writelines(char *lineptr[], int nlines);
void qsort(char *lineptr[], int left, int right);

/* sort input lines */
int main(void) {
	int nlines;
	if ((nlines = readlines(lineptr, MAXLINES)) >= 0) {
		qsort(lineptr, 0, nlines-1);
		writelines(lineptr, nlines);
		return 0;
	} else {
		printf("error: input too big to sort\n");
		return 1;
	}
}



/* qsort algorithm */
void qsort(char *v[], int left, int right) {
	int last;
	void swap(char *v[], int i, int j);

	if (left >= right) return;
	swap(v, left, (left + right)/2);
	last = left;
	for (int i = left+1; i <= right; i++)
		if (strcmp(v[i], v[left]) < 0)
			swap(v, ++last, i);
	swap(v, left, last);
	qsort(v, left, last-1);
	qsort(v, last+1, right);
}

void swap(char *v[], int i, int j) {
	char *temp = v[i];
	v[i] = v[j];
	v[j] = temp;
}



/* io functions */
#define MAXLEN 1000
int getline(char *, int);
char *alloc(int);

int readlines(char *lineptr[], int maxlines) {
	int len, nlines;
	char *p, line[MAXLEN];

	nlines = 0;
	while ((len = getline(line, MAXLEN)) > 0) {
		if (nlines >= maxlines || (p = alloc(len+1)) == NULL) {
			return -1;
		} else {
			/* no need to delete a newline if we   
			   don't have it in the first place. */
			strcpy(p, line);
			lineptr[nlines++] = p;
		}
	}
	return nlines;
}

void writelines(char *lineptr[], int nlines) {
	while (nlines-- > 0)
		printf("%s\n", *lineptr++);
}



/* alloc functions */
#define ALLOCSIZE 10000

static char allocbuf[ALLOCSIZE];
static char *allocp = allocbuf;

char *alloc(int n) {
	if (allocbuf + ALLOCSIZE - allocp >= n) { /* it fits */
		allocp += n;
		return allocp - n; /* old p */
	} else { /* it doesn't fit */
		return NULL;
	}
}

void afree(char *p) {
	if (p >= allocbuf && p < allocbuf + ALLOCSIZE)
		allocp = p;
}



/* modified getline function */
int getline(char s[], int lim) {
	int c, i=0;
	while (--lim > 0 && (c=getchar()) != EOF && c != '\n')
		s[i++] = c;
	s[i] = '\0';
	return i;
}
/* Exercise 4-3.
	Given the basic framework, it's straightforward to extend
	the calculator. Add the modulus (%) operator and provisions
	for negative numbers.
*/

#include <stdio.h>
#include <stdlib.h>    /* for atof() */

#define MAXOP 100     /* max size of operand or operator */
enum optype {         /* signal for operator/operand type */
	NUMBER = '0', ADD = '+', MUL = '*', SUB = '-', DIV = '/', MOD = '%' ,
	PRINT = '\n'
};

int getop(char []);
void push(double);
double pop(void);

/* reverse Polish calculator */
int main(void) {
	int type;
	double op2;
	char s[MAXOP];

	while ((type = getop(s)) != EOF) {
		switch (type) {
		case NUMBER:
			push(atof(s));
			break;
		case ADD:
			push(pop() + pop());
			break;
		case MUL:
			push(pop() * pop());
			break;
		case SUB:
			op2 = pop();
			push(pop() - op2);
			break;
		case DIV:
			op2 = pop();
			if (op2 != 0.0)
				push(pop() / op2);
			else
				printf("error: zero divisor\n");
			break;
		case MOD:
			op2 = pop();
			if (op2 > 0.0)
				push((int)pop() % (int)op2);
			else
				printf("error: invalid modulus\n");
			break;
		case PRINT:
			printf("\t%.8g\n", pop());
			break;
		default:
			printf("error: unknown command %s\n", s);
		}
	}
	return 0;
}



#define MAXSTACK 100    /* maximum depth of val stack */

int sp = 0;              /* next free stack position */
double val[MAXSTACK];    /* value stack */

/* push:  push f onto value stack */
void push(double f) {
	if (sp < MAXSTACK)
		val[sp++] = f;
	else
		printf("error: stack full, can't push %g\n", f);
}

/* pop:  pop and return top value from stack */
double pop(void) {
	if (sp > 0)
		return val[--sp];
	else {
		printf("error: stack empty\n");
		return 0.0;
	}
}



#include <ctype.h>
#include <stdbool.h>

int getch(void);
void ungetch(int);

/* getop:  get next operator or next numeric operand */
int getop(char s[]) {
	int i, c;

	while (s[0] = c = getch(), c == ' ' || c == '\t')
		;

	s[1] = '\0';
	if (!isdigit(c) && c != '.' && c != '-')
		return c;    /* not a number */
	i = 0;

	if (c == '-') {
		if (c = getch(), !isdigit(c)) {
			/* I'm unget-ing the '\n' but the next loop   
			   doesn't pick up on it for some reason... */
			ungetch(c);
			return s[0];    /* unary minus */
		} else {
			s[++i] = c;
		}
	}
	if (isdigit(c))    /* collect integer part */
		while (s[++i] = c = getch(), isdigit(c))
			;
	s[i] = '\0';
	if (c != EOF)
		ungetch(c);
	return NUMBER;
}



/* apparently I was over-clever in trying to make my own ungetch */
#if 0
char ungetchar;    /* only 1 char needed anyway */
bool heldchar = false;

int getch(void) { /* get a (possibly pushed back) character */
	heldchar = false;
	return (heldchar) ? ungetchar : getchar();
}

void ungetch(int c) { /* push character back on input */
	if (heldchar)
		printf("ungetch: too many characters\n");
	else {
		heldchar = true;
		ungetchar = c;
	}
}
#else
#define BUFSIZE 100

char buf[BUFSIZE];
int bufp = 0;

int getch(void) {
	return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c) {
	if (bufp >= BUFSIZE)
		printf("ungetch: too many characters\n");
	else
		buf[bufp++] = c;
}
#endif
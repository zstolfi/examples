/* Exercise 4-5.
	Add access to library functions like sin, exp, and pow. See
	<math.h> in Appendix B, Section 4.
*/

#include <stdio.h>
#include <stdlib.h>    /* for atof() */
#include <math.h>

#define MAXOP 100     /* max size of operand or operator */
enum optype {         /* signal for operator/operand type */
	NUMBER = '0', ADD = '+', MUL = '*', SUB = '-', DIV = '/', POW = '^' ,
	SIN = 's', COS = 'c', EXP = 'e' ,
	PRINT = '\n'
};

int getop(char []);
void push(double);
double pop(void);

/* reverse Polish calculator */
int main(void) {
	int type;
	double op1, op2;
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
		case POW:
			op2 = pop();
			op1 = pop();
			if (!(op1 == 0.0 && op2 <= 0)
			&&  !(op1 < 0.0 && floor(op2) != op2))
				push(pow(op1, op2));
			else
				printf("error: invalid domain\n");
			break;
		case SIN:
			push(sin(pop()));
			break;
		case COS:
			push(cos(pop()));
			break;
		case EXP:
			push(exp(pop()));
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
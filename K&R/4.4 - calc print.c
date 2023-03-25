/* Exercise 4-4.
	Add commands to print the top element of the stack without
	popping, to duplicate it, and to swap the top two elements.
	Add a command to clear the stack.
*/

#include <stdio.h>
#include <stdlib.h>    /* for atof() */

#define MAXOP 100     /* max size of operand or operator */
enum optype {         /* signal for operator/operand type */
	NUMBER = '0', ADD = '+', MUL = '*', SUB = '-', DIV = '/',
	RESULT = '\n',
	PRINT_FULL = 'f', PRINT_TOP = 'p', DUPLICATE_TOP = 'd',
	SWAP_TOP = 's', CLEAR = 'c'
};

#define MAXSTACK 100     /* maximum depth of val stack */
int sp = 0;              /* next free stack position */
double val[MAXSTACK];    /* value stack */

int getop(char []);
void push(double);
double pop(void);

/* reverse Polish calculator */
int main(void) {
	int type, i;
	double op1, op2;
	char s[MAXOP];

	while ((type = getop(s)) != EOF) {
		switch (type) {
		/* arithmetic */
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
		/* output */
		case RESULT:
			if (sp > 1)
				printf("\t%.8g\n", pop());
			else
				printf("stack empty\n");
			break;
		case PRINT_FULL:
			for (i=0; i < sp; i++)
				printf("\t%.8g", val[i]);
			printf(" %d\n", sp);
			break;
		case PRINT_TOP:
			printf("\t%.8g\n", val[sp-1]);
			break;
		case DUPLICATE_TOP:
			push(val[sp-1]);
			break;
		case SWAP_TOP:
			op2=pop(); op1=pop();
			push(op2); push(op1);
			break;
		case CLEAR:
			for (; sp >= 1; sp--)
				val[sp-1] = 0.0;
			break;
		default:
			printf("error: unknown command %s\n", s);
			break;
		}
	}
	return 0;
}


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
	if (!isdigit(c) && c != '.')
		return c;    /* not a number */
	i = 0;
	if (isdigit(c))    /* collect integer part */
		while (s[++i] = c = getch(), isdigit(c))
			;
	s[i] = '\0';
	if (c != EOF)
		ungetch(c);
	return NUMBER;
}



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
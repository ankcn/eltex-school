// Меню телефонного справочника

#include <stdio.h>
#include <stdlib.h>
#include "menu.h"



int menu_title()
{
	printf("\nSelect operation on two complex numbers:\n"
		"1 - Addition\n"
		"2 - Subtraction\n"
		"3 - Multiplication\n"
		"4 - Division\n"
		"q - quit\n"
	);

	int c;
	while ((c = getchar()) == '\n');
	getchar(); // Remove new line from stdin buffer
	return c;
}


float enter_value(const char* req)
{
	printf("Please, enter %s: ", req);
	char buf[NUMBER_LENGTH + 1];
	fgets(buf, NUMBER_LENGTH, stdin);
	return atof(buf);
}


int menu_process(const char c)
{
	if (c < '1' || c > '4') {
		printf(" Unrecognized command\n");
		return -1;
	}

	complex a, b ,r;
	printf("First complex aperand\n");
	a = enter_complex();
	printf("Second complex aperand\n");
	b = enter_complex();

	printf("(");
	compl_print(a);

	switch (c) {
		case '1': printf(") + ("); r = compl_plus(a, b); break;
		case '2': printf(") - ("); r = compl_minus(a, b); break;
		case '3': printf(") * ("); r = compl_mult(a, b); break;
		case '4': printf(") / ("); r = compl_divide(a, b); break;
	}

	compl_print(b);
	printf(") = ");
	compl_print(r);
	printf("\n");

	return 0;
}


complex enter_complex()
{
	complex t;
	t.re = enter_value("Re");
	t.im = enter_value("Im");
	return t;
}


// Меню калькулятора

#include <stdio.h>
#include <stdlib.h>
#include "menu.h"
#include "modules.h"


extern int nmod; // Объявлена в main.c


int menu_title()
{
	printf("\nSelect operation on two complex numbers:\n");
	// Перебираем имеющиеся модули и выводим названия операций, которые они предоставляют
	for (int i = 0; i < nmod; ++i)
		printf("%d - %s\n", i + 1, ops[i].op_name);
	printf("q - quit\n");
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
	int n = c - '1';
	// Проверяем, корректная ли команда получена
	if (n < 0 || n >= nmod) {
		printf(" Unrecognized command\n");
		return -1;
	}

	complex a, b ,r;
	// Запрос у пользователя на ввод операндов
	printf("First complex aperand\n");
	a = enter_complex();
	printf("Second complex aperand\n");
	b = enter_complex();

	// Печатаем выражение, вычисляем и печатаем результат
	printf("(");
	print_complex(a);
	printf(") %c (", ops[n].op_symbol);
	r = ops[n].op_func(a, b);
	print_complex(b);
	printf(") = ");
	print_complex(r);
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


void print_complex(const complex a)
{
	printf("%.2f", a.re);
	float t = a.im;
	if (t < 0)
		t = -a.im;
	if (t)
		printf(" %c j%.2f", (a.im < 0) ? '-' : '+', t);
}


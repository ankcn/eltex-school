#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "error.h"


/*
 * print_and_quit - Вывод сообщения и аварийный выход из программы
 * @msg: сообщение для вывода в консоль
 */
void print_and_quit(const char* msg)
{
	puts(msg);
	exit(EXIT_FAILURE);
}


#include <stdio.h>
#include <string.h>
#include "noecho.h"
#include "common.h"


int main(int argc, char** argv)
{
	// Переключение режима терминала в специальный режим с отключенным эхо и невидимым курсором
	echo_custom();
	hide_cursor();

	// Проверяем аргумент командной строки
	if (argc == 2)
		chating(argv[1]);
	else
		puts("Usage: \"chat server\" for server mode or \"chat _name_\" for client");

	// Восстановление нормального режима терминала
	echo_normal();
	show_cursor();
	return 0;
}

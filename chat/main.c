#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "noecho.h"


// Объявления внешних функций
void start_server();
void reg_on_server();
void chating();


int main(int argc, char** argv)
{
	// Переключение режима терминала в специальный режим с отключенным эхо и невидимым курсором
	echo_custom();
	hide_cursor();

	// Проверяем аргумент командной строки
	if (argc == 2) {
		// Если получено "server", то запускаем сервер
		if (! strcmp(argv[1], "server")) {
			puts("Starting server\nPress q for quit\n");
			start_server();
		// В противном случае принимаем аргумент как псевдоним и запускаем клиента
		} else {
			reg_on_server(argv[1]);
			chating();
		}
	} else
		puts("Usage: \"chat server\" for server mode or \"chat _name_\" for client");

	// Восстановление нормального режима терминала
	echo_normal();
	show_cursor();
	return 0;
}

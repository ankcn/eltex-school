#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include "error.h"


// Объявления внешних функций
extern void echo_server(int proto);
extern void echo_client(int proto, const char* host, const char* msg);


int main(int argc, char** argv)
{
	// Проверка количества переданных аргументов и вывод шаблона использования программы
	if (argc < 3) {
		puts("Usage: necho [-s] -t|u [host] [payload]");
		puts("\t-s : server mode\n\t-t : TCP transport\n\t-u : UDP transport\n\thost : server address\n\tpayload : data for transmit");
		return 1;
	}

	// Признак режима сервера
	int server = 0;

	// Транспортный протокол
	int mode = 0;

	// Перебор аргументов, поиск известных ключей
	for (int i = 1; i < argc; ++i) {
		if (! strcmp(argv[i], "-s"))
			server = 1;
		else if (! strcmp(argv[i], "-t"))
			mode = SOCK_STREAM;
		else if (! strcmp(argv[i], "-u"))
			mode = SOCK_DGRAM;
	}

	if (! mode)
		print_and_quit("Incorrect transport options");

	// Если задан режим сервера, то запускаем сервер
	if (server) {
		printf("Starting server in ");
		if (mode == SOCK_STREAM)
			puts("TCP mode");
		else
			puts("UDP mode");
		echo_server(mode);
	}
	// Иначе запускаем клиента
	else {
		char* msg = "Hello!";
		if (argc > 3)
			msg = argv[3];
		puts("Connecting to server");
		echo_client(mode, argv[2], msg);
	}

	return 0;
}

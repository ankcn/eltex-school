#include "common.h"

/*
 * echo_client - Клиент сетевого эхо
 * @proto: тип сокета, определяющий транспортный протокол (TCP/UDP)
 * @host: строка с IP адресом сервера
 * @msg: строка для отправки на сервер
 * @sock_fd: файловый дескриптор открытого сокета
 * @address: структура IP адреса сервера
 * @buf: буфер для получения данных от сервера
 * Отправка запроса на эхо-сервер и получение ответа
 */

void echo_client(int proto, const char* host, const char* msg)
{
	// Создание сокета
	int sock_fd = socket(AF_INET, proto, 0);
	if (sock_fd < 0)
		print_and_quit("Socket creation failure");

	// Подготовка структуры адреса сервера
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(SOCKET_PORT);

	// Преобразование адреса сервера из текстовой строки в сетевой формат
	if (inet_pton(AF_INET, host, &address.sin_addr) <= 0)
		print_and_quit("Server IP address conversion failed");

	// Подключение к серверу
	if (connect(sock_fd, (struct sockaddr*) &address, sizeof(address)) < 0)
		print_and_quit("Connection error");

	// Отправка сообщения на сервер
	write(sock_fd, msg, strlen(msg) + 1);

	// Получение ответа сервера
	char buf[BUF_SIZE];
	if (read(sock_fd, buf, BUF_SIZE) > 0)
		printf("Server answer: \n%s\n", buf);
}


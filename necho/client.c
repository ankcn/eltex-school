#include "common.h"
#include <netdb.h>


/*
 * resolve_name - Получение IP адреса из доменного имени
 * @name: строка с доменным именем
 * @address: структура адреса, в которую помещается результат
 * @answer: список структур addrinfo на выходе getaddrinfo
 * @hints: подсказки для getaddrinfo
 */
int resolve_name(const char* name, struct sockaddr_in* address)
{
	struct addrinfo* answer;
	struct addrinfo hints;

	hints.ai_family = AF_INET;
	hints.ai_next = NULL;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_socktype = 0;
	hints.ai_flags = 0;

	// Получаем список адресов для заданного имени
	if (getaddrinfo(name, NULL, &hints, &answer))
		return 1;

	// Копируем первый адрес из списка
	*((struct sockaddr*) address) = *answer[0].ai_addr;

	freeaddrinfo(answer);
	return 0;
}

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

	// Преобразование IP адреса сервера из строки с именем
	struct sockaddr_in address;
	if (resolve_name(host, &address)) {
		printf("Host %s: ", host);
		print_and_quit("address resolution failed");
	}
	address.sin_port = htons(SOCKET_PORT);

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


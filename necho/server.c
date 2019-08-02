#include "common.h"


// Заголовок для ответа клиенту
#define CAPTION	"Recieved: "

/*
 * echo_server - Сервер сетевого эхо
 * @proto: тип сокета, определяющий транспортный протокол (TCP/UDP)
 * @sock_fd: файловый дескриптор открытого сокета
 * @address: структура локального IP адреса
 * @buf: буфер для обмена данными с клиентом
 * @claddr: структура IP адреса клиента
 * @conn_fd: дескриптор сокета входящего подключения
 * @caplen: длинна заголовка CAPTION
 * @addr_str: строка под текстовое представление IP адреса клиента
 * @n: количество принятых байт
 * Открытие сокета, обработка входящих подключений и отправка ответа
 */
void echo_server(int proto)
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
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	// Привязка сокета к IP адресу
	if (bind(sock_fd, (struct sockaddr*) &address, sizeof(address)) < 0)
		print_and_quit("Can not bind IP address");

	// Для режма TCP разрешаем приём входящих подключений
	if ((proto == SOCK_STREAM) && listen(sock_fd, MAX_CONNECTIONS) < 0)
		print_and_quit("Can not listen socket");

	for (;;) {
		struct sockaddr_in claddr;
		socklen_t alen = sizeof(claddr);
		int conn_fd = 0;
		char buf[BUF_SIZE] = CAPTION;
		size_t caplen = strlen(CAPTION);
		int n = 0;

		// Принимаем входящее подключение и считывем полученные данные
		if (proto == SOCK_STREAM) {
			conn_fd = accept(sock_fd, (struct sockaddr*) &claddr, &alen);
			if (conn_fd < 0)
				print_and_quit("Error while accepting connection");
			n = read(conn_fd, buf + caplen, BUF_SIZE - caplen);
		} else
			n = recvfrom(sock_fd, buf + caplen, BUF_SIZE - caplen, 0, (struct sockaddr*) &claddr, &alen);

		// Создаём новый процесс для обоработки подключения
		pid_t pid = fork();
		if (pid < 0)
			print_and_quit("Error while forking");

		// Проверяем, если это дочерний процесс, то выполняем обработку запроса клиента
		else if (! pid) {

			// Выясняем IP адрес подключенного клиента
			char addr_str[INET_ADDRSTRLEN];
			if (inet_ntop(AF_INET, &claddr.sin_addr, addr_str, INET_ADDRSTRLEN) != NULL)
				printf("Connection from %s\n", addr_str);

			// В ответ отправляем клиенту то же самое, что получили от него, предваряя заголовком CAPTION
			if (n > 0) {
				puts(buf);
				if (proto == SOCK_STREAM)
					write(conn_fd, buf, n + caplen);
				else
					sendto(sock_fd, buf, n + caplen, 0, (struct sockaddr*) &claddr, alen);
			} else
				puts("No data received");

			// Закрываем открытый сокет
			if (conn_fd)
				close(conn_fd);

			// Завершаем дочерний процесс
			exit(0);
		}


	}
}


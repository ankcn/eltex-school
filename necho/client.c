#include "common.h"
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <errno.h>


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
 * @uh: заголовок UDP датаграммы
 * Отправка запроса на эхо-сервер и получение ответа
 */
void echo_client(int proto, const char* host, const char* msg)
{
	int msg_size = strlen(msg) + 1;
	char buf[BUF_SIZE];
	memset(buf, 0, BUF_SIZE);

	// Создание сокета
	int sock_fd = socket(AF_INET, proto, (proto == SOCK_RAW) ? IPPROTO_RAW : IPPROTO_IP);
	if (sock_fd < 0)
		print_and_quit("Socket creation failure. Must be root for RAW sockets.");

	// Преобразование IP адреса сервера из строки с именем
	struct sockaddr_in address;
	if (resolve_name(host, &address)) {
		printf("Host %s: ", host);
		print_and_quit("address resolution failed");
	}
	address.sin_port = htons(SOCKET_PORT);

	// В случае режима RAW сокетов выполняем особый порядок действий
	if (proto == SOCK_RAW) {

		// Заполняем структуру заголовка IP и размещаем его в начале буфера
		struct ip* iph = (struct ip*) buf;
		iph->ip_p = IPPROTO_UDP;
		iph->ip_hl = 5;
		iph->ip_v = 4;
		iph->ip_ttl = 64;
		iph->ip_id = htons(12345);
		iph->ip_dst = address.sin_addr;

		// Заполняем структуру заголовка UDP и размещаем его после заголовка IP 
		struct udphdr* uh = (struct udphdr*) (buf + IP_H_SIZE);
		uh->uh_dport = address.sin_port;
		uh->uh_sport = htons(SOURCE_PORT);
		uh->uh_ulen = htons(UDP_H_SIZE + msg_size);

		// Копируем полученную от пользователя строку в буфер со смещением на размер заголовкov
		strcpy((char *) (buf + IP_AND_UDP_H_SIZE), msg);

		// Создание отдельного сокета для получения ответа от сервера (поскольку сокет с протоколом IPPROTO_RAW не пригоден для чтения)
		int ro_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
		if (ro_fd < 0)
			print_and_quit("Recieve socket creation failure.");

		// Описываем локальный IP адрес, на котором будем слушать ответ от сервера
		struct sockaddr_in loc_addr;
		loc_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		loc_addr.sin_port = htons(SOURCE_PORT);
		loc_addr.sin_family = AF_INET;

		// Цепляем сокет чтения к локальному адресу
		bind(ro_fd, (struct sockaddr*) &loc_addr, sizeof(loc_addr));

		// Отправляем сообщение серверу
		if (sendto(sock_fd, buf, IP_AND_UDP_H_SIZE + msg_size, 0, (struct sockaddr*) &address, sizeof(address)) < 0)
			print_and_quit("Error at sendto()");

		// Считываем ответ
		read(ro_fd, buf, BUF_SIZE);
		printf("Server answer: \n%s\n", buf);
	}

	// В режимах TCP и UDP
	else {
		// Подключение к серверу
		if (connect(sock_fd, (struct sockaddr*) &address, sizeof(address)) < 0)
			print_and_quit("Connection error");

		// Отправка сообщения на сервер
		write(sock_fd, msg, msg_size);

		// Получение ответа сервера
		if (read(sock_fd, buf, BUF_SIZE) > 0)
			printf("Server answer: \n%s\n", buf);
	}

	shutdown(sock_fd, SHUT_RDWR);
	close(sock_fd);
}


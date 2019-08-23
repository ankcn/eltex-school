#include <pthread.h>
#include "common.h"
#include "server.h"


// Переменные выведены в глабальную область для возможности доступа из других потоков

// Файловый дескриптор сокета
int sock_fd;

// Протокол: TCP или UDP
int proto;


/*
 * echo_server - Сервер сетевого эхо
 * @mode: тип сокета, определяющий транспортный протокол (TCP/UDP)
 * @address: структура локального IP адреса
 * Открытие сокета, принятие входящих подключений, запуск потоков обработки запросов
 */
void echo_server(int mode)
{
	proto = mode;

	// Создание сокета
	sock_fd = socket(AF_INET, proto, 0);
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

	// Инициализация массива обработчиков клиентских запросов
	conn_info_t workers[THREADS_NUM];
	for (int i = 0; i < THREADS_NUM; ++i) {
		pthread_mutex_init(&workers[i].mtx, NULL);
		pthread_cond_init(&workers[i].cnd, NULL);
		workers[i].id = i;
		pthread_create(&workers[i].thread, NULL, request_handler, (void *) &workers[i]);
		pthread_detach(workers[i].thread);
	}

	// Бесконечный цикл; на каждой итерации (для нового подключения) берём следующего обработчика
	for (int k = 0; ; ++k) {
		if (k >= THREADS_NUM)
			k = 0;

		// Перебираем обработчиков, пока не найдём свободного
		while (pthread_mutex_trylock(&workers[k].mtx))
			if (++k >= THREADS_NUM)
				k = 0;

		socklen_t alen = sizeof(struct sockaddr_in);

		// Принимаем входящее подключение и считывем полученные данные
		if (proto == SOCK_STREAM) {
			workers[k].conn_fd = accept(sock_fd, (struct sockaddr*) &workers[k].claddr, &alen);
			if (workers[k].conn_fd < 0)
				print_and_quit("Error while accepting connection");
			workers[k].bytes = read(workers[k].conn_fd, workers[k].buf, BUF_SIZE);
		} else
			workers[k].bytes = recvfrom(sock_fd, workers[k].buf, BUF_SIZE, 0, (struct sockaddr*) &workers[k].claddr, &alen);

		// Разблокируем мьютекс обработчика и сигналим ему, что можно приступать
		pthread_mutex_unlock(&workers[k].mtx);
		pthread_cond_signal(&workers[k].cnd);
	}

	// TODO: move to... better place
	// Закрытие потоков. Здесь никогда не выполняется.
	for (int i = 0; i < THREADS_NUM; ++i) {
		pthread_cancel(workers[i].thread);
		pthread_cond_destroy(&workers[i].cnd);
		pthread_mutex_destroy(&workers[i].mtx);
	}
}


/*
 * request_handler - Обработчик запроса клиента
 * @par: аргумент, под которым скрывается указатель на тип  conn_info_t
 */
void* request_handler(void* par)
{
	conn_info_t* ci = (conn_info_t*) par;
	printf("Starting worker #%d\n", ci->id);

	for (;;) {
		// Ждём сигнала о получении запроса
		while (! ci->bytes)
			pthread_cond_wait(&ci->cnd, &ci->mtx);

		// Выясняем IP адрес подключенного клиента
		char addr_str[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &ci->claddr.sin_addr, addr_str, INET_ADDRSTRLEN) != NULL)
			printf("Connection from %s handled by worker #%d\nGOT: %s\n", addr_str, ci->id, ci->buf);

		// В ответ отправляем клиенту то же самое, что получили от него, предваряя заголовком
		char cap[CAP_MAX_LEN];
		sprintf(cap, "Answer from worker #%d: ", ci->id);
		send_to_client(ci, cap);
		send_to_client(ci, ci->buf);

		// Закрываем открытый сокет
		if (ci->conn_fd)
			close(ci->conn_fd);

		ci->bytes = 0;
	}

	return par;
}


/*
 * send_to_client - Отправка данных клиенту
 * @ci: указатель на структуру данных обработчика запросов
 * @data: данные для передачи клиенту
 */
void send_to_client(const conn_info_t* ci, const char* data)
{
	if (proto == SOCK_STREAM)
		write(ci->conn_fd, data, strlen(data));
	else
		sendto(sock_fd, data, strlen(data), 0, (struct sockaddr*) &ci->claddr, sizeof(ci->claddr));
}


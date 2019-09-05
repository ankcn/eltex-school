#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include "common.h"
#include "server.h"


// Переменные выведены в глабальную область для возможности доступа из других потоков

// Файловый дескриптор сокета
static int sock_fd;

// Протокол: TCP или UDP
static int proto;

// Флаг для прерывания работы программы по сигналу
static volatile int no_cancel = 1;

// Очередь запросов
static req_queue_t que;


/*
 * echo_server - Сервер сетевого эхо
 * @mode: тип сокета, определяющий транспортный протокол (TCP/UDP)
 * @address: структура локального IP адреса
 * Открытие сокета, принятие входящих подключений, запуск потоков обработки запросов
 */
void echo_server(int mode)
{
	proto = mode;

	// Регистрируем обработчика сигнала прерывания
	signal(SIGINT, sig_int_hnd);

	// Создание сокета
	sock_fd = socket(AF_INET, proto, 0);
	if (sock_fd < 0)
		print_and_quit("Socket creation failure");

	// Подготовка структуры адреса сервера
	struct sockaddr_in address;
	init_address(&address);

	// Привязка сокета к IP адресу
	if (bind(sock_fd, (struct sockaddr*) &address, sizeof(address)) < 0)
		print_and_quit("Can not bind IP address");

	// Для режма TCP разрешаем приём входящих подключений
	if ((proto == SOCK_STREAM) && listen(sock_fd, MAX_CONNECTIONS) < 0)
		print_and_quit("Can not listen socket");

	// Инициализация массива обработчиков клиентских запросов
	pthread_t workers[THREADS_NUM];
	for (long i = 0; i < THREADS_NUM; ++i)
		pthread_create(&workers[i], NULL, request_handler, (void *) i);

	// Инициализация очереди
	init_queue();

	// Работаем, пока обработчик сигнала не обнулил флаг no_cancel
	while (no_cancel) {
		socklen_t alen = sizeof(struct sockaddr_in);

		// Продвигаемся по очереди в поисках свободного слота
		while (que.jobs[inc_index(&que.next_add)].state != ST_FREE && no_cancel);
		conn_info_t* ci = &que.jobs[que.next_add];

		// Принимаем входящее подключение и считывем полученные данные
		if (proto == SOCK_STREAM) {
			ci->conn_fd = accept(sock_fd, (struct sockaddr*) &ci->claddr, &alen);
			if (ci->conn_fd < 0) {
				if (no_cancel)
					puts("Error while accepting connection");
				else
					puts("\nInterrupted");
				continue;
			}
			ci->bytes = read(ci->conn_fd, ci->buf, BUF_SIZE);
		} else
			ci->bytes = recvfrom(sock_fd, ci->buf, BUF_SIZE, 0, (struct sockaddr*) &ci->claddr, &alen);

		// Сообщаем потокам-обработчикам, что для них есть работа
		ci->state = ST_WAIT;
		sem_post(&que.sem);
	}

	// Ждём завершения всех потоков
	for (int i = 0; i < THREADS_NUM; ++i)
		pthread_join(workers[i], NULL);

	pthread_mutex_destroy(&que.mtx);
	sem_destroy(&que.sem);
	puts("Closing server");
}


/*
 * request_handler - Обработчик запроса клиента
 * @par: аргумент, через который передаётся порядковый номер обработчика
 */
void* request_handler(void* par)
{
/*
Вероятно, интерпретация указателя как целого и наоборот - не очень хорошая практика,
но вариант с указателем на i (локальную переменную цикла for) не сработал (получался мусор):
par = (void*) &i;
int id = *((int*) par);
*/
	int id = (long) par;
	printf("Starting worker #%d\n", id);

	while (no_cancel) {
		// Ждём сигнала о получении запроса
		sem_wait(&que.sem);

		// Возможно, получен SIGINT, тогда бросаем это дело
		if (! no_cancel)
			break;

		// Монополизируем доступ с помощью мьютекса
		pthread_mutex_lock(&que.mtx);

		// Ищем в очереди следующую задачу, ожидающую обработки
		int k = 0;
		while (que.jobs[inc_index(&que.next_pop)].state != ST_WAIT && k++ < QUEUE_SIZE);

		// Ярлык для более удобного обращения к данным запроса
		conn_info_t* ci = &que.jobs[que.next_pop];

		// Если нашли, что искали, то помечаем задачу, как обрабатываемую
		if (k < QUEUE_SIZE)
			ci->state = ST_WORKING;

		// Открываем доступ к очереди другим потокам
		pthread_mutex_unlock(&que.mtx);

		// Если во всей очереди не нашлось новой задачи, то ждём нового сигнала
		if (k == QUEUE_SIZE || ! ci->bytes)
			continue;

		// Выясняем IP адрес подключенного клиента
		char addr_str[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &ci->claddr.sin_addr, addr_str, INET_ADDRSTRLEN) != NULL)
			printf("Connection from %s handled by worker #%d\nGOT: %s\n", addr_str, id, ci->buf);

		// В ответ отправляем клиенту то же самое, что получили от него, предваряя заголовком
		char cap[CAP_MAX_LEN];
		sprintf(cap, "Answer from worker #%d, queue slot #%d: ", id, que.next_pop);
		send_to_client(ci, cap);
		send_to_client(ci, ci->buf);

		// Закрываем открытый сокет
		if (ci->conn_fd)
			close(ci->conn_fd);

		// Освобождаем слот в очереди
		ci->bytes = 0;
		ci->state = ST_FREE;
	}

	printf("Stoping worker #%d\n", id);
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

/*
 * sig_int_hnd - Обработчик сигнала прерывания
 * Цель в том, чтобы привести к корректному завршению программы
 */
void sig_int_hnd(int x) {
	// Обнуляем флаг, тем самым, прекращая работу основных циклов
	no_cancel = 0;

	// С помощью семафора разблокируем потоки, которые вскоре завершаются
	for (int i = 0; i < THREADS_NUM; ++i)
		sem_post(&que.sem);
}

/*
 * init_address - Инициализация IP адреса сокета
 * @addr: указатель на структуру sockaddr_in
 */
void init_address(struct sockaddr_in* addr)
{
	memset(addr, 0, sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(SOCKET_PORT);
	addr->sin_addr.s_addr = htonl(INADDR_ANY);
}

/*
 * inc_index - Циклический инкремент индекса очереди
 * @id: указатель на переменную индекса
 */
int inc_index(int* id)
{
	*id = (*id + 1) % QUEUE_SIZE;
	return *id;
}

/*
 * init_queue - Инициализация очереди запросов
 */
void init_queue()
{
	que.next_add = 0;
	que.next_pop = QUEUE_SIZE - 1;
	sem_init(&que.sem, 0, 0);
	pthread_mutex_init(&que.mtx, NULL);

	for (int i = 0; i < THREADS_NUM; ++i)
		que.jobs[i].state = ST_FREE;
}


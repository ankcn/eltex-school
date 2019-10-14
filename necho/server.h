#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED


// Допустимое количество подключений к сокету
#define MAX_CONNECTIONS	11

// Количество потоков для обработки запросов клиентов
#define THREADS_NUM	3

// Размер очереди обращений
#define QUEUE_SIZE	5

// Количество прослушиваемых сокетов
#define SOCKETS_NUM	2


// Тип состояний слота очереди: свободен, ожидает обработки, обрабатывается
typedef enum { ST_FREE, ST_WAIT, ST_WORKING } status_t;


// Тип структуры с информацией о клиентском запросе
typedef struct {
	size_t bytes;	// Количество байт, принятых от клиента
	int conn_fd;	// Дескриптор соединения
	struct sockaddr_in claddr;	// Адрес клиента
	char buf[BUF_SIZE];	// Буфер для получения данных от клиента
	status_t state;	// Состояние в очереди
} conn_info_t;

// Тип очереди клиентских запросов
typedef struct {
	int next_add;	// Индекс слота для вставки нового запроса
	int next_pop;	// Индекс слота для извлечения следующего запроса
	pthread_mutex_t mtx;	// Мьютекс для разграничения доступа к next_pop
	sem_t sem;	// Семафор для сигнализации о поступившем запросе
	conn_info_t jobs[QUEUE_SIZE];	// Массив запросов/задач
} req_queue_t;

// Индексы сокетов для работы с массивом
enum { SKT_TCP = 0, SKT_UDP = 1 };


int open_socket(int mode);

void* request_handler(void*);

void send_to_client(const conn_info_t* ci, const char* data);

void sig_int_hnd(int);

void init_address(struct sockaddr_in* addr);

int inc_index(int* id);

void init_queue();


#endif // SERVER_H_INCLUDED

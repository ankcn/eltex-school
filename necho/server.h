#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED


// Допустимое количество подключений к сокету
#define MAX_CONNECTIONS	11

// Количество потоков для обработки запросов клиентов
#define THREADS_NUM	5

// Объём буфера под строку заголовка для ответа клиенту
#define	CAP_MAX_LEN	64


// Тип структуры для функционирования обработчика клиентских запросов
typedef struct {
	size_t bytes;	// Количество байт, принятых от клиента
	int conn_fd;	// Дескриптор соединения
	struct sockaddr_in claddr;	// Адрес клиента
	char buf[BUF_SIZE];	// Буфер для получения данных от клиента
	pthread_t thread;	// Поток
	pthread_mutex_t mtx;	// Мьютекс для блокирования доступа
	pthread_cond_t cnd;	// Флаг состояния для управления потоком
	int id;	// Идентификатор (порядковый номер) потока/обработчика
} conn_info_t;


void echo_server();

void* request_handler(void*);

void send_to_client(const conn_info_t* ci, const char* data);


#endif // SERVER_H_INCLUDED

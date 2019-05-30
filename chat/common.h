#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED


// Максимальная длина сообщения
#define MSG_LEN	555

// Права доступа к разделяемой памяти
#define SH_MEM_RIGHTS	0660

// Максимальная длина псевдонимов
#define MAX_NAME_LEN	15

// Количество сообщений в очереди (буфере)
#define MSG_BUF_LEN	11

// Имя объекта в разделяемой памяти
#define SHARED_MEM_NAME "/chat-shm"

// Имя семафора
#define SEM_NAME	"/chat"

// Размер буфера сообщений
#define MSG_BUF_SZ	sizeof(message_buffer_t)


// Тип структуры сообщения
typedef struct {
	char name[MAX_NAME_LEN + 1]; // Псевдоним пользователя
	char content[MSG_LEN]; // Текст сообщения
} message_t;

// Тип структуры очереди (циклический буфер) сообщений
typedef struct {
	int last;	// Номер последнего добавленного сообщения
	message_t history[MSG_BUF_LEN]; // Массив сообщений
} message_buffer_t;


void chating(const char* name);


#endif // COMMON_H_INCLUDED

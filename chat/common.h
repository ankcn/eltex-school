#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timestamp.h"


// Максимальная длина сообщения
#define MSG_LEN	555

// Ключ очереди сообщений
#define MSG_KEY	12345

// Права доступа очереди сообщений
#define QUEUE_RIGHTS	0660

// Максимальное количество клиентов
#define MAX_CLIENTS	22

// Максимальная длина псевдонимов
#define MAX_NAME_LEN	15

// Размер регистрационного сообщения
#define MSG_REG_LEN	(sizeof(message_reg_t) - sizeof(long))

// Длина строки с уведомлением о подключении участника
#define JOIN_STR_SIZE	55

// Содержимое строки уведомления
#define JOIN_HEAD_STR	"Joined to chat: "

// Позиция в строке, в которую нужно вставить имя участника
#define JOIN_HEAD_LENGTH	strlen(JOIN_HEAD_STR)


// Тип структуры обычного сообщения
typedef struct {
	long type; // Тип сообщения
	char content[MSG_LEN]; // Текст сообщения
} message_t;

// Тип структуры регистрационного сообщения
typedef struct {
	long type; // Тип сообщения
	int qid; // Номер очереди сообщений клиента
	char name[MAX_NAME_LEN + 1]; // Псевдоним пользователя
} message_reg_t;


// Значения типов сообщений
enum {
	MSG_TYPE_ANY = 0,
	MSG_TYPE_SERVER,
	MSG_TYPE_CLIENT
};


#endif // COMMON_H_INCLUDED

#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//
#define MSG_LEN	555

//
#define MSG_KEY	12345

//
#define QUEUE_RIGHTS	0660

//
#define MAX_CLIENTS	22

//
#define MAX_NAME_LEN	15

//
#define MSG_REG_LEN	(sizeof(message_reg_t) - sizeof(long))


//
typedef struct {
	long type;
	char content[MSG_LEN];
} message_t;

//
typedef struct {
	long type;
	int qid;
	char name[MAX_NAME_LEN];
} message_reg_t;


#endif // COMMON_H_INCLUDED

#include "common.h"



struct {
	ssize_t count;
	char* names[MAX_CLIENTS];
	int qids[MAX_CLIENTS];
} clients = { 0 };


void broadcast(const char* text)
{
	for (int i = 0; i < clients.count; ++i) {
		message_t msg;
		msg.type = 1;
		strcpy(msg.content, text);
		if (msgsnd(clients.qids[i], &msg, strlen(text), 0) < 0)
			printf("Error while sending message to %s", clients.names[i]);
	}
}


void reg_client(const int id, const char* name)
{
	if (clients.count == MAX_CLIENTS)
		return;

	clients.qids[clients.count] = id;
	char* cn = malloc(strlen(name) + 1);
	strcpy(cn, name);
	clients.names[clients.count] = cn;
	++clients.count;

	printf("Client joined: %s on queue %d\n", name, id);
	char str[MSG_LEN];
	sprintf(str, "Joined %s", name);
	broadcast(str);
}


void cleanup()
{
	for (int i = 0; i < clients.count; ++i)
		free(clients.names[i]);
}


void start_server()
{
	atexit(cleanup);
	int qid = msgget(MSG_KEY, IPC_CREAT | QUEUE_RIGHTS);
	if (qid < 0) {
		perror("start_server");
		exit(EXIT_FAILURE);
	}

	do {
		message_reg_t rm;
		if (msgrcv(qid, &rm, MSG_REG_LEN, 0, MSG_NOERROR | IPC_NOWAIT) > 0)
			reg_client(rm.qid, rm.name);

		for (int i = 0; i < clients.count; ++i) {
			message_t mes;
			if (msgrcv(clients.qids[i], &mes, MSG_LEN, 0, MSG_NOERROR | IPC_NOWAIT) > 0)
				broadcast(mes.content);
		}

//		puts("test");
	} while (getchar() != 'q');

	msgctl(qid, IPC_RMID, NULL);
}



#include "common.h"
#include "noecho.h"


//
int qid;


void reg_on_server(const char* name)
{
	if (strlen(name) < 3) {
		puts("Nickname must be minimum three characters");
		exit(EXIT_FAILURE);
	}
	puts("Trying to register on server");

	int sqid = msgget(MSG_KEY, 0);
	if (sqid == -1) {
		perror("client: reg_on_server, get server queue id");
		exit(EXIT_FAILURE);
	}

    if ((qid = msgget(IPC_PRIVATE, QUEUE_RIGHTS)) < 0) {
		perror("client: reg_on_server, get private queue id");
		exit(EXIT_FAILURE);
	}

    message_reg_t msg;
    msg.qid = qid;
    msg.type = 1;
    strcpy(msg.name, name);

	if (msgsnd(sqid, &msg, MSG_REG_LEN, 0) < 0) {
		perror("client: reg_on_server, send our queue id");
		exit(EXIT_FAILURE);
	}
}

/*
void send_message(const char* text)
{
    message_t msg;
    strcpy(msg.content, text);
    unsigned len = strlen(text);
    msg.content[len] = 0;
    msg.type = 1;
    msgsnd(qid, &msg, len, 0);
}
*/

void chating()
{
	puts("Press Enter to write a message or q for exit");

	int c;
	message_t msg;
	msg.type = 1;

	do {
		c = getchar();

		// write message
		if (c == '\n') {
			echo_normal();
			show_cursor();
			scanf("%s", msg.content);
			msgsnd(qid, &msg, strlen(msg.content) + 1, 0);
			echo_custom();
			hide_cursor();
		}

		// check new messages
		if (msgrcv(qid, &msg, MSG_LEN, 0, MSG_NOERROR | IPC_NOWAIT) > 0) {
			// TODO: timestamp
			puts(msg.content);
		}
	} while (c != 'q');

	msgctl(qid, IPC_RMID, NULL);
}


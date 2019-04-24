#include "common.h"
#include "noecho.h"


// Номер очереди сообщений данного клиента
int qid;


/*
 * reg_on_server - Регистрация на сервере
 * @name: имя (псевдоним) пользователя
 * В очередь сообщений сервера отправляется сообщение с именем
 * пользователя и номером очереди сообщений данного клиента
 */
void reg_on_server(const char* name)
{
	// Проверка корректности длинны имени
	if (strlen(name) < 3 || strlen(name) > MAX_NAME_LEN) {
		printf("Nickname length must be [3..%d] characters", MAX_NAME_LEN);
		exit(EXIT_FAILURE);
	}
	puts("Trying to register on server...");

	// Определение номера очереди сообщений сервера по фиксированному ключу
	int sqid = msgget(MSG_KEY, 0);
	if (sqid == -1) {
		perror("client: reg_on_server, get server queue id");
		exit(EXIT_FAILURE);
	}

	// Создание собственной очереди сообщений (клиента)
	if ((qid = msgget(IPC_PRIVATE, QUEUE_RIGHTS)) < 0) {
		perror("client: reg_on_server, get private queue id");
		exit(EXIT_FAILURE);
	}

	// Отправка регистрационного сообщения серверу
	message_reg_t msg;
	msg.qid = qid;
	msg.type = MSG_TYPE_CLIENT;
	strcpy(msg.name, name);

	if (msgsnd(sqid, &msg, MSG_REG_LEN, 0) < 0) {
		perror("client: reg_on_server, send our queue id");
		exit(EXIT_FAILURE);
	}
}

/*
 * chating - Процесс обмена сообщениями
 * В цикле, пока не нажата клавиша q, выполняются две вещи: 
 * 1. Проверяется наличие сообщений в очереди. Полученное сообщение отображается.
 * 2. Если нажат Enter, начинается ввод текста до следующего нажатия Enter, 
 * после чего сообщение отправляется на сервер
 */
void chating()
{
	puts("Press Enter to write a message or q for exit\n");

	int c;	// Код нажатой клавиши
	message_t msg;	// Сообщение

	do {
		c = getchar();

		// Ввод и отправка сообщения
		if (c == '\n') {
			// Переход в нормальный режим терминала
			echo_normal();
			show_cursor();
			char* line = msg.content;
			// Считываем ввод
			while ((*line++ = getchar()) != '\n');
			// Удаляем текущую строку с экрана
			printf("\e[F\e[2K");
			// Терминируем полученную строку перед отправкой
			*--line = '\0';
			msg.type = MSG_TYPE_CLIENT;
			// Отправляем сообщение на сервер
			msgsnd(qid, &msg, strlen(msg.content) + 1, 0);
			// Возвращаем "глухой" режим терминала
			echo_custom();
			hide_cursor();
		}

		// Проверка новых сообщений
		if (msgrcv(qid, &msg, MSG_LEN, MSG_TYPE_SERVER, MSG_NOERROR | IPC_NOWAIT) > 0) {
			printf("%s; %s\n\n", get_date_str(), msg.content);
		}

	// Выход по нажатию q
	} while (c != 'q');

	// Удаление очереди сообщений
	msgctl(qid, IPC_RMID, NULL);
}


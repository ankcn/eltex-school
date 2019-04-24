#include "common.h"


/*
 * clients - Структура с информацией о клиентах
 * @count: количество подключенных участников
 * @names: псевдонимы пользователей
 * @qids: идентификаторы очередей клиентов
 */
struct {
	ssize_t count;
	char* names[MAX_CLIENTS];
	int qids[MAX_CLIENTS];
} clients = { 0 };


/*
 * broadcast - Отправка сообщения всем участникам
 * @head: заголовок или имя отправителя
 * @text: текст сообщения
 * @msg: структура сообщения для передачи в msgsnd
 */
void broadcast(const char* head, const char* text)
{
	message_t msg;
	msg.type = MSG_TYPE_SERVER;

	// Форматируем заголовок и текст в одну строку
	sprintf(msg.content, "%s:\n%s", head, text);

	// Перебираем всех участников и отправляем каждому подготовленное сообщение
	for (int i = 0; i < clients.count; ++i) {
		if (msgsnd(clients.qids[i], &msg, strlen(msg.content) + 1, 0) < 0)
			printf("Error while sending message to %s\n", clients.names[i]);
	}
}

/*
 * reg_client - Регистрация клиента
 * @id: идентификатор очереди данного клиента
 * @name: псевдоним пользователя
 * @cn: копия псевдонима для хранения в структуре clients
 */
void reg_client(const int id, const char* name)
{
	// Проверям, не достигнут ли лимит клиентов
	if (clients.count == MAX_CLIENTS)
		return;

	// Запоминаем идентификатор очереди
	clients.qids[clients.count] = id;
	// Выделяем память под копию имени
	char* cn = malloc(strlen(name) + 1);
	// Копируем и сохраняем псевдоним
	strcpy(cn, name);
	clients.names[clients.count] = cn;
	// Увеличиваем счётчик клиентов
	++clients.count;
	
	// Формируем строку с уведомлением о подключении участника
	char join[JOIN_STR_SIZE] = JOIN_HEAD_STR;
	strcpy(&join[JOIN_HEAD_LENGTH], name);

	printf("%s: Client joined: %s on queue %d\n\n", get_date_str(), name, id);
	// Рассылаем всем участникам уведомление
	broadcast("server", join);
}

/*
 * cleanup - Освобождение памяти, выделенной под имена участников
 */
void cleanup()
{
	for (int i = 0; i < clients.count; ++i)
		free(clients.names[i]);
}

/*
 * start_server - Старт сервера
 * @qid: идентификатор очереди сообщений сервера (для регистрации клиентов)
 * @rm: регистрационное сообщение
 * @mes: текстовое сообщение пользователя
 * В цикле, пока не нажата клавиша q, выполняются две вещи:
 * 1. При получении регистрационного сообщения в собственной
 *    очереди qid выполняется регистрация клиента.
 * 2. При получении обычного сообщения в очередях клиентов
 *    выполняется его пересылка всем участникам
 */
void start_server()
{
	// Перед выходом освобождаем память
	atexit(cleanup);

	// Создаём очередь сообщений для регистрации клиентов
	int qid = msgget(MSG_KEY, IPC_CREAT | QUEUE_RIGHTS);
	if (qid < 0) {
		perror("start_server");
		exit(EXIT_FAILURE);
	}

	// Считываем все неактуальные сообщения в очереди, если они есть
	message_reg_t rm;
	while (msgrcv(qid, &rm, MSG_REG_LEN, MSG_TYPE_ANY, MSG_NOERROR | IPC_NOWAIT) > 0);

	do {
		// Если получено регистрационное сообщение, добавляем нового клиента
		if (msgrcv(qid, &rm, MSG_REG_LEN, MSG_TYPE_ANY, MSG_NOERROR | IPC_NOWAIT) > 0)
			reg_client(rm.qid, rm.name);

		// Перебираем подключенных клиентов и проверяем их очереди
		for (int i = 0; i < clients.count; ++i) {
			message_t mes;
			// Если от кого-то пришло сообщение, пересылаем его всем участникам чата
			if (msgrcv(clients.qids[i], &mes, MSG_LEN, MSG_TYPE_CLIENT, MSG_NOERROR | IPC_NOWAIT) > 0) {
				printf("%s: Received from %s:\n%s\n\n", get_date_str(), clients.names[i], mes.content);
				broadcast(clients.names[i], mes.content);
			}
		}
	// Завершаем работу по нажатию клавиши q
	} while (getchar() != 'q');

	// Уадаление очереди сообщений
	msgctl(qid, IPC_RMID, NULL);
}



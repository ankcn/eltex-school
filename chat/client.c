#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>

#include "common.h"
#include "noecho.h"
#include "timestamp.h"


// Псевдоним участника чата
char nickname[MAX_NAME_LEN] = "anonymous";

/*
 * send_message - Отправка сообщения в чат
 * @buf: указатель на структуру с очередью сообщений
 * @str: строка сообщения для отправки
 * @sm: указатель на семафор для безопасного доступа к buf
 */
void send_message(message_buffer_t* buf, const char* str, sem_t* sm)
{
	// Ждём освобождения свободного состояния семафора и тут же занимаем его сами
	sem_wait(sm);

	// Циклический инкремент счётчика сообщений в очереди
	if (++buf->last == MSG_BUF_LEN)
		buf->last = 0;

	// Копирование текста сообщения и имени автора в разделяемую память
	message_t* msg = &buf->history[buf->last];
	strcpy(msg->content, str);
	strcpy(msg->name, nickname);

	// Освобождаем семафор
	sem_post(sm);
}


/*
 * chating - Процесс обмена сообщениями
 * @name: псевдоним участника чата
 * В цикле, пока не нажата клавиша q, выполняются две вещи:
 * 1. Проверяется наличие сообщений в очереди. Полученное сообщение отображается.
 * 2. Если нажат Enter, начинается ввод текста до следующего нажатия Enter,
 * после чего сообщение отправляется другим участникам
 */
void chating(const char* name)
{
	strcpy(nickname, name);

	// Получаем дескриптор объекта в разделяемой памяти
	int memd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, SH_MEM_RIGHTS);
	if (memd < 0)
		perror("shm_open"), exit(EXIT_FAILURE);

	// Задаём размер памяти под наш объект
	ftruncate(memd, MSG_BUF_SZ);
	message_buffer_t* mes_buf = mmap(0, MSG_BUF_SZ, PROT_READ | PROT_WRITE, MAP_SHARED, memd, 0);

	int c;	// Код нажатой клавиши
	int last = mes_buf->last;	// Номер последнего полученного сообщения

	// Открываем/создаём семафор
	sem_t* busy = sem_open(SEM_NAME, O_CREAT, SH_MEM_RIGHTS, 1);
	if (busy == SEM_FAILED)
		perror("sem_open"), exit(EXIT_FAILURE);

	puts("Press Enter to write a message or q for exit\n");
	send_message(mes_buf, "I'm here", busy);

	do {
		c = getchar();

		// Ввод и отправка сообщения
		if (c == '\n') {
			// Переход в нормальный режим терминала
			echo_normal();
			show_cursor();
			char line[MSG_LEN];
			size_t pos = 0;

			// Считываем ввод
			while ((line[pos++] = getchar()) != '\n');

			// Удаляем текущую строку с экрана
			printf("\e[F\e[2K");

			// Терминируем полученную строку перед отправкой
			line[--pos] = '\0';

			// Отправляем введённый текст в чат
			send_message(mes_buf, line, busy);

			// Возвращаем "глухой" режим терминала
			echo_custom();
			hide_cursor();
		}

		// Проверка новых сообщений
		// Если номер последнего полученного сообщения отличается от номера самого
		// свежего сообщения в очереди, то печатаем все новые сообщения
		while (last != mes_buf->last) {
			if (++last == MSG_BUF_LEN)
				last = 0;
			message_t* mes = &mes_buf->history[last];
			printf("%s; %s\n%s\n\n", get_date_str(), mes->name, mes->content);
		}

	// Выход по нажатию q
	} while (c != 'q');

	send_message(mes_buf, "I'm quit", busy);

	// Удаление семафора и объекта в разделяемой памяти
	munmap(mes_buf, MSG_BUF_SZ);
	close(memd);
	shm_unlink(SHARED_MEM_NAME);
	sem_unlink(SEM_NAME);
}


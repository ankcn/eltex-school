#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <ctype.h>
#include "interface.h"
#include "stroka.h"
#include "history.h"
#include "shell.h"


// Строка с командой для выхода из программы
const char CMD_EXIT[] = "exit";

// Строка с приглашением командной строки
const char PROMPT[] = "shell> ";

// Переменная для хранения исходных настроек терминала
struct termios attr_backup;

// Переменная для редактирования вводимой команды
stroka line;

/**
 * echo_normal - Восстановление исходных настроек терминала
 * Здесь же вызов функции очистки истории
 */
void echo_normal()
{
	tcsetattr(STDIN_FILENO, TCSANOW, &attr_backup);
	history_free();
}

/**
 * echo_custom - Изменение настроек терминала
 * Отключается эхо вводимых символов
 */
void echo_custom()
{
	struct termios tattr;

	if (! isatty(STDIN_FILENO)) {
		fprintf(stderr, "Not a terminal.\n");
		exit(EXIT_FAILURE);
    }

	tcgetattr(STDIN_FILENO, &attr_backup);
	atexit(echo_normal);

	tcgetattr(STDIN_FILENO, &tattr);
	tattr.c_lflag &= ~(ICANON | ECHO);
	tattr.c_cc[VMIN] = 1;
	tattr.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);
}

/**
 * replace_line - Замена редактируемой команды на заданную строку
 * @str: строка для замены
 */
void replace_line(const char* str)
{
	if (! str)
		return;
	stroka_load(&line, str);
	printf("\r%s%s", PROMPT, str);
	move_cursor(ERASE_TO_THE_END_OF_LINE);
}

/**
 * operate - работа терминала, взаимодействие с пользователем
 * Считываются коды введённых символов, выполняются соотвествующие действия
 */
void operate()
{
	// Обрабатываем команды в цикле до получения команды выхода
	do {
		// Выводим приглашение командной строки
		printf(PROMPT);
		int c;
		// Обнуляем счётчики буфера вводимой команды
		stroka_reset(&line);
		// Считываем символ со входного потока
		while ((c = getchar()) != '\n' && c != EOF) {
			// Если попался код ESC, то продолжаем считывание для получения ESC последовательности
			if (c == KEY_ESC)
				c = get_esc_seq();

			switch (c) {
			// Если была нажата стрелка вверх, то загружаем предыдущую команду из истории
			case ARROW_UP:
				replace_line(history_get_prev());
				break;
			// В случае стрелки вниз загружаем следующую команду из истории
			case ARROW_DOWN:
				replace_line(history_get_next());
				break;
			// По нажатию на стрелку вправо сдвигаем курсор на экране и позицю в буфере в правую сторону
			case ARROW_RIGHT:
				if (line.pos < line.size) {
					move_cursor(ARROW_RIGHT);
					++line.pos;
				}
				break;
			// По левой стрелке двигаем курсор и позицию в буфере влево
			case ARROW_LEFT:
				if (line.pos) {
					move_cursor(ARROW_LEFT);
					--line.pos;
				}
				break;
			// По клавише Delete удаляем в буфере символ в текущей позиции
			case KEY_DELETE:
				stroka_delete(&line);
				break;
			// По клавише Backspace затираем предшествующий курсору символ
			case KEY_BACKSPACE:
				if (line.pos) {
					move_cursor(ARROW_LEFT);
					if (line.pos == line.size) {
						putchar(' ');
						move_cursor(ARROW_LEFT);
					}
					stroka_backspase(&line);
				}
			// Если получен печатный символ, то выводим его на экран и добавляем в буфер
			default:
				if (isprint(c)) {
					stroka_insert(&line, c);
					putchar(c);
				}
			}

			// Если курсор - не в конце строки и нажатая клавиша - не стрелка
			if (line.pos < line.size && c < ARROW_UP)
				// тогда переотрисовываем на экране часть строки после курсора
				reprint_last();
		}

		// Убираем пробелы и табуляцию в начале команды
		char* cmd = line.text;
		while (isspace(*cmd))
			++cmd;

		puts("");
		if (strlen(cmd)) {
			history_add(cmd);
			//system(cmd);
			parse_command(cmd);
		}

	// Завершаем работу, когда получена команда на выход
	} while (strcmp(line.text, CMD_EXIT));
}

/**
 * get_esc_seq - Получение ESC последовательности
 * @return: возвращает целое число - код введённой ESC последовательности
 */
int get_esc_seq()
{
	if (getc(stdin) == ESC_CSI) {
		int c = getc(stdin);
		if (c == DEL_PREFIX)
			c = getc(stdin);
		return csi_prefix(c);

	} else
		return -1;
}

/**
 * move_cursor - Перемещение курсора, либо другая команда терминала
 */
inline void move_cursor(const int arrow)
{
	printf("\033[%c", arrow & 0xFF);
}

/**
 * reprint_last - Переотрисовка части редактируемой строки
 * На стандартный вывод отправляется часть строки, начиная с позиции курсора
 */
void reprint_last()
{
	// Запоминаем текущую позицию курсора в терминале
	move_cursor(CURSOR_SAVE);
	// Выводим текст
	printf("%s ", &line.text[line.pos]);
	// Восстанавливаем положение курсора
	move_cursor(CURSOR_RESTORE);
}


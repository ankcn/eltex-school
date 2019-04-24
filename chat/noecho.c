#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>


// Переменная для хранения исходных настроек терминала
struct termios attr_backup;


/**
 * echo_normal - Восстановление исходных настроек терминала
 */
void echo_normal()
{
	tcsetattr(STDIN_FILENO, TCSANOW, &attr_backup);
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

	tattr = attr_backup;
	tattr.c_lflag &= ~(ICANON | ECHO | ISIG);
	tattr.c_cc[VMIN] = 0;
	tattr.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);
}

/*
 * hide_cursor - Скрыть курсор
 */
void hide_cursor()
{
	printf("\e[?25l");
	fflush(stdout);
}

/*
 * show_cursor - Показать курсор
 */
void show_cursor()
{
	printf("\e[?25h");
	fflush(stdout);
}



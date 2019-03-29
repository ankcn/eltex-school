#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "shell.h"
#include "interface.h"


// Строка с командой для выхода из программы
const char* CMD_EXIT = "exit";

// Команда смены текущей директории
const char* CMD_CD = "cd";


/**
 * parse_command - Обработка команды
 * @cmd: строка с командой
 * @return: возвращает код результата работы
 * @varg: массив для параметров команды, полученных из cmd
 * @part: часть команды для последовательного разбора
 * @ret: код результата
 * @path: путь к исполняемому файлу
 */
int parse_command(char* cmd)
{
	char* varg[MAX_ARGS];
	char* part = strtok(cmd, " \t");
	unsigned i = 0;
	int ret = RET_NORMAL;

	// Строка разбивается на слова, которые раскладываются по массиву параметров varg
	while (part != NULL) {
		varg[i] = strdupa(part);
		part = strtok(NULL, " \t");
		++i;
	}

	// Если в строке не найдено ни одного слова, то возвращаем ошибку
	if (! i)
		return RET_ERROR;

	varg[i] = NULL;

	// Путь к исполняемому файлу - первое слово команды
	char* path = strdupa(varg[0]);

	// Команду cd выполняем на месте
	if (! strcmp(path, CMD_CD)) {
		if (varg[1] != NULL)
			chdir(varg[1]);
	// В случае команды "выход" присваиваем соответствующий код для возврата
	} else if (! strcmp(path, CMD_EXIT))
		ret = RET_EXIT;
	else {
		// Выделяем имя файла из пути
		char* name = strrchr(path, '/');
		if (name == NULL)
			name = path;
		else
			++name;
		varg[0] = name;

		// Запускаем дочерний процесс
		pid_t pid = fork();
		if (pid < 0)
			return RET_ERROR;
		// В родитеском процессе временно восстанавливаем нормальный режим терминала и ждём завершения дочернего
		else if (pid > 0) {
			echo_normal();
			int st;
			pid = wait(&st);
			echo_custom();
			if (st)
				ret = RET_ERROR;
		// В дочернем процессе вызываем внешний исполняемый файл с массивом параметров
		} else if (execvp(path, varg) == -1)
			exit(EXIT_FAILURE);
	}

	return ret;
}






#include <string.h>
#include <stdlib.h>
#include "history.h"


/**
 * history - Структура, содержащая историю ранее введённых команд
 * @selected: текущая выбранная позиция, номер строки/комнады
 * @size: размер истории, количество записей
 * @vacancy: номер позиции, в которую будет добавлена следующая запись
 * @lines: массив указателей на строки, в которых хранятся команды
 */
struct {
	unsigned selected;
	unsigned size;
	unsigned vacancy;
	char* lines[HISTORY_MAX_SIZE];
} history = { 0, 0, 0 };


/**
 * history_add - Добавление новой записи в историю
 * @line: строка, которая будет добавлена в историю
 */
void history_add(const char* line)
{
	// Создаём ярлык lp для удобного обращения к вакантной строке в истории
	char** lp = &history.lines[history.vacancy];
	// Перевыделяем память под новое содержимое
	*lp = realloc(*lp, strlen(line));
	// Копируем содержимое полученной строки в вакантную позицию истории
	strcpy(*lp, line);

	// Зацикливание массива строк
	if (++history.vacancy >= HISTORY_MAX_SIZE)
		history.vacancy = 0;

	history.selected = history.vacancy;

	// Увеличиваем размер истории до тех пор, пока она не достигла заданного предела
	if (history.size < HISTORY_MAX_SIZE)
		++history.size;
}

/**
 * history_free - Очистка истории команд
 * Здесь высвобождается память, выделенная для хранения строк с командами
 */
void history_free()
{
	for (unsigned i = 0; i < history.size; ++i)
		free(history.lines[i]);
}

/**
 * history_get_selected - Получение выбранной строки из истории
 * @return: возвращает строку из истории по текущему выбранному номеру
 */
char* history_get_selected()
{
	return history.lines[history.selected];
}

/**
 * history_get_next - Получение следующей строки из истории
 * @return: возвращает строку из истории по номеру, следующему за выбранным
 */
char* history_get_next()
{
	unsigned* hs = &history.selected;

	if (*hs != (history.vacancy + HISTORY_MAX_SIZE - 1) % HISTORY_MAX_SIZE && *hs != history.vacancy)
		++*hs;
	else
		return 0;

	if (*hs == HISTORY_MAX_SIZE)
		*hs = 0;

	return history_get_selected();
}

/**
 * history_get_prev - Получение предыдущей строки из истории
 * @return: возвращает строку из истории по номеру, предшествующему выбранному
 */
char* history_get_prev()
{
	unsigned* hs = &history.selected;

	if (*hs == (history.vacancy + 1) || (*hs == 0 && (history.size < HISTORY_MAX_SIZE)))
		return 0;

	if (! *hs)
		*hs = HISTORY_MAX_SIZE - 1;
	else
		--*hs;

	return history_get_selected();
}


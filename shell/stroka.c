/**
 * Модуль, реализцющий функции рабты с типом stroka, представляющем собой буфер
 * для редактирования текстового поля
 */

#include <string.h>
#include "stroka.h"


/**
 * stroka_insert - Вставка символа в текущую позицию
 * @s: указатель на экземпляр типа stroka, в который следуюет добавить символ
 * @c: код символа для вставки
 * @return: возвращает 0 в случае успеха, 1 - если достигнут максимальный размер строки
 */
int stroka_insert(stroka* s, const char c)
{
	if (s->size >= MAX_CMD_LENGTH - 1)
		return 1;

	if (s->pos < s->size)
		for (int i = s->size; i > s->pos; --i)
			s->text[i] = s->text[i - 1];
	s->text[s->pos++] = c;
	s->text[++s->size] = 0;

	return 0;
}

/**
 * stroka_backspase - Стирание предыдущего символа
 * @s: указатель на экземпляр типа stroka, над которым необходимо выполнить операцию
 * Часть строки, начиная с позиции курсора и до конца, сдвигается на одну позицию влево
 */
void stroka_backspase(stroka* s)
{
	if (! s->pos)
		return;

	--s->pos;

	for (int i = s->pos; i < s->size - 1; ++i)
		s->text[i] = s->text[i + 1];

	s->text[--s->size] = 0;
}

/**
 * stroka_delete - Удаление символа в позиции курсора
 * @s: указатель на экземпляр типа stroka, над которым необходимо выполнить операцию
 */
void stroka_delete(stroka* s)
{
	if (s->pos < s->size) {
		++s->pos;
		stroka_backspase(s);
	}
}

/**
 * stroka_reset - Сброс, очистка
 * @s: указатель на экземпляр типа stroka, над которым необходимо выполнить операцию
 */
void stroka_reset(stroka* s)
{
	s->pos = s->size = 0;
	s->text[0] = 0;
}

/**
 * stroka_load - Замещение содержимого буфера заданным текстом
 * @s: указатель на экземпляр типа stroka, над которым необходимо выполнить операцию
 * @src: источник текста для замещения
 */
void stroka_load(stroka* s, const char* src)
{
	strcpy(s->text, src);
	s->size = s->pos = strlen(src);
}



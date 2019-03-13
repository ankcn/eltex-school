#ifndef STROKA_H_INCLUDED
#define STROKA_H_INCLUDED


// Максимальная длина строки с командой
#define MAX_CMD_LENGTH	1024


/**
 * stroka - Тип структуры, представляющей редактируемый текстовый буфер
 * @text: собственно строка с текстом
 * @pos: позиция "курсора" в строке
 * @size: длина строки
 */
typedef struct {
	char text[MAX_CMD_LENGTH];
	unsigned pos;
	unsigned size;
} stroka;


int stroka_insert(stroka* s, const char c);

void stroka_backspase(stroka* s);

void stroka_delete(stroka* s);

void stroka_reset(stroka* s);

void stroka_load(stroka* s, const char* src);


#endif // STROKA_H_INCLUDED

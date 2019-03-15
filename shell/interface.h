#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED


// Код клавиши ESC
#define KEY_ESC	27

// Префикс управляющей последовательности
#define ESC_CSI	91

// Макрос формирования двухбайтового кода с префиксом
#define csi_prefix(k)	((ESC_CSI << 8) + k)

// Код клавиши Backspace
#define KEY_BACKSPACE	127

// Префикс кода клавиши Delete
#define	DEL_PREFIX	51

// Код клавиши Delete
#define KEY_DELETE	csi_prefix(126)

// Клавиша стрелка вверх
#define ARROW_UP	csi_prefix('A')

// Клавиша стрелка вниз
#define ARROW_DOWN	csi_prefix('B')

// Клавиша стрелка влево
#define ARROW_LEFT	csi_prefix('D')

// Клавиша стрелка вправо
#define ARROW_RIGHT	csi_prefix('C')

// Код команды сохранить позицию курсора
#define CURSOR_SAVE	's'

// Код команды восстановить позицию курсора
#define CURSOR_RESTORE	'u'

// Код команды стереть строку терминала от курсора до конца экрана
#define ERASE_TO_THE_END_OF_LINE 'K'


void echo_custom();

void operate();

int get_esc_seq();

void move_cursor(const int arrow);

void reprint_last();

void echo_normal();


#endif // INTERFACE_H_INCLUDED

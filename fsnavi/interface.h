#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED


// Макрос для получения кода сочетания клавиш CTRL + (k)
#define WCTRL(k)	(k - 0100)


// Инициализация и определение параметров экрана ncurses, выделение памяти для документа
void prepare();

// Закрытие экрана, высвобождение памяти
void finish();

// Получить код символа/клавиши о
int get_key();


#endif // INTERFACE_H_INCLUDED

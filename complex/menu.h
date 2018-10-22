#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED


#include "complex.h"


// Предельная Длинна строки с вещественным числом
#define NUMBER_LENGTH	15

// Вывод пунктов меню
int menu_title();

// Ввод вещественного числа
float enter_value(const char* req);

// Обработка введённой комнады
int menu_process(const char c);

// Ввод комплексного числа
complex enter_complex();


#endif

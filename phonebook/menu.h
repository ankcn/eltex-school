#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#define INPUT_STRING_LIMIT	"14"


// Вывод пунктов меню
int menu_title();

// Ввод строки от пользователя
void enter_string(const char* req, char* str);

// Ввод имени и фамилии
void enter_name(char* fn, char* sn);

// Ввод числа
int enter_value(const char* req, int def);

// Пункт меню Добавить контакт
void menu_add();

// Пункт меню Поиск
void menu_find();

// Пункт меню печать телефонной книги
void menu_print();

// Пункт меню Удалить контакт
void menu_delete();

// Обработка введённой комнады
void menu_process(const char c);


#endif

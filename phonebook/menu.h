#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED


// Вывод пунктов меню
int menu_title();


// Ввод строки от пользователя
void enter_string(char* req, char* str);


// Ввод имени и фамилии
void enter_name(char* fn, char* sn);


// Пункт меню Добавить контакт
void menu_add();


// Пункт меню Поиск
void menu_find();


// Пункт меню печать телефонной книги
void menu_print();


// Пункт меню Удалить контакт
void menu_delete();


// Обработка введённой комнады
void menu_process(char c);


#endif

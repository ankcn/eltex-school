#ifndef PHONEBOOK_H_INCLUDED
#define PHONEBOOK_H_INCLUDED


#define FIELD_SIZE	15	// Размер полей под имя, фамилию и номер телефона
#define PHONEBOOK_SIZE	100	// Максимальное количество записей в телефонной книге

// Тип структуры, описывающей запись в телефонной книге
typedef struct {
	char name[FIELD_SIZE];		// Имя
	char sname[FIELD_SIZE];		// Фамилия
	char number[FIELD_SIZE];	// Номер телефона
} chel;

// Копирование строки из массива src в dst
void copy_str(const char* src, char* dst);

// Пуста ли строка?
int is_line_empty(const unsigned int i);

// Поиск первой свободной "ячейки" в телефонной книге
int find_free();

// Добавить контакт в книгу
int add_chel(const char* fn, const char* sn, const char* num);

// Одинаковы ли строки a и b?
int is_equal(const char* a, const char* b);

// Поиск контакта в книге по имени и фамилии
int find_chel(const char* fn, const char* sn);

// Удаление записи в книге по индексу
void delete_record(const unsigned int i);

// Напечатать запись из книги по индексу
void print_record(const unsigned int i);

// Удалить контакт из книги по имени и фамилии
void delete_chel(const char* fn, const char* sn);

// Найти контакт в книге и напечатать его
void find_and_print(const char* fn, const char* sn);


#endif // PHONEBOOK_H_INCLUDED

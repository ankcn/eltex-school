// Телефонный справочник

#include <stdio.h>
#include <stdlib.h>


#define FIELD_SIZE	15	// Размер полей под имя, фамилию и номер телефона
#define PHONEBOOK_SIZE	100	// Максимальное количество записей в телефонной книге


// Тип структуры, описывающей запись в телефонной книге
typedef struct {
	char name[FIELD_SIZE];		// Имя
	char sname[FIELD_SIZE];		// Фамилия
	char number[FIELD_SIZE];	// Номер телефона
} chel;


// Массив записей имён и номеров - телефонная книга
chel book[PHONEBOOK_SIZE];


// Копирование строки из массива src в dst
void copy_str(char* src, char* dst) {
	int i = 0;
	while (++i <= FIELD_SIZE && *src) {
		*dst++ = *src++;
	}
	// Проверка: если прошли строку до предельного размера поля, то оконечиваем её нулём
	if (i > FIELD_SIZE)
		*(--dst) = 0;
}


// Пуста ли строка?
int is_line_empty(const unsigned int i)
{
	return i >= PHONEBOOK_SIZE || *(book[i].number) == 0;
}


// Поиск первой свободной "ячейки" в телефонной книге
int find_free()
{
	int i = 0;
	while (i < PHONEBOOK_SIZE && ! is_line_empty(i))
		++i;
	return i;
}


// Добавить контакт в книгу
int add_chel(char* fn, char* sn, char* num)
{
	unsigned int i = find_free();
	if (i >= PHONEBOOK_SIZE)
		return -1;
	copy_str(fn, book[i].name);
	copy_str(sn, book[i].sname);
	copy_str(num, book[i].number);
	return 0;
}


// Одинаковы ли строки a и b?
int is_equal(char* a, char* b)
{
	int res = 1;
	for	(int i = 0; i < FIELD_SIZE; ++i) {
		res &= *a == *b++;
		if (! *a++ || ! res)
			break;
	}
	return res;
}


// Поиск контакта в книге по имени и фамилии
int find_chel(char* fn, char* sn)
{
	int i = 0;
	while (i < PHONEBOOK_SIZE && ! (is_equal(book[i].name, fn) && is_equal(book[i].sname, sn)))
		++i;
	return i;
}


// Удаление записи в книге по индексу
void delete_record(const unsigned int i)
{
	if (i < PHONEBOOK_SIZE) {
		book[i].name[0] = 0;
		book[i].sname[0] = 0;
		book[i].number[0] = 0;
	}
}

// Напечатать запись из книги по индексу
void print_record(const unsigned int i)
{
	if (i >= PHONEBOOK_SIZE)
		printf("Wrong index");
	else if (is_line_empty(i))
		printf("#%d. -- empty --", i + 1);
	else
		printf("#%d. Full name: %s %s, phone number: %s", i + 1, book[i].sname, book[i].name, book[i].number);
	printf("\n");
}


// Удалить контакт из книги по имени и фамилии
void delete_chel(char* fn, char* sn)
{
	int i = find_chel(fn, sn);
	if (i >= PHONEBOOK_SIZE)
		printf("Not found %s %s in phonebook, nothing to delete\n", sn, fn);
	else
		delete_record(i);
}


// Найти контакт в книге и напечатать его
void find_and_print(char* fn, char* sn)
{
	int i = find_chel(fn, sn);
	if (i >= PHONEBOOK_SIZE)
		printf("Not found %s %s in phonebook\n", sn, fn);
	else {
		printf("Found:\n");
		print_record(i);
	}
}


int main()
{
	// Добавим пару контактов
	add_chel("Ivan", "Petrov", "223-322");
	add_chel("Vasya", "Pupkin", "89977445566");

	// Попробуем найти кого-нибудь
	find_and_print("Ivan", "Petrov");
	find_and_print("Zozo", "3030");

	// Удалим контакт
	delete_chel("Ivan", "Petrov");

	// Напечатаем первые пять записей из телефонной книги
	printf("\nFirst five records in phonebook: \n");
	for (int i = 0; i < 5; ++i)
		print_record(i);

	return 0;
}

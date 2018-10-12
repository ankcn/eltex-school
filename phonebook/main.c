// Телефонный справочник

#include <stdio.h>
#include "phonebook.h"


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

// Телефонный справочник

#include <stdio.h>
#include "phonebook.h"


int menu_title()
{
	printf("\nWorking with phonebook:\n"
		"a - Add new contact\n"
		"f - Find contact\n"
		"d - Delete contact\n"
		"p - Print phonebook\n"
		"q - quit\n"
	);

	int c;
	while ((c = getchar()) == '\n');
	return c;
}


void enter_string(char* req, char* str)
{
//	int c = 0;
//	int i = -1;
	printf(req);
	fflush(stdin);
/*
	do
		*str++ = c = getchar();
	while (c != EOF && c != '\n' && c != '\r' && ++i < FIELD_SIZE);
	*(--str) = 0;
*/
	scanf("%s", str);
}


void enter_name(char* fn, char* sn)
{
	enter_string("Please, enter first name: ", fn);
	enter_string("Please, enter second name: ", sn);
}


void menu_add()
{
	char temp_name[FIELD_SIZE];
	char temp_sname[FIELD_SIZE];
	char temp_phone[FIELD_SIZE];
	enter_name(temp_name, temp_sname);
	enter_string("Please, enter phone number: ", temp_phone);
	add_chel(temp_name, temp_sname, temp_phone);
}


void menu_find()
{
	char temp_name[FIELD_SIZE];
	char temp_sname[FIELD_SIZE];
	enter_name(temp_name, temp_sname);
	find_and_print(temp_name, temp_sname);
}


void menu_print()
{
	printf("\nFirst five records in phonebook: \n");
	for (int i = 0; i < 5; ++i)
		print_record(i);
}


void menu_delete()
{
	char temp_name[FIELD_SIZE];
	char temp_sname[FIELD_SIZE];
	enter_name(temp_name, temp_sname);
	delete_chel(temp_name, temp_sname);
}


void menu_process(char c)
{
	switch (c) {
		case 'a': menu_add(); break;
		case 'd': menu_delete(); break;
		case 'f': menu_find(); break;
		case 'p': menu_print(); break;
		default: printf("Unrecognized command\n");
	}
}


int main()
{
/*
	// Добавим пару контактов
	add_chel("Ivan", "Petrov", "223-322");
	add_chel("Vasya", "Pupkin", "89977445566");

	// Попробуем найти кого-нибудь
	find_and_print("Ivan", "Petrov");
	find_and_print("Zozo", "3030");

	// Удалим контакт
	delete_chel("Ivan", "Petrov");
*/
	char c;
	while ((c = menu_title()) != 'q')
		menu_process(c);

	return 0;
}

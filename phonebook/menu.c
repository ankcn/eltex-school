// Меню телефонного справочника

#include <stdio.h>
#include "menu.h"
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
	printf(req);
	fflush(stdin);
	scanf("%s", str);
	// TODO: length check
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
	// TODO: selectable range
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


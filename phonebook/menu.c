// Меню телефонного справочника

#include <stdio.h>
#include <stdlib.h>
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
	getchar(); // Remove new line from stdin buffer
	return c;
}


void enter_string(const char* req, char* str)
{
	printf(req);
	scanf("%" INPUT_STRING_LIMIT "s", str);
}


int enter_value(const char* req, int def)
{
	printf("%s [%d]: ", req, def);
	char buf[4];
	fgets(buf, 3, stdin);
	int x = atoi(buf);
	if (! x)
		return def;
	else
		return x;
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
	int start = enter_value("Start position", 1) - 1;
	if (start < 0 || start >= PHONEBOOK_SIZE)
		start = 0;
	int stop = enter_value("Stop position", 5);
	if (stop > PHONEBOOK_SIZE)
		stop = PHONEBOOK_SIZE;
	if (stop <= start)
		stop = start + 1;
	for (int i = start; i < stop; ++i)
		print_record(i);
}


void menu_delete()
{
	char temp_name[FIELD_SIZE];
	char temp_sname[FIELD_SIZE];
	enter_name(temp_name, temp_sname);
	delete_chel(temp_name, temp_sname);
}


void menu_process(const char c)
{
	switch (c) {
		case 'a': menu_add(); break;
		case 'd': menu_delete(); break;
		case 'f': menu_find(); break;
		case 'p': menu_print(); break;
		default: printf("Unrecognized command\n");
	}
}


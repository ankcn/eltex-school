// Основной модуль телефонного справочника

#include <stdio.h>
#include "phonebook.h"
#include "menu.h"


int main()
{
	// Добавим пару контактов
	add_chel("Ivan", "Petrov", "223-322");
	add_chel("Vasya", "Pupkin", "89977445566");

	char c;
	// Основной цикл: выводим меню, запрашиваем действие, выполняем...
	while ((c = menu_title()) != 'q')
		menu_process(c);

	return 0;
}

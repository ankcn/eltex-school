#include <stdio.h>
#include <stdlib.h>
#include "menu.h"


int main()
{
    printf("Complex numbers calculator\n");
    char c;
	// Основной цикл: выводим меню, запрашиваем действие, выполняем...
	while ((c = menu_title()) != 'q')
		menu_process(c);
    return 0;
}

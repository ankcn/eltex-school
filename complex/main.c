#include <stdio.h>
#include <stdlib.h>
#include "menu.h"
#include "modules.h"


// Глобальная переменная, содержащая количество загруженных модулей
int nmod = 0;


int main()
{
	printf("Complex numbers calculator\n");
	nmod = load_modules(); // Загружаем модули
	if (! nmod) {	// Если ни одного модуля не загружено, то выходим
		printf("No plugins found\n");
		return -1;
	}
	printf("Loaded plugins: %d\n", nmod);

	char c;
	// Основной цикл: выводим меню, запрашиваем действие, выполняем...
	while ((c = menu_title()) != 'q')
		menu_process(c);

	close_modules();
	return 0;
}

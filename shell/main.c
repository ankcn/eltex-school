#include <stdio.h>
#include <stdlib.h>
#include "interface.h"


int main()
{
	// Изменение параметров терминала
	echo_custom();

	// Начало обработки символов со стандартного ввода
	operate();

    return 0;
}

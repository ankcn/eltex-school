#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "utils.h"
#include "buyer.h"
#include "shop.h"
#include "loader.h"


int main()
{
	// Инициализируем генератор случайных чисел
	randomizer_init();

	// Инициализируем магазины
	shops_init();
	printf("Let's shopping!\n");

	// Создаём погрузчик
	loader_init();

	// Инициализируем покупателей, запускаем их потоки и ждём их завершения
	buyers_init_and_wait();
	printf("All buyers satisfied their demands\n");

	// Закрываем поток погрузчика
	loader_finish();

	return 0;
}

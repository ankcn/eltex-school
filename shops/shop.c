#include <pthread.h>
#include <stdio.h>
#include "utils.h"
#include "shop.h"


// Минимальное начальное значение товара в магазине
const int SHOP_INIT_VAL_MIN = 1000;

// Максимальное начальное значение товара в магазине
const int SHOP_INIT_VAL_MAX = 1200;


// Массив магазинов
shop_t shops[SHOPS_NUM];


/*
 * shops_init - Инициализация магазинов
 */
void shops_init()
{
	for (int i = 0; i < SHOPS_NUM; ++i) {
		shops[i].val = random_range(SHOP_INIT_VAL_MIN, SHOP_INIT_VAL_MAX);
		printf("Shop #%d has initial value %d\n", i + 1, shops[i].val);
		pthread_mutex_init(&shops[i].mtx, NULL);
	}
}



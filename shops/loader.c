#include <stdio.h>
#include <pthread.h>
#include "utils.h"
#include "loader.h"
#include "shop.h"


// Время сна погрузчика после загрузки магазина
const unsigned LOADER_SLEEP = 1;

// Минимальный объём товара, разгружаемый погрузчиком в магазине
const int LOADER_SET_MIN = 300;

// Максимальный объём товара, разгружаемый погрузчиком в магазине
const int LOADER_SET_MAX = 500;


// Поток погрузчика
pthread_t loader_thread;


/*
 * loader_work - Работа погрузчика, выполняется в отдельном потоке
 * @par: указатель на входной аргумент, не используется
 */
void* loader_work(void* par)
{
	// Погрузчик работает постоянно, до принудительной отмены потока
	while (1) {
		// Случайным образом выбирается магазин
		size_t n = random_range(0, SHOPS_NUM);
		// Запираем мьютекс для ограничения доступа к этому магазину
		pthread_mutex_lock(&shops[n].mtx);
		// Погрузчик выгружает имеющийся у него товар
		shops[n].val += random_range(LOADER_SET_MIN, LOADER_SET_MAX);
		// Разблокируем доступ к магазину
		pthread_mutex_unlock(&shops[n].mtx);
		printf("Shop #%lu loaded to %d\n", n + 1, shops[n].val);
		// Погрузчик засыпает на определённое время
		delay(LOADER_SLEEP);
	}
	return par;
}

/*
 * loader_init - Создание потока погрузчика
 */
void loader_init()
{
	pthread_create(&loader_thread, NULL, loader_work, NULL);
	pthread_detach(loader_thread);
}

/*
 * loader_finish - Завершение потока погрузчика
 */
void loader_finish()
{
	pthread_cancel(loader_thread);
}


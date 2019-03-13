#include <stdio.h>
#include <pthread.h>
#include "utils.h"
#include "buyer.h"
#include "shop.h"


// Минимальная потребность покупателя в товарах
const int BUYER_DEMAND_MIN = 3000;

// Максимальная потребность покупателя в товарах
const int BUYER_DEMAND_MAX = 3500;

// Время сна покупателя после посещения магазина
const unsigned BUYER_SLEEP = 3;


// Массив покупателей
buyer_t buyers[BUYERS_NUM];


/*
 * buyer_shopping - Действия покупателя, выполняется в отдельном потоке
 * @par: указатель на входной аргумент, интерпретируется как покупатель (buyer_t*)
 */
void* buyer_shopping(void* par)
{
	// Определяем покупателя через полученный аргумент
	buyer_t* b = (buyer_t*) par;
	// Покупатель ходит по магазинам, пока не удовлетворит свою потребность (demand)
	while (b->demand > 0) {
		// Случайным образом выбирается магазин
		size_t sn = random_range(0, SHOPS_NUM);
		// Запираем мьютекс для ограничения доступа к этому магазину
		pthread_mutex_lock(&shops[sn].mtx);
		// Определяем объём товара, который будет куплен: это меньшее из двух
		// значений: наличия в магазине и потребности покупателя
		int amount = (shops[sn].val < b->demand) ? shops[sn].val : b->demand;
		// Уменьшаем товар магазина на объём покупки
		shops[sn].val -= amount;
		// Разблокируем доступ к магазину
		pthread_mutex_unlock(&shops[sn].mtx);
		// Уменьшаем потребность покупателя на объём покупки
		b->demand -= amount;
		printf("Buyer #%lu spent %d at shop #%lu, demand %d\n", b->n, amount, sn + 1, b->demand);
		// Покупатель засыпает на определённое время
		delay(BUYER_SLEEP);
	}
	return par;
}

/*
 * buyers_init_and_wait - Инициализация покупателей, создание и ожидание завершения их потоков
 */
void buyers_init_and_wait()
{
	// Инициализация покупателей и создание их потоков
	for (int i = 0; i < BUYERS_NUM; ++i) {
		buyers[i].demand = random_range(BUYER_DEMAND_MIN, BUYER_DEMAND_MAX);
		buyers[i].n = i + 1;
		printf("Buyer #%d has initial demand %d\n", i + 1, buyers[i].demand);
		pthread_create(&buyers[i].thread, NULL, buyer_shopping, (void*) &buyers[i]);
	}
	// Присоединение потоков покупателей
	for (int i = 0; i < BUYERS_NUM; ++i)
		pthread_join(buyers[i].thread, NULL);
}



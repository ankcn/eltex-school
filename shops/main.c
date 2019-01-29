#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>


// Количество магазинов
#define SHOPS_NUM	5

// Количество покупателей
#define BUYERS_NUM	3

// Время сна покупателя после посещения магазина
#define BUYER_SLEEP	3

// Время сна погрузчика после загрузки магазина
#define LOADER_SLEEP	1

// Минимальное начальное значение товара в магазине
#define SHOP_INIT_VAL_MIN	1000

// Максимальное начальное значение товара в магазине
#define SHOP_INIT_VAL_MAX	1200

// Минимальный объём товара, разгружаемый погрузчиком в магазине
#define LOADER_SET_MIN	300

// Максимальный объём товара, разгружаемый погрузчиком в магазине
#define LOADER_SET_MAX	500

// Минимальная потребность покупателя в товарах
#define BUYER_DEMAND_MIN	3000

// Максимальная потребность покупателя в товарах
#define BUYER_DEMAND_MAX	3500


// Тип структуры для описания покупателя
typedef struct {
	int demand;	// Потребность в товарах
	size_t n;	// Порядковый номер покупателя
	pthread_t thread;	// Поток
} buyer_t;


// Структура для описания магазина
struct shop_t {
	int val; // Объём товара в наличии
	pthread_mutex_t mtx;	// Мьютекс для блокирования доступа
} shops[SHOPS_NUM]; // Массив магазинов

// Массив покупателей
buyer_t buyers[BUYERS_NUM];

// Мьютекс для разграничения доступа к ресурсам рандомайзера
pthread_mutex_t rnd_mtx = PTHREAD_MUTEX_INITIALIZER;


/* random_range - Получение случайного числа в заданном диапазоне
 * @min: нижний предел диапазона
 * @max: верхний предел диапазона, с исключением
 */
int random_range(int min, int max)
{
	pthread_mutex_lock(&rnd_mtx);
	int r = (rand() % (max - min)) + min;
	pthread_mutex_unlock(&rnd_mtx);
	return r;
}

/*
 * delay - Пауза на заданное количество секунд
 * @seconds: количество секунд, на которое надо взять паузу
 */
void delay(unsigned int seconds)
{
    unsigned int wake_at = time(0) + seconds;
    while (time(0) < wake_at);
}

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


int main()
{
	// Инициализация генератора случайных чисел
	srand(time(NULL));

	// Инициализация магазинов
	for (int i = 0; i < SHOPS_NUM; ++i) {
		shops[i].val = random_range(SHOP_INIT_VAL_MIN, SHOP_INIT_VAL_MAX);
		printf("Shop #%d has initial value %d\n", i + 1, shops[i].val);
		pthread_mutex_init(&shops[i].mtx, NULL);
	}

	printf("Let's shopping!\n");

	// Создание потока погрузчика
	pthread_t loader_thread;
	pthread_create(&loader_thread, NULL, loader_work, NULL);
	pthread_detach(loader_thread);

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

	printf("All buyers satisfied their demands\n");

	// Завершение потока погрузчика
	pthread_cancel(loader_thread);
	return 0;
}

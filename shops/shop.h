#ifndef SHOP_H_INCLUDED
#define SHOP_H_INCLUDED


// Количество магазинов
#define SHOPS_NUM	5


// Структура для описания магазина
typedef struct {
	int val; // Объём товара в наличии
	pthread_mutex_t mtx;	// Мьютекс для блокирования доступа
} shop_t;


// Массив магазинов
extern shop_t shops[];


void shops_init();


#endif // SHOP_H_INCLUDED

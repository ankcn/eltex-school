#ifndef BUYER_H_INCLUDED
#define BUYER_H_INCLUDED


// Количество покупателей
#define BUYERS_NUM	3


// Тип структуры для описания покупателя
typedef struct {
	int demand;	// Потребность в товарах
	size_t n;	// Порядковый номер покупателя
	pthread_t thread;	// Поток
} buyer_t;


void buyers_init_and_wait();


#endif // BUYER_H_INCLUDED

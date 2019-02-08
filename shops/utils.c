#include <stdlib.h>
#include <pthread.h>
#include <time.h>


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
 * randomizer_init - Инициализация генератора случайных чисел
 */
void randomizer_init()
{
	srand(time(NULL));
}



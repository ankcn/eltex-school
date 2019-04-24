#include <time.h>
#include <stdio.h>
#include "timestamp.h"


// Переменная для хранения времени
time_t rawtime;

// Строковый буфер для представления даты и времени в текстовом виде
char buffer [TIME_BUF_SIZE];


 /*
 * get_raw_time - Получить текущее время
 * @return: возвращает указатель на струтуру, отображающую текущее время
 */
struct tm* get_raw_time()
{
    time(&rawtime);
    return localtime(&rawtime);
}

 /*
 * get_date_str - Получение текстового представления текущей даты и времени
 * @return: возвращает строку с датой и временем
 */
char* get_date_str()
{
	strftime(buffer, TIME_BUF_SIZE, "%d.%m.%Y, %T", get_raw_time());
	return buffer;
}

 /*
 * print_date - Вывод даты и времени
 */
void print_date()
{
    puts(get_date_str());
}



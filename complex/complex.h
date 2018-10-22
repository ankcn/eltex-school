#ifndef COMPLEX_H_INCLUDED
#define COMPLEX_H_INCLUDED


// Тип структуры комплексного числа
typedef struct {
	float re;	// Вещественная часть
	float im;	// Мнимая часть
} complex;


// Вычисление суммы двух комплексных чисел
complex compl_plus(const complex a, const complex b);

// Вычисление разности двух комплексных чисел
complex compl_minus(const complex a, const complex b);

// Умножение двух комплексных чисел
complex compl_mult(const complex a, const complex b);

// Деление двух комплексных чисел
complex compl_divide(const complex a, const complex b);

// Печать комплексного числа
void compl_print(const complex a);


#endif // COMPLEX_H_INCLUDED

#ifndef COMPLEX_H_INCLUDED
#define COMPLEX_H_INCLUDED

// Вспомогательные макросы для представления имени переменной в виде строки 
#define STRINGIFY(x)		#x
#define PRE_STRINGIFY(x)	STRINGIFY(x)
#define INFO_VAR_STR    PRE_STRINGIFY(INFO_VAR)

// Прототип функции - операции с комплексными числами
#define FUNC_PROTO(n)		complex (*n)(complex, complex)

// предельная длинна строки с именем операции/функции
#define OP_NAME_LENGTH	15

// Имя переменной, содержащей струтуру с информацией о модуле
#define INFO_VAR		mod_info


// Тип структуры комплексного числа
typedef struct {
	float re;	// Вещественная часть
	float im;	// Мнимая часть
} complex;

// Тип структуры, описывающей параметры модуля (плагина)
typedef struct {
	char fname[OP_NAME_LENGTH];	// Имя функции
	char opname[OP_NAME_LENGTH];	// Название операции
	char opsym;			// Символ операции
} mod_info_t;


#endif // COMPLEX_H_INCLUDED

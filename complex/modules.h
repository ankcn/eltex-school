#ifndef MODULES_H_INCLUDED
#define MODULES_H_INCLUDED

// Директория с плагинами
#define MODULES_DIR	"plugins"

// Предельное количество плагинов
#define MAX_MODULES	9

// Предельная длинна пути, содержащего имя файла плагина
#define PATH_LENGTH	222


// Структура с параметрами модуля (плагина) 
typedef struct {
	FUNC_PROTO(op_func);		// Указатель на функцию в данном модуле
	void* mod_hnd;			// Указатель на сам модуль для динамической загрузки
	char op_name[OP_NAME_LENGTH];	// Название операции, предоставляемой модулем
	char op_symbol;			// Символ операции
} operation;

// Объявление внешней переменной - массива модулей
// только там, где нет оригинала
#ifndef OPS_ARE_HERE
extern operation ops[MAX_MODULES];
#endif // OPS_ARE_HERE

// Загрузка внешних модулей
int load_modules();

// Закрытие открытых модулей
void close_modules();


#endif // MODULES_H_INCLUDED

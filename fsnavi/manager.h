#ifndef MANAGER_H_INCLUDED
#define MANAGER_H_INCLUDED

#include <time.h>


// Количество файлов, под которое выделяется дополнительная память
#define FILES_PACK	1000

// Максимальная длина строки, представляющей дату и время
#define TIME_STR_LEN	44


// Тип структуры для описания файла
typedef struct {
	char* name;	// Полный путь к файлу
	struct tm mtime;	// Время изменения файла
	ssize_t size;	// Размер файла
	int is_dir;	// Признак директории
	unsigned int mode_bits, uid, gid;	// Владелец, группа и права доступа
} file_info;

// Тип структуры, описывающий содержимое директории, открытой в панели менеджера файлов
typedef struct {
	ssize_t count;	// Количество файлов в директории
	ssize_t space;	// Количество файлов, под которое выделена память
	ssize_t select;	// Порядковый номер выбранного файла
	file_info* files;	// Массив "файлов"
} file_panel;


// Просмотр содержимого директории
int scan_dir(const char* path);

// Высвобождение ресурсов
void clean_up();

// Нарисовать панель
void print_panel();

// Сортировка файлов для отображения на панели
void sort_panel();


#endif // MANAGER_H_INCLUDED

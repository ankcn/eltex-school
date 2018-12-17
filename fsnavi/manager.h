#ifndef MANAGER_H_INCLUDED
#define MANAGER_H_INCLUDED

#include <time.h>
#include <ncurses.h>
#include <limits.h>


// Количество файлов, под которое выделяется дополнительная память
#define FILES_PACK	1000

// Максимальная длина строки, представляющей дату и время
#define TIME_STR_LEN	44

#define INNER_OFFSET	2
#define ATTR_LENGTH	28
#define SIZE_LENGTH	7

// Макрос для получения кода сочетания клавиш CTRL + (k)
#define WCTRL(k)	(k - 0100)

#define MIN_PATH	3


// Варианты панелей: левая и правая
typedef enum { P_LEFT, P_RIGHT } side_t;

// Тип структуры для описания файла
typedef struct {
	char* name;	// Имя файла
	struct tm mtime;	// Время изменения файла
	ssize_t size;	// Размер файла
	bool is_dir;	// Признак директории
	unsigned int mode_bits, uid, gid;	// Владелец, группа и права доступа
} file_info;

// Тип структуры, описывающий содержимое директории, открытой в панели менеджера файлов
typedef struct {
	const side_t side;
	ssize_t count;	// Количество файлов в директории
	ssize_t space;	// Количество файлов, под которое выделена память
	ssize_t select;	// Порядковый номер выбранного файла
	file_info* files;	// Массив "файлов"
	WINDOW* wnd;	// Окно ncurses для данной панели
	bool highlight;	// Необходимость подсветки выбранного файла
	char path[PATH_MAX];	// Полный путь к текущей директории
	ssize_t start;
} file_panel;


// Просмотр содержимого директории
int scan_dir(const char* path);

// Высвобождение ресурсов
void clean_up();

// Нарисовать панель
void list_files();

// Сортировка файлов для отображения на панели
void sort_panel();

// Инициализация и определение параметров экрана ncurses
void prepare();

// Получить код символа/клавиши
int get_key();

// Отрисовка панели на экране терминала
void draw_panel();

// Переключиться на другую панель
void switch_panel();

// Сменить директорию на выбранную
void change_dir();

// Получить полный путь к файлу из его имени и текущей директории
void full_path(char* buf, const char* fname);

// Получить родительский каталог для заданного пути
void parent_dir(char* buf, const char* path);


int max_lines();

// Подняться вверх по списку файлов
void move_up(int num);

// Спустится вниз по спику файлов
void move_down(int num);


#endif // MANAGER_H_INCLUDED

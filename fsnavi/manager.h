#ifndef MANAGER_H_INCLUDED
#define MANAGER_H_INCLUDED

#include <time.h>
#include <ncurses.h>
#include <limits.h>


// Количество файлов, под которое выделяется дополнительная память
#define FILES_PACK	1000

// Максимальная длина строки, представляющей дату и время
#define TIME_STR_LEN	21

// Отступ от края внутри панели
#define INNER_OFFSET	2

// Длина поля под размер файла
#define SIZE_LENGTH	7

// Макрос для получения кода сочетания клавиш CTRL + (k)
#define WCTRL(k)	(k - 0100)

// Как найти редактор файлов
#define EDITOR_REL_PATH	"/../myedit/myedit"
#define EDITOR_FIRST_ARGUMENT	"myedit"

// Объём данных в начале файла для анализа
#define TEXT_SIZE_FOR_ANALYSE	2222


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
	const side_t side;	// Сторона
	ssize_t count;	// Количество файлов в директории
	ssize_t space;	// Количество файлов, под которое выделена память
	ssize_t select;	// Порядковый номер выбранного файла
	file_info* files;	// Массив "файлов"
	WINDOW* wnd;	// Окно ncurses для данной панели
	bool highlight;	// Необходимость подсветки выбранного файла
	char path[PATH_MAX];	// Полный путь к текущей директории
	ssize_t start;	// Номер файла, с которого начинать отображение
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

// Сменить директорию на заданную
void change_dir(const char* dirname);

// Получить полный путь к файлу из его имени и текущей директории
void full_path(char* buf, const char* fname);

// Получить родительский и текущий каталог от заданного полного пути
void parent_dir(char* par, char* cur, const char* path);

// Определить максимальное количество файлов, которые можно отобразить одновременно
int max_lines();

// Подняться вверх по списку файлов на заданное количество позиций
void move_up(int num);

// Спустится вниз по спику файлов на заданное количество позиций
void move_down(int num);

// Проверка, не является ли текущая директория корневой
bool is_root();

// Перейти наверх списка файлов
void go_top();

// Перейти на последний файл в директории
void go_end();

// Переотрисовка экрана после изменения размера терминала
void new_size();

// Обработка нажатия клавиши Enter
void enter();

// Запуск редактора файлов
int start_editor(const char* fname);

// Перевести экран терминала в режим curses
void switch_to_curses_mode();


#endif // MANAGER_H_INCLUDED

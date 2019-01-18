#ifndef MANAGER_H_INCLUDED
#define MANAGER_H_INCLUDED

#include <time.h>
#include <ncurses.h>
#include <limits.h>
#include <sys/stat.h>


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
#define EDITOR_REL_PATH	"/../../myedit/bin/myedit"
#define EDITOR_FIRST_ARGUMENT	"myedit"

// Объём данных в начале файла для анализа
#define TEXT_SIZE_FOR_ANALYSE	2222

// Размер блока данных для копирования
#define COPY_BLOCK_SIZE	0x100000


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

// Тип структуры, описывающей содержимое директории, открытой в панели менеджера файлов
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

// Структура с параметрами для задачи копирования
typedef struct {
	char source[FILENAME_MAX];	// Полный путь источника
	char destination[FILENAME_MAX];	// Полный путь назначения
	struct stat attr;	// Аттрибуты файла
} copy_params;


// Высвобождение ресурсов
void clean_up();

// Инициализация и определение параметров экрана ncurses
void prepare(const char* dir);

// Получить код символа/клавиши
int get_key();

// Отрисовка панели на экране терминала
void draw_panel();

// Переключиться на другую панель
void switch_panel();

// Сменить директорию на заданную
void change_dir(const char* dirname);

// Определить максимальное количество файлов, которые можно отобразить одновременно
int max_lines();

// Подняться вверх по списку файлов на заданное количество позиций
void move_up(size_t num);

// Спустится вниз по спику файлов на заданное количество позиций
void move_down(size_t num);

// Перейти наверх списка файлов
void go_top();

// Перейти на последний файл в директории
void go_end();

// Переотрисовка экрана после изменения размера терминала
void redraw();

// Обработка нажатия клавиши Enter
void enter();

void handle_copy_key();


#ifdef MONITOR_C

// Отобразить список файлов в текущей панели
static void list_files();

// Просмотр содержимого директории
static int scan_dir(const char* path);

// Получить полный путь к файлу из его имени и текущей директории
inline static void full_path(char* buf, const char* fname);

// Получить родительский и текущий каталог от заданного полного пути
static void parent_dir(char* par, char* cur, const char* path);

// Проверка, не является ли текущая директория корневой
static bool is_root();

// Сортировка файлов, обнаруженных в директории
inline static void sort_panel();

// Запуск внешнего редактора файлов
static int start_editor(const char* fname);

// Перевести экран терминала в режим curses
static void switch_to_curses_mode();

// Освобождение памяти, выделенной под имена файлов
static void free_names(file_panel* pnl);

// Добавление информации о файле в список
static void add_file(const char* fname);

// Представление размера файла в коротком виде с множителем (кило, Мега и т.п.)
static void  size_short(char* buf, size_t x);

// Функция сравнения имён файлов
static int cmp_adapter(const void* a, const void* b);

static file_info* selected_file();

static file_panel* get_other_panel();

static void* copy_file(void* val);

static void copy_dir(const copy_params* prm);

static void print_status(const char* msg);

static void* show_progress(void* par);

static void check_path(char* path);

static void get_fileinfo(file_info* fi, const char* fname);

inline static void copy_item(const copy_params* prm);

static void name_with_path(char* buf, const char* path, const char* name);

static void fill_copy_params(copy_params* prm, const char* src_dir, const char* dst_dir, const char* name);


#endif // MONITOR_C


#endif // MANAGER_H_INCLUDED


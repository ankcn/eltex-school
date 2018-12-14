#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "manager.h"


// Левая и правая паенели файлового менеджера
file_panel lpanel = { P_LEFT, 0, 0, 0 }, rpanel = { P_RIGHT, 0, 0, 0 };

// Текущая (активная) панель
file_panel* cp = &rpanel;


// Освобождение памяти
void free_names(file_panel* pnl)
{
	for (size_t i = 0; i < pnl->count; ++i)
		free(pnl->files[i].name);
	pnl->count = 0;
	free(pnl->files);
	pnl->space = 0;
}

// Добавление информации о файле в список
void add_file(const char* fname)
{
	// Текущий каталог не берём
	if(! strcmp(fname, "."))
		return;

	// Проверка свободного места в массиве
	if (cp->count >= cp->space) {
		cp->files = realloc(cp->files, (cp->space + FILES_PACK) * sizeof(file_info));
		if (cp->files == NULL)
			return;
		cp->space += FILES_PACK;
	}

	// Указатель на следующий (свободный элемент массива)
	// куда будем помещать информацию о новом файле
	file_info* fi = &(cp->files[cp->count++]);

	// Выделяем память под имя файла
	size_t len = strlen(fname);
	char* slot = realloc(fi->name, len);
	if (slot == NULL)
		return;

	// Копируем имя нового файла в подготовленный буфер
	strcpy(slot, fname);
	fi->name = slot;

	// Формируем полный путь к файлу
	slot = malloc(strlen(fname) + strlen(cp->path) + 1);
	full_path(slot, fname);

	// Получаем атрибуты файла: режим доступа, владелец, размер, дата и пр.
	struct stat attributes;
	stat(slot, &attributes);
	fi->mtime = *(localtime(&attributes.st_ctime));
	fi->size = attributes.st_size;
	fi->is_dir = S_ISDIR(attributes.st_mode);
	fi->mode_bits = attributes.st_mode;
	fi->uid = attributes.st_uid;
	fi->gid = attributes.st_gid;
	free(slot);
}


int scan_dir(const char* path)
{
	struct dirent* fentry;
	DIR* dir = opendir(path);

	if (! dir)
		return 1;

	cp->count = 0;
	cp->select = 0;

	// Когда успешно открыли новый каталог, делаем его путь текущим
	strcpy(cp->path, path);

	// Если в конце пути нет косой черты, добавляем её
	if (cp->path[strlen(cp->path) - 1] != '/')
		strcat(cp->path, "/");

	// Обходим содержимое директории, добавляем найденные файлы в коллекцию
	while ((fentry = readdir(dir)))
		add_file(fentry->d_name);
	closedir(dir);

	// Сортируем по алфавиту всё, что нашли
	sort_panel();

	return 0;
}


void clean_up()
{
	endwin();
	free_names(&lpanel);
	free_names(&rpanel);
}

// Представление размера файла в коротком виде с суффиксом (кило, Мега и т.п.)
void  size_short(char* buf, ssize_t x)
{
	char m = ' ';
	if (x > 1L << 40)
		x >>= 40, m = 'T';
	else if (x > 1 << 30)
		x >>= 30, m = 'G';
	else if (x > 1L << 20)
		x >>= 20, m = 'M';
	else if (x > 9999)
		x >>= 10, m = 'K';
	sprintf(buf, "%ld%c", x, m);
}

// Вывод на экран списка файлов текущего каталога
void list_files()
{
	for (size_t i = 0; i < cp->count; ++i) {
		file_info* fi = &(cp->files[i]);
		if (i == cp->select && cp->highlight)
			wattron(cp->wnd, A_REVERSE);
		mvwprintw(cp->wnd, INNER_OFFSET + i, INNER_OFFSET, "%c%s", fi->is_dir ? '/' : ' ', fi->name);
		char buf[TIME_STR_LEN];
		size_short(buf, fi->size);
		size_t attr_pos = getmaxx(cp->wnd) - ATTR_LENGTH;
		mvwaddstr(cp->wnd, INNER_OFFSET + i, attr_pos, buf);
		strftime(buf, TIME_STR_LEN, "%d.%m.%Y %T", &(fi->mtime));
		mvwaddstr(cp->wnd, INNER_OFFSET + i, attr_pos + SIZE_LENGTH, buf);
		if (i == cp->select)
			wattroff(cp->wnd, A_REVERSE);
	}
	wrefresh(cp->wnd);
}

/*
Функция - адаптер к библитечной функции сравнения строк strcmp
для передачи в качестве параметра в функию сортировки qsort
При этом вводится дополнительный критерий сравнения, разделяющий
файлы и директории
*/
int cmp_adapter(const void* a, const void* b)
{
	file_info* fa = (file_info*) a;
	file_info* fb = (file_info*) b;
	if (fa->is_dir && ! fb->is_dir)
		return -1;
	else if	(! fa->is_dir && fb->is_dir)
		return 1;
	else
		return strcmp(fa->name, fb->name);
}


void sort_panel()
{
	qsort(cp->files, cp->count, sizeof(file_info), cmp_adapter);
}


void get_down()
{
	if (cp->select < cp->count - 1) {
		++cp->select;
		list_files();
	}
}


void get_up()
{
	if (cp->select) {
		--cp->select;
		list_files();
	}
}


void prepare()
{
	initscr();
	noecho();
	curs_set(FALSE);
	keypad(stdscr, TRUE);
	cbreak();
	refresh();
}


int get_key()
{
	int a = getch();
	return (a == WCTRL('X') || a == KEY_F(10)) ? 0 : a;
}


void draw_panel()
{
	if (cp->wnd)
		delwin(cp->wnd);

	size_t mx, my, wl;
	getmaxyx(stdscr, my, mx);
	wl = mx >> 1;
	cp->wnd = newwin(0, wl, 0, (cp->side == P_LEFT) ? 0 : wl);

	wborder(cp->wnd, '|', '|', '-', '-', '-', '-', '-', '-');
	mvwprintw(cp->wnd, 0, INNER_OFFSET, "[%s]", cp->path);
	mvwprintw(cp->wnd, INNER_OFFSET - 1, INNER_OFFSET + 1, "File name");
	mvwprintw(cp->wnd, INNER_OFFSET - 1, wl - ATTR_LENGTH, "Size");
	mvwprintw(cp->wnd, INNER_OFFSET - 1, wl - ATTR_LENGTH + SIZE_LENGTH, "Last change");
	mvwvline(cp->wnd, 1, wl - ATTR_LENGTH - INNER_OFFSET, '|', my - 2);
	mvwvline(cp->wnd, 1, wl - ATTR_LENGTH - INNER_OFFSET + SIZE_LENGTH, '|', my - 2);
	wrefresh(cp->wnd);
	list_files();
}


void switch_panel()
{
	// Сбрасываем подсветку выбранного файла перед сменой панели
	cp->highlight = FALSE;
	list_files();
	if (cp->side == P_LEFT)
		cp = &rpanel;
	else
		cp = &lpanel;
	// На панели, в которую перешли, включаем подсветку выбранного файла
	cp->highlight = TRUE;
	list_files();
}


void change_dir()
{
	// Указатель на выбранный файл
	file_info* fi = &cp->files[cp->select];
	if(! fi->is_dir)
		return;

	char name[FILENAME_MAX];
	if (cp->select)
		full_path(name, fi->name);
	else if (strlen(cp->path) < MIN_PATH)
		return;
	else
		parent_dir(name, cp->path);

	if (! scan_dir(name))
		draw_panel();
}


void full_path(char* buf, const char* fname)
{
	strcpy(buf, cp->path);
	strcat(buf, fname);
}


void parent_dir(char* buf, const char* path)
{
	bool parent = FALSE;
	for (int i = strlen(path) - 2; i >= 0; --i) {
		if(! parent && path[i] == '/') {
			parent = TRUE;
			buf[i + 1] = 0;
		}
		if (parent)
			buf[i] = path[i];
	}
}

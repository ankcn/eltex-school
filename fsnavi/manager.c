#define MONITOR_C

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include "manager.h"


// Левая и правая паенели файлового менеджера
static file_panel lpanel = { P_LEFT, 0, 0, 0 }, rpanel = { P_RIGHT, 0, 0, 0 };

// Текущая (активная) панель
static file_panel* cp = &rpanel;

// Полный путь к редактору файлов
static char editor_path[PATH_MAX];


// Освобождение памяти
static void free_names(file_panel* pnl)
{
	for (size_t i = 0; i < pnl->count; ++i)
		free(pnl->files[i].name);
	pnl->count = 0;
	free(pnl->files);
	pnl->space = 0;
	delwin(pnl->wnd);
}

// Добавление информации о файле в список
static void add_file(const char* fname)
{
	// Текущий каталог и родительский для root не берём
	if((! strcmp(fname, ".")) || (is_root() && ! strcmp(fname, "..")))
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
	size_t len = strlen(fname) + 1;
	char* slot = realloc(fi->name, len);
	if (slot == NULL)
		return;

	// Копируем имя нового файла в подготовленный буфер
	strcpy(slot, fname);
	fi->name = slot;

	// Формируем полный путь к файлу
	slot = malloc(len + strlen(cp->path));
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


static int scan_dir(const char* path)
{
	struct dirent* fentry;
	DIR* dir = opendir(path);

	if (! dir)
		return 1;

	cp->count = 0;
	cp->select = 0;
	cp->start = 0;

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

// Представление размера файла в коротком виде с множителем (кило, Мега и т.п.)
static void  size_short(char* buf, ssize_t x)
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
	for (size_t i = 0; i < cp->count && i < max_lines(); ++i) {
		file_info* fi = &(cp->files[i + cp->start]);
		bool hl = (i + cp->start == cp->select) && cp->highlight;
		if (hl)
			wattron(cp->wnd, A_REVERSE);
		mvwprintw(cp->wnd, INNER_OFFSET + i, INNER_OFFSET, "%c", fi->is_dir ? '/' : ' ');
		size_t attr_pos = getmaxx(cp->wnd) - TIME_STR_LEN - SIZE_LENGTH;
		waddnstr(cp->wnd, fi->name, attr_pos - INNER_OFFSET * 3);
		char buf[TIME_STR_LEN];
		size_short(buf, fi->size);
		mvwaddstr(cp->wnd, INNER_OFFSET + i, attr_pos, buf);
		strftime(buf, TIME_STR_LEN, "%d.%m.%Y %T", &(fi->mtime));
		mvwaddstr(cp->wnd, INNER_OFFSET + i, attr_pos + SIZE_LENGTH, buf);
		if (hl)
			wattroff(cp->wnd, A_REVERSE);
	}
	wrefresh(cp->wnd);
}

/*
Функция - адаптер к библиотечной функции сравнения строк strcmp
для передачи в качестве параметра в функцию сортировки qsort
При этом вводится дополнительный критерий сравнения, разделяющий
файлы и директории
*/
static int cmp_adapter(const void* a, const void* b)
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


static void sort_panel()
{
	qsort(cp->files, cp->count, sizeof(file_info), cmp_adapter);
}


int max_lines()
{
	return getmaxy(cp->wnd) - INNER_OFFSET - 1;
}


void move_up(int num)
{
	if (! cp->select)
		return;

	if ((cp->select -= num) < 0)
		cp->select = 0;

	if (cp->start > cp->select) {
		cp->start = cp->select;
		draw_panel();
	}
	else
		list_files();
}


void move_down(int num)
{
	if (cp->select == cp->count - 1)
		return;

	if ((cp->select += num) >= cp->count)
		cp->select = cp->count - 1;

	if (cp->select - cp->start >= max_lines()) {
		cp->start = cp->select - max_lines() + 1;
		draw_panel();
	}
	else
		list_files();
}


void prepare()
{
	switch_to_curses_mode();

	// Выясняем текущую директорию и отображаем её содержимое
	char path[PATH_MAX];
	getcwd(path, PATH_MAX);
	scan_dir(path);
	draw_panel();
	switch_panel();
	scan_dir(path);
	draw_panel();

/*
Берём полный путь к исполняемому файлу данной программы, то есть менеджера файлов,
и, полагая, что файловый редактор находится в соседней директории, формируем
путь для запуска редактора
*/
	readlink("/proc/self/exe", path, PATH_MAX);
	parent_dir(editor_path, NULL, path);
	strcat(editor_path, EDITOR_REL_PATH);
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
	mvwprintw(cp->wnd, INNER_OFFSET - 1, wl - TIME_STR_LEN - SIZE_LENGTH, "Size");
	mvwprintw(cp->wnd, INNER_OFFSET - 1, wl - TIME_STR_LEN, "Last change");
	mvwvline(cp->wnd, 1, wl - TIME_STR_LEN - SIZE_LENGTH - INNER_OFFSET, '|', my - 2);
	mvwvline(cp->wnd, 1, wl - TIME_STR_LEN - INNER_OFFSET, '|', my - 2);
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


void change_dir(const char* dirname)
{
	char path[PATH_MAX];
	char current[FILENAME_MAX];
	current[0] = 0;
	if (cp->select || is_root())
		// Если заходим в подкаталог
		full_path(path, dirname);
	else
		// А иначе - выходим в родительский
		parent_dir(path, current, cp->path);

	if (! scan_dir(path)) {
/*
Текущий каталог current у нас определён, когда мы выходим в родительский.
В этом случае перебираем его содержимое в поисках директории, которую
только что покинули. Её номер присваиваем выбранному файлу.
*/
		if (*current)
			for (int i = 0; i < cp->count && ! cp->select; ++i)
				if (! strcmp(current, cp->files[i].name))
					cp->select = i;
		draw_panel();
	}
}


static void full_path(char* buf, const char* fname)
{
	strcpy(buf, cp->path);
	strcat(buf, fname);
}


static void parent_dir(char* par, char* cur, const char* path)
{
	bool parent = FALSE;
	int e = 0;
	for (int i = strlen(path) - 2; i >= 0; --i) {
/*
Идём с конца строки, содержащей полный путь, ищем косую черту,
которую считаем разделителем между именем текущего каталога и
полным путём к родительскому.
После того, как нашли - запоминаем позицию и копируем остаток.
Получили полный путь родителя в par[].
*/
		if (parent)
			par[i] = path[i];
		else if(path[i] == '/') {
			parent = TRUE;
			par[e = i] = '/';
			par[i + 1] = 0;
		}
	}
/*
Используя найденную ранее позицю разделителя, копируем имя
текущей директории в cur[].
*/
	if (cur) {
		while((*cur++ = path[++e]));
		*(cur - 2) = 0;	// Затираем косую черту в конце имени
	}
}


static bool is_root()
{
	return (bool) ! strcmp(cp->path, "/");
}


void go_top()
{
	cp->select = 0;
	cp->start = 0;
	draw_panel();
}


void go_end()
{
	move_down(cp->count);
}


void new_size()
{
	draw_panel();
	switch_panel();
	draw_panel();
	switch_panel();
}


void enter()
{
	// Указатель на выбранный файл
	file_info* fi = &cp->files[cp->select];
	if (fi->is_dir)
		// Если выбрана директория, то переходим в неё
		change_dir(fi->name);
	else {
/*
В противном случае полагаем, что выбран обычный файл и пытаемся его открыть
и прочиать некоторое количество байт. Если попались только ASCII символы,
то считаем файл текстовым и открываем его в редакторе.
*/
		char path[PATH_MAX];
		full_path(path, fi->name);
		FILE* tf = fopen(path, "r");
		if (tf) {
			bool is_text = TRUE;
			int i = 0, c = 0;
			while ((c = fgetc(tf)) != EOF && ++i < TEXT_SIZE_FOR_ANALYSE)
				is_text &= (c < 0x80);
			fclose(tf);
			if (is_text)
				start_editor(path);
		}
	}
}


static int start_editor(const char* fname)
{
	// Создаём копию процесса
	pid_t pid = fork();
	if (pid < 0)
		return 1;
	else if (pid > 0) {
/*
В родительском процессе восстанавливаем обычный режим экрана,
ждём завершения работы дочернего процесса, после чего снова
переключаем экран терминала в режим curses и отрисовываем интерфейс.
*/
		endwin();
		int st;
		pid = wait(&st);
		switch_to_curses_mode();
		new_size();
	} else
		// В дочернем процессе запускаем файловый редактор
		if (execl(editor_path, EDITOR_FIRST_ARGUMENT, fname, NULL) == -1)
			exit(EXIT_FAILURE);
	return 0;
}


static void switch_to_curses_mode()
{
	initscr();
	noecho();
	curs_set(FALSE);
	keypad(stdscr, TRUE);
	cbreak();
	refresh();
}

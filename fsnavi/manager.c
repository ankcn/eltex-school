#define MONITOR_C

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "manager.h"


// Левая и правая паенели файлового менеджера
static file_panel lpanel = { P_LEFT, 0, 0, 0 }, rpanel = { P_RIGHT, 0, 0, 0 };

// Текущая (активная) панель
static file_panel* cp = &rpanel;

// Полный путь к редактору файлов
static char editor_path[PATH_MAX];

//
static WINDOW* statusbar;

//
static float progress;


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

	// Получаем атрибуты файла: режим доступа, владелец, размер, дата и пр.
	struct stat attributes;
	stat(fname, &attributes);
	fi->mtime = *(localtime(&attributes.st_ctime));
	fi->size = attributes.st_size;
	fi->is_dir = S_ISDIR(attributes.st_mode);
	fi->mode_bits = attributes.st_mode;
	fi->uid = attributes.st_uid;
	fi->gid = attributes.st_gid;
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

	//
	chdir (cp->path);

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
	if (statusbar)
		delwin(statusbar);
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
	print_status(" ");
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
	switch_panel();
	scan_dir(path);
	redraw();

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

	size_t mx, my, wl, wh;
	getmaxyx(stdscr, my, mx);
	wl = mx >> 1;
	wh = my - 1;
	cp->wnd = newwin(wh, wl, 0, (cp->side == P_LEFT) ? 0 : wl);

	wborder(cp->wnd, '|', '|', '-', '-', '-', '-', '-', '-');
	mvwprintw(cp->wnd, 0, INNER_OFFSET, "[%s]", cp->path);
	mvwprintw(cp->wnd, INNER_OFFSET - 1, INNER_OFFSET + 1, "File name");
	mvwprintw(cp->wnd, INNER_OFFSET - 1, wl - TIME_STR_LEN - SIZE_LENGTH, "Size");
	mvwprintw(cp->wnd, INNER_OFFSET - 1, wl - TIME_STR_LEN, "Last change");
	mvwvline(cp->wnd, 1, wl - TIME_STR_LEN - SIZE_LENGTH - INNER_OFFSET, '|', wh - 2);
	mvwvline(cp->wnd, 1, wl - TIME_STR_LEN - INNER_OFFSET, '|', wh - 2);
	wrefresh(cp->wnd);
	list_files();
}


void switch_panel()
{
	// Сбрасываем подсветку выбранного файла перед сменой панели
	cp->highlight = FALSE;
	list_files();
	cp = get_other_panel();

	// На панели, в которую перешли, включаем подсветку выбранного файла
	cp->highlight = TRUE;
	chdir(cp->path);
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
	else
		print_status("Error opening directory");
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


void redraw()
{
	draw_panel();
	switch_panel();
	draw_panel();
	switch_panel();

	if (statusbar)
		delwin(statusbar);
	statusbar = newwin(0, 0, getmaxy(stdscr) - 1, 0);
}


static void print_status(const char* msg)
{
	werase(statusbar);
	mvwaddstr(statusbar, 0, 1, msg);
	wrefresh(statusbar);
}


static file_info* selected_file()
{
	return &cp->files[cp->select];
}


void enter()
{
	// Указатель на выбранный файл
	file_info* fi = selected_file();
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
			else
				print_status("Selected file can't be opened in editor");
		}
		else
			print_status("Can't access this file");
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
		redraw();
		if (st)
			print_status("Failure exit from editor");
		else
			print_status("Normal exit from editor");
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


static void copy_file(const char* fname, const char* dest)
{
	int src_fd, dst_fd;
	src_fd = open(fname, O_RDONLY);
	if (src_fd <= 0) {
		print_status("Error opening source file");
		return;
	}

	dst_fd = open(dest, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (dst_fd <= 0) {
		close(src_fd);
		print_status("Error opening destination file");
		return;
	}

	// Определение размера файла
	long fsize = (long) lseek(src_fd, 0, SEEK_END);
	if (fsize < 0) {
		close(src_fd);
		print_status("Error gathering file size");
		return;
	}

	progress = 0;
	char buf[COPY_BLOCK_SIZE];
	ssize_t sz, readed = 0;
	if (fsize) while ((sz = read(src_fd, buf, COPY_BLOCK_SIZE))) {
		progress = (readed += sz) / fsize;
		write(dst_fd, buf, sz);
		/*
		mvwprintw(statusbar, 0, 0, "progress: %f", progress);
		wrefresh(statusbar);
		getchar();
		*/
	}

	close(src_fd);
	close(dst_fd);
}


static file_panel* get_other_panel()
{
	if (cp->side == P_LEFT)
		return &rpanel;
	else
		return &lpanel;
}


static void* panel_copy(void* par)
{
	file_info* fi = selected_file();
	char dstp[FILENAME_MAX];

	cp = get_other_panel();
	full_path(dstp, fi->name);

	if (fi->is_dir)
		copy_dir(fi->name, dstp);
	else
		copy_file(fi->name, dstp);

	scan_dir(cp->path);
	draw_panel();
	cp = get_other_panel();

	return par;
}


static void copy_dir(const char* dirname, const char* dest)
{
	print_status("Directory copying not supported yet");
}


void start_copy()
{
	if (! cp->select)
		return;
	pthread_t copy_thread, progress_thread;
	pthread_create(&copy_thread, NULL, panel_copy, NULL);
	pthread_create(&progress_thread, NULL, show_progress, NULL);
	pthread_join(copy_thread, NULL);
	pthread_cancel(progress_thread);
}


static void* show_progress(void* par)
{
	print_status("Copy progress: ");
	ssize_t pw = getmaxx(statusbar) - getcurx(statusbar) - 1;
	ssize_t i = 0, old = 0;
	while (i < pw) {
		i = progress * pw;
		if (i != old) {
			whline(statusbar, '!', i - old);
			wrefresh(statusbar);
			old = i;
		}
	}
	return par;
}

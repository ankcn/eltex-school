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
#include <errno.h>
#include "manager.h"


// Левая и правая паенели файлового менеджера
static file_panel lpanel = { P_LEFT, 0, 0, 0 }, rpanel = { P_RIGHT, 0, 0, 0 };

// Текущая (активная) панель
static file_panel* cp = &rpanel;

// Полный путь к редактору файлов
static char editor_path[PATH_MAX];

// Окно статуса
static WINDOW* statusbar;

// Прогресс копирования (от 0 до 1)
static float progress = 0;


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

/*
 * scan_dir - Просмотр содержимого директории
 * @path: путь для просмотра
 */
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

	// Проверям и корректируем окончание пути
	check_path(cp->path);

	// Задаём текущий рабочий каталог
	chdir(cp->path);

	// Обходим содержимое директории, добавляем найденные файлы в коллекцию
	while ((fentry = readdir(dir)))
		add_file(fentry->d_name);
	closedir(dir);

	// Сортируем по алфавиту всё, что нашли
	sort_panel();

	return 0;
}

/*
 * clean_up - Высвобождение ресурсов
 */
void clean_up()
{
	endwin();
	free_names(&lpanel);
	free_names(&rpanel);
	if (statusbar)
		delwin(statusbar);
}

// Представление размера файла в коротком виде с множителем (кило, Мега и т.п.)
static void  size_short(char* buf, size_t x)
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
static void list_files()
{
	if (! cp->wnd)
		return;

	for (ssize_t i = 0; i < cp->count && i < max_lines(); ++i) {
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
	print_status("");
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

/* 
 * sort_panel - Сортировка файлов, обнаруженных в директории
 */
static void sort_panel()
{
	qsort(cp->files, cp->count, sizeof(file_info), cmp_adapter);
}

/*
 * max_lines - получение максимального количества линий панели
 * 
 * Функция определяет максимальное количество файлов, 
 * которые можно отобразить одновременно на панели менеджера
 */
int max_lines()
{
	return getmaxy(cp->wnd) - INNER_OFFSET - 1;
}

/*
 * move_up - Подняться вверх по списку файлов на заданное количество позиций
 * @num: количество позиций для перехода
 */
void move_up(size_t num)
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

/*
 * move_down - Спустится вниз по спику файлов на заданное количество позиций
 * @num: количество позиций для перехода
 */
void move_down(size_t num)
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

/*
 * prepare - Подготовтельные мероприятия при запуске программы
 * @dir: директория, которую следует открыть при старте
 *
 * Здесь выполняется переключение экрана в режим curses,
 * сканирование начальных каталогов и отрисовка панелей
 */
void prepare(const char* dir)
{
	switch_to_curses_mode();

	// Выясняем текущую директорию и отображаем её содержимое
	char path[PATH_MAX];
	getcwd(path, PATH_MAX);
	scan_dir(path);
	cp = get_other_panel();
	if (! dir || scan_dir(dir))
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

	print_status("Ready");
}

/*
 * get_key - Получить код символа/клавиши
 */
int get_key()
{
	int a = getch();
	return (a == WCTRL('X') || a == KEY_F(10)) ? 0 : a;
}

/*
 * draw_panel - Отрисовка панели на экране терминала
 */
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

/*
 * switch_panel - Переключиться на другую панель менеджера файлов
 */
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

/*
 * change_dir - Сменить директорию на заданную
 * @dirname: имя директории, куда надо зайти
 */
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

/*
 * full_path - Получить полный путь к файлу из его имени и текущей директории
 * @buf: строка, в которую будет сохранён результат
 * @fname: имя файла
 */
static void full_path(char* buf, const char* fname)
{
	strcpy(buf, cp->path);
	strcat(buf, fname);
}

/*
 * parent_dir - Получить родительский и текущий каталог от заданного полного пути
 * @par: строка для сохранения полного пути к родительскому каталогу
 * @cur: стока для сохранения имени текущего каталога
 * @path: заданный путь
 */
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

/*
 * is_root - Проверка, не является ли текущая директория корневой
 */
static bool is_root()
{
	return (bool) ! strcmp(cp->path, "/");
}

/*
 * go_top - Перейти наверх списка файлов
 */
void go_top()
{
	cp->select = 0;
	cp->start = 0;
	draw_panel();
}

/*
 * go_end - Перейти на последний файл в директории
 */
void go_end()
{
	move_down(cp->count);
}

/*
 * redraw - Переотрисовка экрана файлового менеджера
 */
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

/*
 * print_status - Печать статусного сообщения
 * @msg: строка сообщения
 */
static void print_status(const char* msg)
{
	werase(statusbar);
	mvwaddstr(statusbar, 0, 1, msg);
	wrefresh(statusbar);
}

/*
 * selected_file - Получение указателя на выбранный файл
 */
static file_info* selected_file()
{
	return &cp->files[cp->select];
}

/*
 * enter - Обработка нажатия клавиши Enter
 */
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

/*
 * start_editor - Запуск внешнего редактора файлов
 * @fname: имя файла для открытия в редакторе
 */
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

/*
 * switch_to_curses_mode - Перевод экрана терминала в режим curses
 */
static void switch_to_curses_mode()
{
	initscr();
	noecho();
	curs_set(FALSE);
	keypad(stdscr, TRUE);
	cbreak();
	refresh();
}

/*
 * copy_file - Копирование файла
 * @fname: имя файла, который надо скопировать
 * @dest: путь назначения
 */
static void copy_file(const char* fname, const char* dest)
{
	int src_fd, dst_fd;
	src_fd = open(fname, O_RDONLY);
	if (src_fd <= 0) {
		print_status("Error opening source file");
		return;
	}

	dst_fd = open(dest, O_WRONLY | O_TRUNC | O_CREAT | O_SYNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (dst_fd <= 0) {
		close(src_fd);
		print_status("Error opening destination file");
		return;
	}

	// Определение размера файла
	ssize_t fsize = (ssize_t) lseek(src_fd, 0, SEEK_END);
	if (fsize < 0) {
		close(src_fd);
		print_status("Error gathering file size");
		return;
	}
	lseek(src_fd, 0, SEEK_SET);

	char buf[COPY_BLOCK_SIZE];
	ssize_t sz, readed = 0;

	if (fsize) while ((sz = read(src_fd, buf, COPY_BLOCK_SIZE))) {
		if (sz > 0) {
			progress = ((float) (readed += sz)) / fsize;
			write(dst_fd, buf, sz);
		}
		else if (errno == EINTR)
			continue;
	}
	progress = 0;

	close(src_fd);
	close(dst_fd);
}

/*
 * get_other_panel - Получить указатель на другую панель
 */
static file_panel* get_other_panel()
{
	if (cp->side == P_LEFT)
		return &rpanel;
	else
		return &lpanel;
}

/*
 * panel_copy - Подготовка к копированию файла или директории
 */
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
	chdir(cp->path);

	return par;
}

/*
 * copy_dir - Копирование директории
 * @dirname: каталог, подлежащий копированию
 * @dest: путь назначения
 */
static void copy_dir(const char* dirname, const char* dest)
{
	print_status("Directory copying not supported yet");
}

/*
 * start_copy - создание потоков для копирования
 */
void start_copy()
{
	if (! cp->select)
		return;
	pthread_t copy_thread, progress_thread;
	pthread_create(&copy_thread, NULL, panel_copy, NULL);
	pthread_create(&progress_thread, NULL, show_progress, NULL);
	pthread_join(copy_thread, NULL);
	pthread_join(progress_thread, NULL);
}

/*
 * show_progress - отображение прогресса копирования
 */
static void* show_progress(void* par)
{
	print_status("Copy progress: ");
	wattron(statusbar, A_REVERSE);
	size_t pw = getmaxx(statusbar) - getcurx(statusbar) - 1;
	size_t i = 0, old = 0;
	while (i < pw) {
		i = progress * pw;
		if (i != old) {
			whline(statusbar, ' ', i);
			wrefresh(statusbar);
			old = i;
		}
	}
	wattroff(statusbar, A_REVERSE);
	return par;
}

/*
 * check_path - Проверка имени пути на корректное окончание
 * @path: строка для проверки
 */
static void check_path(char* path)
{
	char* c = &path[strlen(path) - 1];
	while (c != path && (*c == ' ' || *c == '\t'))
		--c;
	
	// Если в конце пути нет косой черты, добавляем её
	if (*c != '/')
		*++c = '/';
	*++c = 0;
}

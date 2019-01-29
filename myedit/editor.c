#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include "files.h"
#include "editor.h"


// Структура с параметрами редактора
struct {
	size_t pos;	// Текущая позиция в документе
	ssize_t size;	// Размер документа
	size_t x, y;	// Координаты курсора на экране
	int width;	// Ширина экрана
	int height;	// Высота экрана
	size_t disp;	// Позиция символа, с которого начинается видимая область документа
	size_t buf_len;	// Размер буфера под документ
	char fname[FNAME_LEN];	// Имя открытого файла
	char* doc;	// Буфер для открытого документа
} ed = { 0, 0, 0, 0, 0, 0, 0, 0, NO_FILE };
/*
Переменная x соответствует горизонтальной координате экрана ncurses,
а величина y - это номер линни воображаемого экрана, который занимет
весь документ целиком
Координата y экрана ncurses при этом определяется как ed.y - ed.disp
*/


void add_letter(const char c)
{
	// Если добавляем букву не в конец документа, то передвигаем каждый символ
	// от курсора до конца на одну позицию вправо
	if (ed.pos < ed.size)
		for (int i = ed.size; i > ed.pos; --i)
			ed.doc[i] = ed.doc[i - 1];
	// Вставляем новый символ в текущую позицию и увеличиваем её на единицу
	ed.doc[ed.pos++] = c;
	// Увеличиваем размер документа на единицу и проверяем,
	// не заканчивается ли место в буфере
	if (++ed.size > ed.buf_len - (BLOCK_SIZE >> 2))
		// Перезапрашиваем увеличенный объём памяти для буфера
		ed.doc = realloc(ed.doc, ed.size + BLOCK_SIZE);
}


void backspase()
{
	// При удалении предыдущего символа происходит такая же перестановка,
	// как и при добавлении, только в обратную сторону
	if (! ed.pos)
		return;
	--ed.pos;
	for (int i = ed.pos; i < ed.size - 1; ++i)
		ed.doc[i] = ed.doc[i + 1];
	ed.doc[--ed.size] = 0;
}


void fill_line(const size_t y, const char c)
{
	move(y, 0);
	for (int x = 0; x < ed.width; ++x)
		addch(c);
}


void print_on_screen()
{
	clear();
	move(0, 0);
	printw("MyEdit - %s; line: %d; col: %d; pos: %d; size: %d", ed.fname, ed.y, ed.x, ed.pos, ed.size);
	fill_line(1, '-');
	move(FIRST_LINE, 0);
	// Здесь сохраняется символ в позиции, следующей сразу за видимой областью текста
	size_t tp = pos_by_xy(0, ed.disp + ed.height);
	char tc = ed.doc[tp];
	// Временно обнуляем этот байт, чтобы обозначить "конец строки" для printw
	ed.doc[tp] = 0;
	addstr(ed.doc + pos_by_xy(0, ed.disp));
	// Восстанавливаем символ вместо нуля
	ed.doc[tp] = tc;
	// Помещаем курсор терминала в нужные координаты
	move(ed.y - ed.disp + FIRST_LINE, ed.x);
	refresh();
}


size_t pos_by_xy(const int x, const int y)
{
/*
Здесь определяется позиция в документе, соответсвующая заданным
координатам воображаемого экрана
Для этого идёт подсчёт экранных линий rp в документе от начала и
до момента пока не будет достигнуто заданное значение y
*/
	if (x < 0 || y < 0)
		return 0;
	size_t tx = 0, ty = 0;
	size_t rp = 0;
	while (ty < y && rp < ed.size)
		if (ed.doc[rp++] == '\n' || ++tx >= ed.width)
			tx = 0, ++ty;
	return rp + tx;
}


void line_down()
{
/*
Ищем позицию в документе, соответствующую позиции курсора, смещённого
на одну линию вниз. При этом учитываются переносы строк ('\n') в
текущей и следующей линиях.
*/
	int current_line = TRUE;
	for (int i = 0; i < ed.width && ed.pos < ed.size; ++i) {
		if (ed.doc[ed.pos++] == '\n') {
			if (current_line) {
				i = ed.width - ed.x - 1;
				current_line = FALSE;
			}
			else {
				--ed.pos;
				break;
			}
		}
		if (i + ed.x == ed.width)
			current_line = FALSE;
	}
}


void line_up()
{
	if (!ed.y)
		return;
/*
Сначала устанавливаем позицию в начало предыдущей строки,
а затем постепенно идём до координаты x, если строка не
закончится раньше
*/
	ed.pos = pos_by_xy(0, ed.y - 1);
	for (int i = 0; i < ed.x && ed.doc[ed.pos] != '\n'; ++i)
		++ed.pos;
}


void step_forward()
{
	if (ed.pos < ed.size)
		++ed.pos;
}


void step_back()
{
	if (ed.pos)
		--ed.pos;
}


void calc_xy()
{
	ed.y = ed.x = 0;
/*
Расставляем линии по воображаемому экрану, пока не доберёмся до
текущей позиции в документе
*/
	for (int i = 0; i < ed.pos; ++i)
		if (ed.doc[i] == '\n' || ++ed.x >= ed.width)
			ed.x = 0, ++ed.y;
/*
Если курсор вышел за пределы видимой области, то смещаем эту область
в нужную сторону через переменную ed.disp
*/
	if (ed.y < ed.disp)
		ed.disp = ed.y;
	if (ed.y >= ed.disp + ed.height)
		ed.disp = ed.y - ed.height + 1;
}


int load_doc(const char* fname)
{
	ed.pos = 0;
	ed.size = open_file(fname, &ed.doc);
	if (ed.size < 0) {
		strcpy(ed.doc, "Could not open file: ");
		strcat(ed.doc, fname);
		strcpy(ed.fname, NO_FILE);
		ed.size = FNAME_LEN;

		return -1;
	}
	strcpy(ed.fname, fname);
	ed.buf_len = ed.size + BLOCK_SIZE;

	// Ищем и заменяем неподдерживаемые символы
	for (char* it = ed.doc; *it; ++it)
		if (*it == '\t')
			*it = ' ';

	return 0;
}


void update_size()
{
	getmaxyx(curscr, ed.height, ed.width);
	ed.height -= FIRST_LINE;
}


void prepare()
{
	initscr();
	noecho();
	keypad(stdscr, TRUE);
	cbreak();
	ed.doc = (char*) malloc(BLOCK_SIZE);
	ed.buf_len = BLOCK_SIZE;
}


void finish()
{
	endwin();
	free(ed.doc);
}


int get_key()
{
	int a = getch();
	return (a == WCTRL('X') || a == KEY_F(10)) ? 0 : a;
}


void end_of_paragraph()
{
	while (ed.pos < ed.size && ed.doc[ed.pos] != '\n')
		++ed.pos;
}


void begin_of_paragraph()
{
	if (ed.pos)
		while (ed.doc[ed.pos - 1] != '\n' && --ed.pos > 0);
}


void page_down()
{
	ed.pos = pos_by_xy(ed.x, ed.y + ed.height);
}


void page_up()
{
	ed.pos = pos_by_xy(ed.x, ed.y - ed.height);
}


int save_doc()
{
	return save_file(ed.fname, ed.doc, ed.size);
}


int ask_string(const char* question, char* answer)
{
	fill_line(0, ' ');
	mvprintw(0, 0, question);
	echo();
	getstr(answer);
	noecho();
	return strlen(answer);
}


int ask_and_load()
{
	char name[FNAME_LEN];
	if (ask_string("Open file: ", name))
		return load_doc(name);
	else
		return -1;
}


int ask_and_save()
{
	char name[FNAME_LEN];
	if (ask_string("Save as: ", name) && ! save_file(name, ed.doc, ed.size)) {
		strcpy(ed.fname, name);
		return 0;
	}
	else {
		strcpy(ed.fname, "Error");
		return -1;
	}
}


void del_letter()
{
	if (ed.pos < ed.size) {
		++ed.pos;
		backspase();
	}
}

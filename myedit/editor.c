#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include "files.h"
#include "editor.h"


struct {
	unsigned long pos; // ed.pos in doc
	unsigned long size; // Document size
	unsigned int x, y; // Cursor
	int width;
	int height;
	unsigned int disp;
	unsigned long buf_len;
	char fname[FNAME_LEN];
	char* doc;
} ed = { 0, 0, 0, 0, 0, 0, 0, 0, NO_FILE };


void add_letter(const char c)
{
	if (ed.pos < ed.size)
		for (int i = ed.size; i > ed.pos; --i)
			ed.doc[i] = ed.doc[i - 1];
	ed.doc[ed.pos++] = c;
	++ed.size;
	if (ed.size > ed.buf_len - (BLOCK_SIZE >> 2))
		ed.doc = realloc(ed.doc, ed.size + BLOCK_SIZE);
}


void backspase()
{
	if (! ed.pos)
		return;
	--ed.pos;
	for (int i = ed.pos; i < ed.size - 1; ++i)
		ed.doc[i] = ed.doc[i + 1];
	ed.doc[--ed.size] = 0;
}


void print_on_screen()
{
	clear();
	move(0, 0);
	printw("MyEdit - %s; line: %d; col: %d; pos: %d; size: %d", ed.fname, ed.y, ed.x, ed.pos, ed.size);
	move(1, 0);
	for (int x = 0; x < ed.width; ++x)
		addch('-');
	move(FIRST_LINE, 0);
	printw(ed.doc + pos_by_xy(0, ed.disp));
	move(ed.y - ed.disp + FIRST_LINE, ed.x);
	refresh();
}


unsigned long pos_by_xy(const int x, const int y)
{
	if (x < 0 || y < 0)
		return 0;
	unsigned int tx = 0, ty = 0;
	unsigned long rp = 0;
	while (ty < y && rp < ed.size)
		if (ed.doc[rp++] == '\n' || ++tx >= ed.width)
			tx = 0, ++ty;
	return rp + tx;
}


void line_down()
{
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
	for (int i = 0; i < ed.pos; ++i)
		if (ed.doc[i] == '\n' || ++ed.x >= ed.width)
			ed.x = 0, ++ed.y;
	if (ed.y < ed.disp)
		ed.disp = ed.y;
	if (ed.y >= ed.disp + ed.height)
		ed.disp = ed.y - ed.height + 1;
}


long load_doc(const char* fname)
{
	ed.size = open_file(fname, ed.doc);
	if (ed.size < 0) {
		strcpy(ed.fname, "open error");
		ed.size = 0;
		return 1;
	}
	strcpy(ed.fname, fname);
	ed.buf_len = ed.size + BLOCK_SIZE;
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


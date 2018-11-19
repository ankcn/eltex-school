#include <curses.h>
#include "files.h"
#include "editor.h"


struct {
	unsigned long pos; // ed.pos in doc
	unsigned long sz; // Document size
	unsigned int x, y; // Cursor
	int width;
	int height;
} ed = { 0, 0, 0, 0, 0, 0 };

char doc[111222] = "Test line";


void add_letter(const char c)
{
	if (ed.pos < ed.sz)
		for (int i = ed.sz; i > ed.pos; --i)
			doc[i] = doc[i - 1];
	doc[ed.pos++] = c;
	++ed.sz;
}


void backspase()
{
	if (! ed.pos)
		return;
	--ed.pos;
	for (int i = ed.pos; i < ed.sz - 1; ++i)
		doc[i] = doc[i + 1];
	doc[--ed.sz] = 0;
}


void print_on_screen()
{
	clear();
	move(0, 0);
	printw("MyEdit - unsaved");
	move(FIRST_LINE, 0);
	printw(doc);
	move(ed.y + FIRST_LINE, ed.x);
	refresh();
}


void line_down()
{
	int current_line = 1;
	for (int i = 0; i < ed.width && ed.pos < ed.sz; ++i) {
		if (doc[ed.pos++] == '\n') {
			if (current_line) {
				i = ed.width - ed.x - 1;
				current_line = 0;
			}
			else {
				--ed.pos;
				break;
			}
		}
		if (i + ed.x == ed.width)
			current_line = 0;
	}
}


void line_up()
{

}


void step_forward()
{
	if (ed.pos < ed.sz)
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
		if (doc[i] == '\n' || ++ed.x >= ed.width)
			ed.x = 0, ++ed.y;
}


int load_doc(const char* fname)
{
	ed.sz = open_file(fname, doc);
	return 0;
}


void update_size()
{
	getmaxyx(curscr, ed.width, ed.height);
}


void prepare()
{
	initscr();
	noecho();
	keypad(stdscr, TRUE);
	cbreak();
}


void finish()
{
	endwin();
}


int get_key()
{
	int a = getch();
	return (a == ('X' - 0100) || a == KEY_F(10)) ? 0 : a;
}



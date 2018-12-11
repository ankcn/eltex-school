#include <curses.h>
#include "interface.h"
#include "manager.h"



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
	clean_up();
}


int get_key()
{
	int a = getch();
	return (a == WCTRL('X') || a == KEY_F(10)) ? 0 : a;
}




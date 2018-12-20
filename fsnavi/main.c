#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include "manager.h"


int main(int argc, char** argv)
{
	prepare();

	int k = 0;	// Код клавиши, получаемой от терминала
	do switch (k) {
		case KEY_UP:
			move_up(1);
			break;
		case KEY_DOWN:
			move_down(1);
			break;
		case KEY_END:
		case WCTRL('E'):
			go_end();
			break;
		case KEY_HOME:
		case WCTRL('H'):
			go_top();
			break;
		case KEY_NPAGE:
		case KEY_RIGHT:
			move_down(max_lines());
			break;
		case KEY_PPAGE:
		case KEY_LEFT:
			move_up(max_lines());
			break;
		case KEY_ENTER:
		case '\n':
			enter();
			break;
		case '\t':
			switch_panel();
			break;
		case KEY_RESIZE:
			new_size();
			break;
	} while ((k = get_key()));

	clean_up();
	return 0;
}

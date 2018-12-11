#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include "interface.h"


int main()
{
	prepare();
//	scan_dir(".");
//	print_panel();

	int k = 0;	// Код клавиши, получаемой от терминала

	do {
//		update_size();

		switch (k) {
		case KEY_UP:

			break;
		case KEY_DOWN:

			break;
		case KEY_END:
		case WCTRL('E'):

			break;
		case KEY_HOME:
		case WCTRL('H'):

			break;
		case KEY_NPAGE:
		case KEY_RIGHT:

			break;
		case KEY_PPAGE:
		case KEY_LEFT:

			break;
		case KEY_ENTER:

			break;
		}


	} while ((k = get_key()));

	finish();
    return 0;
}

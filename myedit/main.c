#include <stdio.h>
#include <curses.h>
#include "editor.h"


int main()
{
	prepare();
	load_doc("kilo.txt");

	int k = 0x100;	// Keycode

	do {
		update_size();

		switch (k) {
		case KEY_BACKSPACE:
			backspase();
			break;
		case KEY_UP:
			line_up();
			break;
		case KEY_LEFT:
			step_back();
			break;
		case KEY_RIGHT:
			step_forward();
			break;
		case KEY_DOWN:
			line_down();
			break;
		case KEY_END:
//			while (ed.pos < ed.sz && doc[++ed.pos] != '\n');
			break;
		default:
			if (! (k & 0xFF00))
				add_letter(k);
		}

		// Defining cursor coordinates on the screen
		calc_xy();
		print_on_screen();
//		move(0,0);
//		printw("ed.pos = %d; sz = %d; y = %d; x = %d", ed.pos, ed.sz, ed.y, ed.x);

	} while (k = get_key());

	finish();
    return 0;
}


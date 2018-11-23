// Проект MyEdit - простой текстовый редактор

#include <stdio.h>
#include <curses.h>
#include "editor.h"


int main(int argc, char** argv)
{
	prepare();	// Подготовительные мероприятия

	// Если передан параметр, то считаем его именем файла и
	// пытаемся загрузить
	if (argc == 2)
		load_doc(argv[1]);
	else
		load_doc("hotkeys.txt");

	int k = 0;	// Код клавиши, получаемой от терминала

/*
Основной цикл, в котором запрашиваем код символа из терминала,
выполняем процедуру, соответствующую полученному коду, либо
добавляем букву в текущий документ, если это печатный символ.
Здесь же обновляем значения размера окна редактора,
вычисляем текущие координаты курсора и рисуем экран вместе с
видимой частью документа.
Цикл завершается, когда получена определённая комбинация клавиш.
*/
	do {
		update_size();

		switch (k) {
		case KEY_BACKSPACE:
			backspase();
			break;
		case KEY_DC:
		case WCTRL('D'):
			del_letter();
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
		case WCTRL('E'):
			end_of_paragraph();
			break;
		case KEY_HOME:
		case WCTRL('H'):
			begin_of_paragraph();
			break;
		case KEY_NPAGE:
			page_down();
			break;
		case KEY_PPAGE:
			page_up();
			break;
		case KEY_F(2):
		case WCTRL('O'):
			ask_and_save();
			break;
		case KEY_F(3):
		case WCTRL('R'):
			ask_and_load();
			break;
		default:
			if ((k > 31 && k < 127 && k != 37) || k == '\n')
				add_letter(k);
		}

		calc_xy();
		print_on_screen();

	} while ((k = get_key()));

	finish();	// Закрываем открытое, освобождаем ресурсы
    return 0;
}


#ifndef EDITOR_H_INCLUDED
#define EDITOR_H_INCLUDED


#define FIRST_LINE	1


void prepare();

void finish();

void print_on_screen();

void add_letter(const char c);

void backspase();

void line_down();

void calc_xy();

void step_forward();

void step_back();

int load_doc(const char* fname);

void update_size();

int get_key();


#endif // EDITOR_H_INCLUDED

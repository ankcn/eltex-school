#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED


// Предельное количество аргументов команды
#define MAX_ARGS	99

// Коды результата парсинга команды
enum { RET_NORMAL = 0, RET_EXIT, RET_ERROR };


int parse_command(char* cmd);


#endif // SHELL_H_INCLUDED

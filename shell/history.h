#ifndef HISTORY_H_INCLUDED
#define HISTORY_H_INCLUDED


// Предельный размер истории команд
#define HISTORY_MAX_SIZE	333


void history_add(const char* line);

void history_free();

char* history_get_next();

char* history_get_prev();


#endif // HISTORY_H_INCLUDED

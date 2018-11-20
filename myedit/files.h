#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED


#define BLOCK_SIZE	0x10000


// Загрузка из файла в буфер
int open_file(const char* fname, char* buf);

// Выгрузка из буфера в файл
int save_file(const char* fname, const char* buf);


#endif // FILES_H_INCLUDED

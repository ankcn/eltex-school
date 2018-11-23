#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "files.h"


int open_file(const char* fname, char** buf)
{
	int fd = open(fname, O_RDONLY);
	if (fd <= 0)
		return -1;

	// Определение размера файла
	long fsize = (long) lseek(fd, 0, SEEK_END);
	if (fsize < 0) {
		close(fd);
		return -1;
	}
	lseek(fd, 0, SEEK_SET);

	// Перевыделяем память под содержимое файла
	char* tmp = (char*) realloc(*buf, fsize + BLOCK_SIZE);
	if (! tmp)
		return -1;
/*
Для возможности перемещения буфера на вновь выделенную область памяти
в качестве параметра функции передаётся указатель на указатель buf,
а не просто указатель по значению
*/
	*buf = tmp;

	// Не работает
/*
	size_t readed, need = fsize;
	while (need && (readed = read(fd, *buf, need))) {
		if (readed < 0) {
			if (errno == EINTR)
				continue;
			close(fd);
			return -1;
		}
		need -= readed;
		*buf += readed;
	}
*/
	// Чтение из файла в буфер
	if (read(fd, *buf, fsize) < 0) {
		close(fd);
		return -1;
	}

	if (close(fd) < 0)
		return -1;
	return fsize;
}


int save_file(const char* fname, const char* buf, const size_t sz)
{
	int fd = open(fname, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	if (fd <= 0)
		return -1;

	if (write(fd, buf, sz) < 0) {
		close(fd);
		return -1;
	}

	if (close(fd) < 0)
		return -1;
	return 0;
}

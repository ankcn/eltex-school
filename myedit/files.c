#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "files.h"


int open_file(const char* fname, char* buf)
{
	int fd = open(fname, O_RDONLY);
	if (fd <= 0)
		return -1;
	long fsize = (long) lseek(fd, 0, SEEK_END);
	if (fsize < 0) {
		close(fd);
		return -1;
	}
	lseek(fd, 0, SEEK_SET);
	char* tmp = (char*) realloc(buf, fsize + BLOCK_SIZE);
	if (! tmp)
		return -2;
	buf = tmp;
	if (read(fd, buf, fsize) < 0) {
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

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
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
	if (read(fd, buf, fsize) < 0) {
		close(fd);
		return -1;
	}
	if (close(fd) < 0)
		return -1;
	return fsize;
}


int save_file(const char* fname, const char* buf)
{

	return 0;
}

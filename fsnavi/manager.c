#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "manager.h"



file_panel lpanel = { 0, 0, 0 }, rpanel = { 0, 0, 0 };
file_panel* cp = &lpanel;


void free_names(file_panel* pnl)
{
	for (size_t i = 0; i < pnl->count; ++i)
		free(pnl->files[i].name);
	pnl->count = 0;
	free(pnl->files);
	pnl->space = 0;
}


void add_file(const char* fname)
{
	if(! strcmp(fname, "."))
		return;
	if (cp->count >= cp->space) {
		cp->files = realloc(cp->files, (cp->space + FILES_PACK) * sizeof(file_info));
		cp->space += FILES_PACK;
	}

	file_info* fi = &(cp->files[cp->count++]);

	size_t len = strlen(fname);
	char* slot = realloc(fi->name, len);
	if (slot == NULL)
		return;

	strcpy(slot, fname);
	fi->name = slot;

	struct stat attributes;
	stat(fname, &attributes);
	fi->mtime = *(localtime(&attributes.st_ctime));
	fi->size = attributes.st_size;
	fi->is_dir = S_ISDIR(attributes.st_mode);
	fi->mode_bits = attributes.st_mode;
	fi->uid = attributes.st_uid;
	fi->gid = attributes.st_gid;
}


int scan_dir(const char* path)
{
	struct dirent* fentry;
	DIR* dir = opendir(path);

	if (! dir)
		return 1;

	cp->count = 0;

	while ((fentry = readdir(dir)))
		add_file(fentry->d_name);
	closedir(dir);

	sort_panel();

	return 0;
}


void clean_up()
{
	free_names(&lpanel);
	free_names(&rpanel);
}


void print_panel()
{
	for (size_t i = 0; i < cp->count; ++i) {
		char timestr[TIME_STR_LEN] = "undefined";
		file_info* fi = &(cp->files[i]);
		strftime(timestr, TIME_STR_LEN, "%d.%m.%Y %T", &(fi->mtime));
		printf("%c%s\t| %u\t| %s\n", fi->is_dir ? '/' : ' ',
				fi->name, (unsigned int) fi->size, timestr);
	}
}


int cmp_adapter(const void* a, const void* b)
{
	file_info* fa = (file_info*) a;
	file_info* fb = (file_info*) b;
	if (fa->is_dir && ! fb->is_dir)
		return -1;
	else if	(! fa->is_dir && fb->is_dir)
		return 1;
	else
		return strcmp(fa->name, fb->name);
}


void sort_panel()
{
	qsort(cp->files, cp->count, sizeof(file_info), cmp_adapter);
}



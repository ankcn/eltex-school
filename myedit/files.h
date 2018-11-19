#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED


#define BLOCK_SIZE	0x10000


//
int open_file(const char* fname, char* buf);

//
int save_file(const char* fname, const char* buf);


#endif // FILES_H_INCLUDED

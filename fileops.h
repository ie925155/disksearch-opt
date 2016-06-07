#ifndef _FILEOPS_H_
#define _FILEOPS_H_

#include <stdio.h>

void *Fileops_init(char *diskpath);
int Fileops_open(char *pathname);
int Fileops_read(int fd, char *buffer, int length);
int Fileops_getchar(int fd);
int Fileops_tell(int fd);
int Fileops_close(int fd);
int Fileops_isfile(char *pathname);

void Fileops_Dumpstats(FILE *file);

#endif // _FILEOPS_H_

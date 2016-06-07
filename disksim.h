#ifndef _DISKSIM_NEW_H_
#define _DISKSIM_NEW_H_

#include <stdint.h>
#include <stdio.h>

int disksim_open(char *pathname, int readOnly);
int disksim_getsize(int fd); 
int disksim_readsector(int fd, int sectorNum, void *buf); 
int disksim_writesector(int fd, int sectorNum, void *buf); 
int disksim_close(int fd);
void disksim_dumpstats(FILE *file);

#endif // _DISKSIM_NEW_H_

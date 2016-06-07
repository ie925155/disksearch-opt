#ifndef _SCAN_H_
#define _SCAN_H_

#include "index.h"
#include "pathstore.h"
#include <stdio.h>

int Scan_TreeAndIndex(char *pathname, Index *ind, Pathstore *store, int discardDups);
void Scan_Dumpstats(FILE *file);

#endif // _SCAN_H_

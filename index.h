#ifndef _INDEX_H_
#define _INDEX_H_

#include <stdio.h>
#include <stdbool.h>

typedef struct Index {
  void *private;
} Index;

typedef struct IndexLocation {
  char *pathname;   // Pathname of file containing the word
  int  offset;      // Offset into pathname of the word
} IndexLocation;

typedef struct IndexLocationList {
  IndexLocation item;                      // Where it is located
  struct IndexLocationList *nextLocation;  // Link list to next
} IndexLocationList;

Index *Index_Create(void);
bool Index_StoreEntry(Index *ind, char *keyword, char *pathname, int offset);
IndexLocationList *Index_RetrieveEntry(Index *ind, char *keyword);
void Index_Dumpstats(FILE *file);

#endif // _INDEX_H_

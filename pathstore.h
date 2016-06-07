#ifndef _PATHSTORE_H_
#define _PATHSTORE_H_

typedef struct Pathstore {
  struct PathstoreElement *elementList;
  void                    *fshandle;
} Pathstore;

Pathstore* Pathstore_create(void *fshandle);
void       Pathstore_destory(Pathstore *store);
char*      Pathstore_path(Pathstore *store, char *pathname,
                          int discardDuplicateFiles);

void Pathstore_Dumpstats(FILE *file);

#endif // _PATHSTORE_H_

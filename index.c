/**
 * index.c  -  This module maintains the in memory word index for the file search
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <openssl/lhash.h>
#include "index.h"
#include "debug.h"

static uint64_t numstores = 0;
static uint64_t numlookups = 0;
static uint64_t numentriesalloc = 0;

static _LHASH *hashTable = NULL;  // For stats print only

typedef struct IndexHashEntry {
  char *keyword;    // The string we care about
  IndexLocationList *locationList;   // Where it is located
} IndexHashEntry;

static unsigned long HashCallback(const void *arg) {
  IndexHashEntry *hash_entry = (IndexHashEntry *) arg;
  return lh_strhash(hash_entry->keyword);
}

static int CompareCallback(const void *arg1, const void *arg2) {
  IndexHashEntry *e1 = (IndexHashEntry *) arg1;
  IndexHashEntry *e2 = (IndexHashEntry *) arg2;
  return strcmp(e1->keyword,e2->keyword);
}

Index *Index_Create(void) {
  Index *ind = malloc(sizeof(Index));
  if (!ind)
    return NULL;

  ind->private = lh_new(HashCallback, CompareCallback);
  hashTable = (_LHASH*) (ind->private);
  return ind;
}

bool Index_StoreEntry(Index *ind, char *keyword, char *pathname, int offset) {
  _LHASH *hashtable = (_LHASH*) (ind->private);

  DPRINTF('i', ("Index_Store(key=%s,%s:%d)\n", keyword, pathname, offset));

  numstores++;

  IndexLocationList *newItem = (IndexLocationList *) malloc(sizeof(IndexLocationList));
  newItem->item.pathname = pathname;
  newItem->item.offset = offset;
  newItem->nextLocation = NULL;

  char *word = strdup(keyword);
  IndexHashEntry key; 
  key.keyword = word;

  IndexHashEntry *entry;
  entry = lh_retrieve(hashtable, (char *) &key);
  
  if (entry == NULL) {
    entry = malloc(sizeof(IndexHashEntry));
    if (entry == NULL) {
      free(word);
      return false;
    }
    numentriesalloc++;
    entry->keyword = word;
    entry->locationList = NULL;

    lh_insert(hashtable,(char *) entry);

    if (lh_error(hashtable)) {
      free(word);
      free(entry);
      return false;
    }
  } else {
    free(word);
  }

  newItem->nextLocation =  entry->locationList;
  entry->locationList = newItem;
  return true;
}

IndexLocationList *Index_RetrieveEntry(Index *ind, char *keyword) {
  _LHASH *hashtable = (_LHASH*) ind->private;

  numlookups++;

  IndexHashEntry key;
  key.keyword = keyword;

  IndexHashEntry *entry = lh_retrieve(hashtable, (char *) &key);
  return entry ? entry->locationList : (IndexLocationList *) NULL;
}

void Index_Dumpstats(FILE *file) {
  fprintf(file,
          "Index: %"PRIu64" stores, %"PRIu64" allocates, %"PRIu64" lookups\n",
          numstores, numentriesalloc, numlookups);

#ifdef PRINT_HASH_STATS
  if (hashTable)
    lh_stats(hashTable, file);
#endif
}

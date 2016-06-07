/*
 * scan.c  -  This module provides scans files and inserts them
 * into the index.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <inttypes.h> // for PRIu64

#include "index.h"
#include "fileops.h"
#include "scan.h"
#include "debug.h"

#include "assign1/direntv6.h"

static uint64_t numfiles = 0;
static uint64_t numwords = 0;
static uint64_t numchars = 0;
static uint64_t numdups = 0;
static uint64_t numdirs = 0;
static uint64_t numdirents = 0;

#define MAX_WORD_SIZE 64

/**
 * Tokenizes the specified file and place it in the index.
 */
int Scan_File(char *inpathname, Index *ind, Pathstore *store, int discardDups) {
  // Save the pathname in the store
  char *pathname = Pathstore_path(store, inpathname,discardDups);
  if (pathname == NULL) {
    numdups++;
    DPRINTF('s',("Scan_Pathname discard dup (%s)\n", inpathname));
    return 0;
  }
  numfiles++;
  DPRINTF('s', ("Scan_Pathname(%s)\n", pathname));

  int fd = Fileops_open(pathname);
  if (fd < 0) {
    fprintf(stderr, "Can't open pathname %s\n", pathname);
    return -1;
  }

  int ch = Fileops_getchar(fd);
  numchars++;

  while (!(ch < 0)) {   // Process words until we reach the end of the file
    while (!isalpha(ch)) {    // Skip any leading non-alpha characters
      ch = Fileops_getchar(fd);
      if (ch < 0) {Fileops_close(fd); return 0; }
      numchars++;
    }
    // Found a word - record it in the index.
    int offset = Fileops_tell(fd);
    int pos = 0;
    // read off the word until we hit the end of the word buffer
    // or the end of the file or we hit an non-alpah characters
    char word[MAX_WORD_SIZE+1];
    while ((pos < MAX_WORD_SIZE) && !(ch < 0) && isalpha(ch)) {
      word[pos++] = ch;
      ch = Fileops_getchar(fd);
      numchars++;
    }
    numwords++;
    word[pos] = 0; // terminate string
    bool ok = Index_StoreEntry(ind, word, pathname, offset);
    assert(ok);
  }

  Fileops_close(fd);
  return 0;
}

int Scan_TreeAndIndex(char *pathname, Index *ind, Pathstore *store,int discardDups) {
  const uint32_t MAXPATH = 1024;
  if (Fileops_isfile(pathname)) {
    return Scan_File(pathname, ind, store, discardDups);
  }
  // Not a file must be directory, process all entries in the directory

  if (strlen(pathname) > MAXPATH-16) {
    fprintf(stderr, "Too deep of directories %s\n", pathname);
    return -1;
  }
  numdirs++;

  int dirfd = Fileops_open(pathname);
  if (dirfd < 0) {
    fprintf(stderr, "Can't open pathname %s\n", pathname);
    return -1;
  }

  if (pathname[1] == 0) {
    // pathame == "/"
    pathname++; // Delete extra / character
  }


  int ret;
  while (1)  {
    struct direntv6 dirent;
    ret = Fileops_read(dirfd, (char *)&dirent, sizeof(struct direntv6));

    if (ret == 0)  {
      /* Done with directory */
      break;
    }

    if (ret != sizeof(struct direntv6)) {
      fprintf(stderr, "Error reading directory %s\n", pathname);
      ret = -1;
      break;
    }

    numdirents++;
    char *n = dirent.d_name;
    if (n[0] == '.') {
      if ((n[1] == 0) || ((n[1] == '.') && (n[2] == 0))) {
	       /* Skip over "." and ".." */
	       continue;
      }
    }

    char nextpath[MAXPATH];
    sprintf(nextpath, "%s/%s",pathname, n);
    Scan_TreeAndIndex(nextpath, ind, store, discardDups);
  }

  Fileops_close(dirfd);
  return ret;
}

void Scan_Dumpstats(FILE *file) {
  fprintf(file,
	  "Scan: %"PRIu64" files, %"PRIu64" words, %"PRIu64" characters, "
          "%"PRIu64" directories, %"PRIu64" dirents, %"PRIu64" duplicates\n",
	  numfiles, numwords, numchars, numdirs, numdirents, numdups);
}

/**
 * fileops.c  -  This module provides an Unix like file absraction
 * on the assign1 file system access code
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>

#include "fileops.h"
#include "assign1/pathname.h"
#include "assign1/unixfilesystem.h"
#include "diskimg.h"
#include "assign1/inode.h"
#include "assign1/file.h"
#include "assign1/chksumfile.h"

#define MAX_FILES 64

static uint64_t numopens = 0;
static uint64_t numreads = 0;
static uint64_t numgetchars = 0;
static uint64_t numisfiles = 0;

/**
 * Table of open files.
 */
static struct {
  char *pathname;    // absolute pathname NULL if slot is not used.
  int  cursor;       // Current position in the file
} openFileTable[MAX_FILES];

static struct unixfilesystem *unixfs;

/**
 * Initialize the fileops module for the specified disk.
 */
void *Fileops_init(char *diskpath) {
  memset(openFileTable, 0, sizeof(openFileTable));
  int fd = diskimg_open(diskpath, 1);
  if (fd < 0) {
    fprintf(stderr, "Can't open diskimagePath %s\n", diskpath);
    return NULL;
  }

  unixfs = unixfilesystem_init(fd);
  if (unixfs == NULL) {
    diskimg_close(fd);
    return NULL;
  }
  return unixfs;
}

/**
 * Open the specified absolute pathname for reading. Returns -1 on error;
 */
int Fileops_open(char *pathname) {
  numopens++;
  int inumber = pathname_lookup(unixfs,pathname);
  if (inumber < 0) {
    return -1; // File not found
  }

  int fd;
  for (fd = 0; fd < MAX_FILES; fd++) {
    if (openFileTable[fd].pathname == NULL) break;
  }
  if (fd >= MAX_FILES) {
    return -1;  // No open file slots
  }
  openFileTable[fd].pathname = strdup(pathname); // Save our own copy
  openFileTable[fd].cursor = 0;
  return fd;
}

/**
 * Fetch the next character from the file. Return -1 if at end of file.
 */
int Fileops_getchar(int fd) {
  int inumber;
  struct inode in;
  unsigned char buf[DISKIMG_SECTOR_SIZE];
  int bytesMoved;
  int err, size;
  int blockNo, blockOffset;

  numgetchars++;

  if (openFileTable[fd].pathname == NULL)
    return -1;  // fd not opened.

  inumber = pathname_lookup(unixfs, openFileTable[fd].pathname);
  if (inumber < 0) {
    return inumber; // Can't find file
  }

  err = inode_iget(unixfs, inumber,&in);
  if (err < 0) {
    return err;
  }
  if (!(in.i_mode & IALLOC)) {
    return -1;
  }

  size = inode_getsize(&in);

  if (openFileTable[fd].cursor >= size) return -1; // Finished with file

  blockNo = openFileTable[fd].cursor / DISKIMG_SECTOR_SIZE;
  blockOffset =  openFileTable[fd].cursor % DISKIMG_SECTOR_SIZE;

  bytesMoved = file_getblock(unixfs, inumber,blockNo,buf);
  if (bytesMoved < 0) {
    return -1;
  }
  assert(bytesMoved > blockOffset);


  openFileTable[fd].cursor += 1;

  return (int)(buf[blockOffset]);
}

/**
 * Implement the Unix read system call. Number of bytes returned.  Return -1 on
 * err.
 */
int Fileops_read(int fd, char *buffer, int length) {
  numreads++;
  int i;
  for (i = 0; i < length; i++) {
    int ch = Fileops_getchar(fd);
    if (ch == -1) break;
    buffer[i] = ch;
  }
  return i;
}

/**
 * Return the current position in the file.
 */
int Fileops_tell(int fd) {
  if (openFileTable[fd].pathname == NULL)
    return -1;  // fd not opened.
  return openFileTable[fd].cursor;
}

/**
 * Close the files - return the resources
 */

int Fileops_close(int fd) {
  if (openFileTable[fd].pathname == NULL)
    return -1;  // fd not opened.
  free(openFileTable[fd].pathname);
  openFileTable[fd].pathname = NULL;
  return 0;
}

/**
 * Return true if specified pathname is a regular file.
 */
int Fileops_isfile(char *pathname) {
  numisfiles++;
  int inumber = pathname_lookup(unixfs, pathname);
  if (inumber < 0) {
    return 0;
  }

  struct inode in;
  int err = inode_iget(unixfs, inumber, &in);
  if (err < 0) return 0;

  if (!(in.i_mode & IALLOC) || ((in.i_mode & IFMT) != 0)) {
    // Not allocated or not a file
    return 0;
  }
  return 1; // Must be a file
}

void Fileops_Dumpstats(FILE *file) {
  fprintf(file,
          "Fileops: %"PRIu64" opens, %"PRIu64" reads, "
          "%"PRIu64" getchars, %"PRIu64 " isfiles\n",
          numopens, numreads, numgetchars, numisfiles);
}


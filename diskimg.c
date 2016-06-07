#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pthread.h>
#include <assert.h>
#include "diskimg.h"
#include "disksim.h"
#include "debug.h"

static uint64_t numreads, numwrites;


/** 
 * Opens a disk image for I/O.  Returns an open file descriptor, or -1 if
 * unsuccessful  
 */
int diskimg_open(char *pathname, int readOnly) {
  return disksim_open(pathname, readOnly);
}

/**
 * Returns the size of the disk image in bytes, or -1 if unsuccessful.
 */
int diskimg_getsize(int fd) {
  return disksim_getsize(fd);
}

/**
 * Read the specified sector from the disk.  Return number of bytes read, or -1
 * on error.
 */
int diskimg_readsector(int fd, int sectorNum, void *buf) {
  numreads++;
  return disksim_readsector(fd, sectorNum, buf);
}

/**
 * Writes the specified sector to the disk.  Return number of bytes written,
 * -1 on error.
 */
int diskimg_writesector(int fd, int sectorNum, void *buf) {
  numwrites++;
  return disksim_writesector(fd, sectorNum, buf);
}

/**
 * Cleans up from a previous diskimg_open() call.  Returns 0 on success, -1 on
 * error
 */
int diskimg_close(int fd) {
  return disksim_close(fd);
}

void diskimg_dumpstats(FILE *file) {
  fprintf(file, "Diskimg: %"PRIu64" reads, %"PRIu64" writes\n",
          numreads, numwrites);
}

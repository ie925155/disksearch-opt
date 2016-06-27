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
#include "cachemem.h"

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
  //printf("%s sectorNum = %d\n", __func__, sectorNum);
  uint16_t tag = sectorNum >> 16;
  uint16_t set_index = sectorNum & 0xFFFF;
  // 64MB / (4 * 516) ~= 32513.xxx
  if(set_index > 32513){
    set_index %= 32513;
  }
  //printf("tag=%hu set_index=%hu\n", tag, set_index);
  extern void *cacheMemPtr;
  /* Use 4-way set associative cache architecture, but not implement
   * LRU eviction policy
  */
  LINE * line = ((LINE*)cacheMemPtr) + (4 * set_index);
  for(int i = 0 ; i < 4 ; i++){
    line += i;
    if( (line->valid == 1) && (line->tag == tag)){
      //printf("cache hit %d++++++++++++++++++++++++++++++++++++++++\n", sectorNu m);
      memcpy(buf, line->block, DISKIMG_SECTOR_SIZE);
      return DISKIMG_SECTOR_SIZE;
    }
  }
  int ret = disksim_readsector(fd, sectorNum, buf);
  line->valid = 1;
  line->tag = tag;
  memcpy(line->block, buf, DISKIMG_SECTOR_SIZE);
  //printf("cache miss %d++++++++++++++++++++++++++++++++++++++++\n", sectorNum);
  return ret;
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

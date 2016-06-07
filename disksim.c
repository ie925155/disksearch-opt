/**
 * disksim - Simulate a disk for the assignment.  
 * You may not modify this file.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#include <assert.h>
#include "disksim.h"
#include "assign1/diskimg.h"
#include "debug.h"

#define ALWAYS_INLINE __attribute__((always_inline))
#define NEVER_INLINE __attribute__((noinline))

extern int diskLatency;
static uint64_t numreads = 0;  // Count of the number of disk reads.
static uint64_t numwrites = 0; // Count of the number of disk writes.

/** 
 * We want this function to show up in gprof, so mark it as
 * NEVER_INLINE. 
 */
static NEVER_INLINE void SimulateDiskLatency(int64_t startTime) {
  extern int diskBusyWaitEnable;
  // Compute how much more we need to wait to simulated the disk
  int64_t waitTimeSoFar = Debug_GetTimeInMicrosecs() - startTime;

  if (waitTimeSoFar < diskLatency) {
    // We need to wait some more.  If we need more than 10ms, we can get this
    // from the Linux kernel with usleep() (unless diskBusyWaitEnable is true).
    // Otherwise we just spin until the time is up.
    int64_t waitTime = diskLatency - waitTimeSoFar;
    if ((waitTime >= 10000) && !diskBusyWaitEnable) {
      usleep(waitTime);
    } else {
      int64_t endTime = startTime + diskLatency;
      while (Debug_GetTimeInMicrosecs() < endTime) ; // spin
    }
  }
}

/**
 * Opens a disk image for I/O.  Returns an open file descriptor, or -1 if
 * unsuccessful  
 */
int disksim_open(char *pathname, int readOnly) {
  return open(pathname, readOnly ? O_RDONLY : O_RDWR);
}

/**
 * Returns the size of the disk image in bytes, or -1 if unsuccessful.
 */
int disksim_getsize(int fd) {
  return lseek(fd, 0, SEEK_END);
}

/**
 * Perform either a read or a write to disk.  Return number of bytes read or
 * written, or -1 on error.
 *
 * Since we declared this function as ALWAYS_INLINE, the compiler will make two
 * copies, one for read() and one for write(), and will optimize away our
 * |if(read)| statements in each copy.
 */
static ALWAYS_INLINE int disksim_perform_operation(int fd, int sectorNum, void* buf, bool do_read) {
  int simulateDisk = diskLatency > 0;
  off_t offset = sectorNum * DISKIMG_SECTOR_SIZE;
  int64_t startTime = 0;

  if (simulateDisk) {
    startTime = Debug_GetTimeInMicrosecs();
  }

  if (lseek(fd, offset, SEEK_SET) == (off_t) -1) {
    return -1;
  }

  ssize_t bytes;
  if (do_read) {
    bytes = read(fd, buf, DISKIMG_SECTOR_SIZE);
    numreads++;
  } else {
    bytes = write(fd, buf, DISKIMG_SECTOR_SIZE);
    numwrites++;
  }

  if (simulateDisk) {
    SimulateDiskLatency(startTime);
  }
  return bytes;
}

/**
 * Reads the specified sector from the disk.  Return number of 
 * bytes read, or -1 on error.
 */
int disksim_readsector(int fd, int sectorNum, void *buf) {
  return disksim_perform_operation(fd, sectorNum, buf, true);
}

/**
 * Writes the specified sector to the disk.  Return number of bytes written,
 * -1 on error.
 */
int disksim_writesector(int fd, int sectorNum, void *buf) {
  return disksim_perform_operation(fd, sectorNum, buf, false);
}

/**
 * Cleans up a previous diskimg_open() call.  Returns 0 on success, -1 on error.
 */
int disksim_close(int fd) {
  return close(fd);
}

void disksim_dumpstats(FILE *file) {
  fprintf(file, "Disksim: %"PRIu64" reads, %"PRIu64" writes\n",
          numreads, numwrites);
}


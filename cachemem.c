/**
 * cachemem.c  -  This module allocates the memory for caches. 
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <inttypes.h> // for PRIu64

#include <sys/mman.h>

#include "cachemem.h"

int cacheMemSizeInKB;
void *cacheMemPtr;

/**
 * Allocate memory of the specified size for the data cache optimizations
 * Return -1 on error, 0 on success. 
 */

int CacheMem_Init(int sizeInKB) {
  /**
   * Size needs to be not negative or too big and 
   * multiple of the 4KB page size 
   */
  if ((sizeInKB < 0) || (sizeInKB > (CACHEMEM_MAX_SIZE/1024)) || (sizeInKB % 4)) {
    fprintf(stderr, "Bad cache size %d\n", sizeInKB);
    return -1;
  }

  void *memPtr = mmap(NULL, 1024 * sizeInKB, PROT_READ | PROT_WRITE, 
		      MAP_PRIVATE|MAP_ANON, -1, 0);
  if (memPtr == MAP_FAILED) {
    perror("mmap");
    return -1;
  }

  cacheMemSizeInKB = sizeInKB;
  cacheMemPtr = memPtr;
  return 0;
}


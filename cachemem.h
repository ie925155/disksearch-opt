#ifndef _CACHEMEM_H_
#define _CACHEMEM_H_

#include "diskimg.h"

/**
 * The main export of the cachemem module is the memory for the cache
 * pointed to by the following global variables:
 *
 * cacheMemSizeInKB - The size of the cache memory in kiloytes.
 * cacheMemPtr      - Starting address of the cache memory.
 */

extern int cacheMemSizeInKB;
extern void *cacheMemPtr;

#define CACHEMEM_MAX_SIZE (64*1024*1024)

int CacheMem_Init(int sizeInKB);

typedef struct {
  uint16_t valid;
  uint16_t tag;
  uint8_t block[DISKIMG_SECTOR_SIZE];
} LINE;

#endif // _CACHEMEM_H_

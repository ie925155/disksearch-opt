#ifndef _CACHEMEM_H_
#define _CACHEMEM_H_

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

#endif // _CACHEMEM_H_

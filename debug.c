#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "debug.h"


char debugFlags[256];

/**
 * Self-explanatory setter.
 */
void Debug_SetFlag(char ch, int val) {
  debugFlags[(unsigned char)ch] = val;
}

/**
 * Get the number of microseconds since the epoch (midnight, January 1, 1970).
 */
int64_t Debug_GetTimeInMicrosecs(void) {
  struct timeval curtime;
  int err = gettimeofday(&curtime, NULL);
  return (curtime.tv_sec * (int64_t) 1000000) + curtime.tv_usec;
}

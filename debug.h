#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>
#include <stdint.h>

extern char debugFlags[];
#define DPRINTF(flag, args) if (debugFlags[flag]) printf args;
void Debug_SetFlag(char ch, int val);
int64_t Debug_GetTimeInMicrosecs(void);

#endif // _DEBUG_H_

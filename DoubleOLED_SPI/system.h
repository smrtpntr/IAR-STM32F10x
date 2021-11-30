#ifndef SYSTEM_H
#define SYSTEM_H

#define CLOCKS_PER_SEC   	        1000

typedef uint32_t 			        clock_t;

void DisableJTAG(void);
clock_t GetTickCount(void);
void InitSystemClock(void);

#endif  // SYSTEM_H
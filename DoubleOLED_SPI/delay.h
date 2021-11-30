#ifndef DELAY_H
#define DELAY_H

void __delay(uint32_t ncy);

#define CLOCK_FREQ      	            72000000
#define INSTR_FREQ      	            ((CLOCK_FREQ) * 8 / 9)
#define US_CYCLES       	            ((INSTR_FREQ) / 1000000)
#define MS_CYCLES       	            ((INSTR_FREQ) / 1000)

#define DelayUs(us)     	            __delay((us) * US_CYCLES)
#define DelayMs(ms)     	            __delay((ms) * MS_CYCLES)

#endif  // DELAY_H

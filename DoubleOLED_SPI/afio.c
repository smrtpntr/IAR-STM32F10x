#include <stdio.h>
#include <stdint.h>

#include "stm32f10x.h"
#include "afio.h"

void InitAFIOClock(void)
{
    uint32_t                            uRCC_APB2Periph;
    FunctionalState                     iNewState;
    
    uRCC_APB2Periph = RCC_APB2Periph_AFIO;
    iNewState = ENABLE;
    
    RCC_APB2PeriphClockCmd(uRCC_APB2Periph, iNewState);
}

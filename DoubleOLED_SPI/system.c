#include <stdio.h>
#include <stdint.h>

#include "stm32f10x.h"
#include "system.h"

static volatile clock_t                 g_uTick;

void DisableJTAG(void)
{
    uint32_t                            uGPIO_Remap;
    FunctionalState                     iNewState;
      
    uGPIO_Remap = GPIO_Remap_SWJ_JTAGDisable;
    iNewState = ENABLE;
    
    GPIO_PinRemapConfig(uGPIO_Remap, iNewState);
}

clock_t GetTickCount(void)
{
    return g_uTick;
}

void InitSystemClock(void)
{
    SysTick_Config(SystemCoreClock / CLOCKS_PER_SEC);
}

void Sys_ClockTick(void)
{
    // elapsed time counter
    ++g_uTick;
}

#include <stdio.h>
#include <stdint.h>

#include "stm32f10x.h"
#include "gpio.h"

GPIOSpeed_TypeDef                       g_iDefaultGPIOSpeed = GPIO_Speed_50MHz;

// RCC : Reset and clock control

static const uint32_t g_uaGPIOClocks[] = {
    RCC_APB2Periph_GPIOA,
    RCC_APB2Periph_GPIOB,
    RCC_APB2Periph_GPIOC,
    RCC_APB2Periph_GPIOD,
    RCC_APB2Periph_GPIOE,
    RCC_APB2Periph_GPIOF,
    RCC_APB2Periph_GPIOG,
};

// Same order with the GPIOPeripheralIndex enumeration 
GPIO_TypeDef* g_paGPIOPeripherals[] = {
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
    GPIOE,
    GPIOF,
    GPIOG,
};

// Same order with the GPIOPeripherals enumeration 
GPIOPeripheral g_aGPIOPeripheralMap [] = {
    {gpiC, 13},             // 
    
    // SPI
    {gpiA, 5}, 		        // D0 (SCK-SERIAL CLOCK OUTPUT)
    {gpiA, 6}, 		        // MISO (MASTER IN SLAVE OUT)
    {gpiA, 7}, 		        // D1 (MOSI-MASTER OUT SLAVE IN)

    // OLED
    {gpiB, 0}, 		        // RES
    {gpiB, 1}, 		        // DC (DATA COMMAND)
    {gpiA, 4}, 		        // CS (CHIP SELECT RIGHT)
    {gpiA, 3}, 		        // CS (CHIP SELECT LEFT)
};

void InitGPIO(int iMapIndex, GPIOMode iMode)
{
    GPIOPeripheralIndex                 gpi;
    uint16_t                            wPinMask;
    GPIO_TypeDef*                       pGPIOx;
    GPIO_InitTypeDef                    gpioInit;
    
    gpi = g_aGPIOPeripheralMap[iMapIndex].gpi;
    wPinMask = (1 << g_aGPIOPeripheralMap[iMapIndex].wPin);
    
    pGPIOx = g_paGPIOPeripherals[gpi];

    gpioInit.GPIO_Mode = (GPIOMode_TypeDef)iMode;
    gpioInit.GPIO_Speed = g_iDefaultGPIOSpeed;
    gpioInit.GPIO_Pin = wPinMask;
    
    GPIO_Init(pGPIOx, &gpioInit);
}

void InitGPIOClock(void)
{
    int iGPIOPeripheralCount, i;
    
    iGPIOPeripheralCount = sizeof(g_uaGPIOClocks) / sizeof(g_uaGPIOClocks[0]);
    
    for (i = 0; i < iGPIOPeripheralCount; i++)
        RCC_APB2PeriphClockCmd(g_uaGPIOClocks[i], ENABLE);
}

void SetGPIOSpeed(GPIOSpeed_TypeDef iGPIOSpeed)
{
    g_iDefaultGPIOSpeed = iGPIOSpeed;
}

int ReadGPIO(int iMapIndex)
{
    GPIOPeripheralIndex                 gpi;
    uint16_t                            wPinMask;

    gpi = g_aGPIOPeripheralMap[iMapIndex].gpi;
    wPinMask = (1 << g_aGPIOPeripheralMap[iMapIndex].wPin);
    
    return (g_paGPIOPeripherals[gpi]->IDR & wPinMask) != 0;
}

void WriteGPIO(int iMapIndex, int iVal)
{
    GPIOPeripheralIndex                 gpi;
    uint16_t                            wPinMask;

    gpi = g_aGPIOPeripheralMap[iMapIndex].gpi;
    wPinMask = (1 << g_aGPIOPeripheralMap[iMapIndex].wPin);
    
    if (iVal)
        g_paGPIOPeripherals[gpi]->BSRR = wPinMask;
    else 
        g_paGPIOPeripherals[gpi]->BRR = wPinMask;
}


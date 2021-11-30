#ifndef GPIO_H
#define GPIO_H

#include "stm32f10x_gpio.h"

typedef enum {
    gpiA, 
    gpiB, 
    gpiC, 
    gpiD, 
    gpiE, 
    gpiF, 
    gpiG, 
} GPIOPeripheralIndex;

typedef enum {
    modeInput = GPIO_Mode_IN_FLOATING,
    modeOutput = GPIO_Mode_Out_PP, 
    modeAlternate = GPIO_Mode_AF_PP, 
    modeAnalog = GPIO_Mode_AIN, 
    
    modeInputPD = GPIO_Mode_IPD, 
    modeInputPU = GPIO_Mode_IPU, 
    modeOutputOD = GPIO_Mode_Out_OD, 
    modeAlternateOD = GPIO_Mode_AF_OD, 
} GPIOMode;

typedef enum {
    gpiopLED,    			// Blue pill LED
    
    // SPI
    gpiopSPI_SCK,           // D0 (SCK-SERIAL CLOCK OUTPUT)
    gpiopSPI_MISO,          // MISO (MASTER IN SLAVE OUT)
    gpiopSPI_MOSI,          // D1 (MOSI-MASTER OUT SLAVE IN)
    
    // OLED
    gpiopOLED_RES,          // Reset
    gpiopOLED_DC,           // DC (DATA COMMAND)
    gpiopOLED_CS_DISPLAY_0, // CS (CHIP SELECT)
    gpiopOLED_CS_DISPLAY_1, // CS (CHIP SELECT)

} GPIOPeripherals;

typedef struct {
    GPIOPeripheralIndex gpi;
    uint16_t wPin;
} GPIOPeripheral;

void InitGPIOClock(void);
void InitGPIO(int iMapIndex, GPIOMode iMode);
int ReadGPIO(int iMapIndex);
void WriteGPIO(int iMapIndex, int iVal);

void SetGPIOSpeed(GPIOSpeed_TypeDef iGPIOSpeed);

#endif  // _GPIO_H
#include <stdio.h>
#include <stdint.h>

#include "stm32f10x_spi.h"
#include "spi.h"
#include "gpio.h"

#define SPI_PORT                        SPI1

void InitSPIClock(void);
void InitSPIPins(void);

void InitSPI(void)
{
    SPI_InitTypeDef                     spiInit;
    
    // SPI_mode 0
    spiInit.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spiInit.SPI_Mode = SPI_Mode_Master;
    spiInit.SPI_DataSize = SPI_DataSize_8b;
    spiInit.SPI_CPOL = SPI_CPOL_Low;
    spiInit.SPI_CPHA = SPI_CPHA_1Edge;
    spiInit.SPI_NSS = SPI_NSS_Soft;
    spiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    spiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    spiInit.SPI_FirstBit = SPI_FirstBit_MSB;
    //spiInit.SPI_CRCPolynomial = ;
    
    InitSPIPins();
    InitSPIClock();
   
    SPI_Init(SPI_PORT, &spiInit);
    
    SPI_Cmd(SPI_PORT, ENABLE);
}

void InitSPIClock(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
}

void InitSPIPins(void)
{
    InitGPIO(gpiopSPI_SCK, modeAlternate);
    InitGPIO(gpiopSPI_MISO, modeInput);
    InitGPIO(gpiopSPI_MOSI, modeAlternate);
}

// SPI birimi üzerinden 8-bit veri gönderir ve alýr
// byData : gönderilen deðer
// return : alýnan deðer
uint8_t SPISendReceive(uint8_t byData)
{
    // SPI Transmit buffer boþ mu?
    while (!SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE));
    
    SPI_I2S_SendData(SPI_PORT, byData);
    
    while (!SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_RXNE));
    
    byData = SPI_I2S_ReceiveData(SPI_PORT);
    
    return byData;
}
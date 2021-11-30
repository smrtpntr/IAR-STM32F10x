#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "afio.h"
#include "console.h"
#include "oled.h"
#include "spi.h"
#include "system.h"

void InitPulseTask(void);

static unsigned                         count;

void Init(void)
{
    InitSystemClock();
    InitGPIOClock();
    InitAFIOClock();
    DisableJTAG();
    
    InitSPI();
    
    InitConsole();    
    InitPulseTask();
}

void InitPulseTask(void)
{
    WriteGPIO(gpiopLED, 1);
    InitGPIO(gpiopLED, modeOutput);
}

void PulseTask(void)
{
    static enum {
        I_LED_OFF, 
        S_LED_OFF, 
        I_LED_ON, 
        S_LED_ON, 
    } state = I_LED_OFF;
    
    // Duruma ilk geçiþ saati
    static clock_t t0;
    
    // Güncel saat deðeri 
    clock_t t1;
    
    t1 = GetTickCount();
    
    switch (state) {
    case I_LED_OFF:
        t0 = t1;
        WriteGPIO(gpiopLED, 1);
        state = S_LED_OFF;
      	//break;
      
    case S_LED_OFF:
        if (t1 >= t0 + 9 * CLOCKS_PER_SEC / 10)
            state = I_LED_ON;
		break;
      
    case I_LED_ON:
      	t0 = t1;
		WriteGPIO(gpiopLED, 0);
		state = S_LED_ON;
		//break;
      
    case S_LED_ON:
        if (t1 >= t0 + CLOCKS_PER_SEC / 10)
            state = I_LED_OFF;
      break;
    }
}

void DrawSin(void)
{
    int x, y;
    float fx, pi = 3.14159265359;
    
    // y = A * sin(2 * p * x);
    for (x = 0; x < 128; ++x) {
    	fx = 32 + 30.0 * sin(2 * pi * (x / 20.0));
    	y = 63 - (int)(fx + 0.5);
        SetPixel(x, y, pxlSet);
    }
}

void Task_Print(void)
{
    printf("SAYI:%10u\r", ++count);
}

int main()
{
    // Baþlangýç yapýlandýrmalarý
    Init();

    int i;
    DisplayDevice iDisplayDevice = ddDisplay0;
    
    for (i = 0; i < 2; i++) {
      
        SetDisplayDevice(iDisplayDevice);
        SetFont(fntLarge);
        printf("Hello, world!   \n");
    
        SetFont(fntSmall);
        SetCursor(2, 0);

        printf("ABCÇDEFGÐHIÝJKLMNOÖPRSÞTUÜVYZ\n");
        printf("abcçdefgðhýijklmnoöprsþtuüvyz\n");
        printf("012345678901234567890\n");
    
        /*OLED_Line(37, 11, 93, 55, pxlSet);
        OLED_Line(127, 0, 0, 63, pxlSet);
      
        OLED_Circle(61, 30, 15, pxlSet);
        OLED_Circle(41, 17, 6, pxlSet);

        //DrawSin();
      
        OLED_PixelData(5, 40, pxlSet);
        OLED_PixelData(6, 39, pxlSet);
        OLED_PixelData(7, 38, pxlSet);
        OLED_PixelData(8, 37, pxlSet);
        OLED_UpdateDisplay();*/
        
        if (iDisplayDevice == ddDisplay0)
          iDisplayDevice = ddDisplay1;
    
    }
    
    iDisplayDevice = ddDisplay0;
    
    // Görev çevrimi (task Loop)
    // Co-Operative Multitasking (Yardýmlaþmalý çoklu görev)
  
    while (1) {
        PulseTask();
        
        switch (iDisplayDevice) {
        case ddDisplay0:
            iDisplayDevice = ddDisplay1;
            break;
        case ddDisplay1:
            iDisplayDevice = ddDisplay0;
            break;
        }
        
        SetDisplayDevice(iDisplayDevice);
        
        Task_Print();
    }
  
    return 0;
}

#include <stdio.h>
#include <stdint.h>

#include "console.h"
#include "gpio.h"
#include "oled.h"

#define _STDIN                          0
#define _STDOUT                         1
#define _STDERR                         2

OLEDisplay                              g_display0 = {gpiopOLED_CS_DISPLAY_0};
OLEDisplay                              g_display1 = {gpiopOLED_CS_DISPLAY_1};

void _putch(unsigned char ch);

void InitConsole(void)
{
    StartOLEDEx(1);
  
    SetDisplayDevice(ddDisplay0);
    StartOLED(1);
    
    SetDisplayDevice(ddDisplay1);
    StartOLED(1);

#ifndef __IAR_SYSTEMS_ICC__
    setvbuf(stdout, NULL, _IONBF, 0);
#endif
}

void _putch(unsigned char ch)
{
    OLED_putch(ch);
}

void SetDisplayDevice(DisplayDevice iDisplayDevice)
{
    switch (iDisplayDevice) {
    case ddDisplay0:
        SetOLEDisplay(&g_display0);
        break;
    case ddDisplay1:
        SetOLEDisplay(&g_display1);
        break;
    }
}

#ifdef __IAR_SYSTEMS_ICC__
size_t __write(int handle, const unsigned char *buffer, size_t size)
#else
size_t _write(int handle, const unsigned char *buffer, size_t size)
#endif
{
    size_t nChars = 0;
  
    if (buffer == NULL)
        return 0;
  
    if (handle != _STDOUT && handle != _STDERR)
        return 0;
  
    while (size--) {
        _putch(*buffer++);
        ++nChars;
    }
  
    return nChars;
}

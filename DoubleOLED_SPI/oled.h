#ifndef _OLED_H
#define _OLED_H

#include "gpio.h"

#define PAGE_COUNT                      8
#define SEGMENT_COUNT                   128

typedef enum {
    fntSmall,
    fntLarge
} Fonts;

typedef enum {
    pxlClear, 
    pxlSet, 
    pxlInvert, 
} PixelOperation;

typedef struct {
    int iWidth;   // aktif font pixel geniþliði (bit)
    int iHeight;  // aktif font yüksekliði (page - byte)
    const unsigned char* pFontTable;     // aktif font tablosu
    Fonts iFont;
} OLEDFont;

typedef struct {
    GPIOPeripherals iCSPin;
    int iPage;
    int iSegment;
    uint8_t byaDisplayData[PAGE_COUNT * SEGMENT_COUNT];
    int iRow;
    int iCol;
    OLEDFont font;
} OLEDisplay;

void Circle(int x, int y, int r, PixelOperation iPOP);
int GetPixel(int x, int y);
void Line(int x0, int y0, int x1, int y1, PixelOperation iPOP);
void OLED_putch(char c);
void SendResetPulse(void);
void SetCursor(int row, int col);
void SetFont(Fonts iFont);
void SetOLEDisplay(OLEDisplay* pDisplay);
void SetPixel(int x, int y, PixelOperation iPOP);
void SetPixelData(int x, int y, PixelOperation iPOP);
void StartOLED(int iRotate);

void StartOLEDEx(int iRotate);

#endif
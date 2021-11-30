#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "delay.h"
#include "system.h"
#include "spi.h"

#include "oled.h"

#include "font_5x8.h"
#include "font_7x16.h"

#define PI      			3.141593654
#define abs(a)      (((a) > 0) ? (a) : -(a))

static void CarriageReturn(void);
static void ClearDisplay(void);
static void FillDisplay(uint8_t ch);
static void FillPage(uint8_t byPage, uint8_t ch);
static int GetColCount(void);
static int GetRowCount(void);
static void NewLine(void);
static void PutChar(unsigned char ch);
static void Rotate(int iRotate);
static void Scroll(int nLines);
static void SendCommand(uint8_t byCommand);
static void SendData(uint8_t byPixelData);
static void SetPage(uint8_t byPage);
static void SetSegment(uint8_t bySegment);
static void UpdateDisplay(void);

OLEDisplay*                             g_pDisplay;
uint8_t*                                g_pDisplayData;
OLEDFont*                               g_pFont;

void CarriageReturn(void)
{
    g_pDisplay->iCol = 0;
}

void Circle(int x, int y, int r, PixelOperation iPOP)
{
    float step, t;
    int dx, dy;

    step = PI / 2 / 64;

    for (t = 0; t <= PI / 2; t += step) {
        dx = (int)(r * cos(t) + 0.5);
        dy = (int)(r * sin(t) + 0.5);

        if (x + dx < 128) {
            if (y + dy < 64)
                SetPixel(x + dx, y + dy, iPOP);
            if (y - dy >= 0)
                SetPixel(x + dx, y - dy, iPOP);
        }
        if (x - dx >= 0) {
            if (y + dy < 64)
                SetPixel(x - dx, y + dy, iPOP);
            if (y - dy >= 0)
                SetPixel(x - dx, y - dy, iPOP);
        }
    }
}

void ClearDisplay(void)
{
    FillDisplay(0);
}

void FillDisplay(uint8_t ch)
{
    int iPage;
    
    for (iPage = 0; iPage < PAGE_COUNT; ++iPage)
        FillPage(iPage, ch);
}

void FillPage(uint8_t byPage, uint8_t ch)
{
    int i;
    
    SetPage(byPage);
    SetSegment(0);
    
    for (i = 0; i < SEGMENT_COUNT; ++i)
      SendData(ch);
}

int GetColCount(void)
{
    return SEGMENT_COUNT / (g_pFont->iWidth + 1);
}

int GetPixel(int x, int y)
{
    uint8_t page, bitIdx, val;
  
    x &= 0x7F;
    y &= 0x3F;
  
    page = y >> 3;
    bitIdx = y & 7;

    val = g_pDisplayData[(unsigned)page * SEGMENT_COUNT + x];
  
    return (val & (1 << bitIdx)) != 0;
}

int GetRowCount(void)
{
    return PAGE_COUNT / (g_pFont->iHeight);
}

void Line(int x0, int y0, int x1, int y1, PixelOperation iPOP)
{
    int steep, t;
    int deltax, deltay, error;
    int x, y;
    int ystep;

    steep = abs(y1 - y0) > abs(x1 - x0);

    if (steep) {
        // swap x and y
        t = x0; x0 = y0; y0 = t;
        t = x1; x1 = y1; y1 = t;
    }

    if (x0 > x1) {
        // swap ends
        t = x0; x0 = x1; x1 = t;
        t = y0; y0 = y1; y1 = t;
    }

    deltax = x1 - x0;
    deltay = abs(y1 - y0);
    error = 0;
    y = y0;

    if (y0 < y1) 
        ystep = 1;
    else
        ystep = -1;

    for (x = x0; x < x1; x++) {
        if (steep)
            SetPixel(y, x, iPOP);
        else
            SetPixel(x, y, iPOP);

        error += deltay;
        
        if ((error << 1) >= deltax) {
            y += ystep;
            error -= deltax;
        }
    }
}

void NewLine(void)
{
    if (++g_pDisplay->iRow >= GetRowCount()) {
        g_pDisplay->iRow = GetRowCount() - 1;
        Scroll(g_pFont->iHeight);
    }
}

void OLED_putch(char c)
{
    switch (c) {
    case '\n':
        NewLine();
    case '\r':
        CarriageReturn();
        break;
    case '\f':
        ClearDisplay();
        break;
    default:
        PutChar(c);
        break;
  }
}

void PutChar(unsigned char ch)
{
    int i, j, k;
    
    if (g_pDisplay->iCol >= GetColCount()) {
        CarriageReturn();
        NewLine();
    }
  
    for (k = 0; k < g_pFont->iHeight; ++k) {
      
        SetPage(g_pDisplay->iRow * g_pFont->iHeight + k);
        SetSegment(g_pDisplay->iCol * (g_pFont->iWidth + 1));
    
        i = g_pFont->iWidth * g_pFont->iHeight * ch + k;
    
        for (j = 0; j < g_pFont->iWidth; ++j) {
            SendData(g_pFont->pFontTable[i]);
            i += g_pFont->iHeight;
        }
        SendData(0);
    }
    ++g_pDisplay->iCol;
}

void Rotate(int iRotate)
{
    unsigned char byRemap, byScan;
  
    if (iRotate) {
        byRemap = 0xA0;
        byScan = 0xC0;
    }
    else {
        byRemap = 0xA1;
        byScan = 0xC8;
    }

    SendCommand(byRemap);      // Set Segment Re-Map
    SendCommand(byScan);       // Set Com Output Scan Direction
}

void SetCursor(int row, int col)
{
    g_pDisplay->iRow = row;
    g_pDisplay->iCol = col;
}

void Scroll(int nLines)
{
    int i, j;
  
    j = nLines * SEGMENT_COUNT;
  
    for (i = 0; i < (PAGE_COUNT - nLines) * SEGMENT_COUNT; ++i)
        g_pDisplayData[i] = g_pDisplayData[j++];
  
    for ( ; i < PAGE_COUNT * SEGMENT_COUNT; ++i)
        g_pDisplayData[i] = 0;
  
    UpdateDisplay();
}

void SendCommand(uint8_t byCommand)
{
    // DataCommandPin = 0 aktif
    WriteGPIO(gpiopOLED_DC, 0);
    
    // ChipSelectPin = 0 aktif
    WriteGPIO(g_pDisplay->iCSPin, 0);
    
    SPISendReceive(byCommand);

    // ChipSelectPin = 1 pasif
    WriteGPIO(g_pDisplay->iCSPin, 1);
    
    // DataCommandPin = 1 pasif
    WriteGPIO(gpiopOLED_DC, 1);
}

void SendData(uint8_t byPixelData)
{
    // DataCommandPin = 0 aktif
    WriteGPIO(gpiopOLED_DC, 1);
    
    // ChipSelectPin = 0 aktif
    WriteGPIO(g_pDisplay->iCSPin, 0);
    
    SPISendReceive(byPixelData);
    
    g_pDisplayData[g_pDisplay->iPage * SEGMENT_COUNT + g_pDisplay->iSegment] = byPixelData;

    if (++g_pDisplay->iSegment >= SEGMENT_COUNT)
        g_pDisplay->iSegment = 0;
    
    // ChipSelectPin = 1 pasif
    WriteGPIO(g_pDisplay->iCSPin, 1);
    
    // DataCommandPin = 1 pasif
    WriteGPIO(gpiopOLED_DC, 0);
}

void SendResetPulse(void)
{
    // SSD1306 reset pulse
    WriteGPIO(gpiopOLED_RES, 0);
    InitGPIO(gpiopOLED_RES, modeOutput);
    DelayMs(10);
    WriteGPIO(gpiopOLED_RES, 1);
}

void SetFont(Fonts iFont)
{
    switch (iFont) {
    case fntSmall:
        g_pFont->iWidth = 5;
        g_pFont->iHeight = 1;
        g_pFont->pFontTable = g_font5x8;
        g_pFont->iFont = fntSmall;
        break;
    case fntLarge:
        g_pFont->iWidth = 7;
        g_pFont->iHeight = 2;
        g_pFont->pFontTable = g_font7x16;
        g_pFont->iFont = fntLarge;
        break;
    }
}

void SetOLEDisplay(OLEDisplay* pDisplay)
{
    g_pDisplay = pDisplay;
    g_pDisplayData = g_pDisplay->byaDisplayData;
    g_pFont = &g_pDisplay->font;
}

void SetPage(uint8_t byPage)
{
    byPage &= 0x07;
    
    g_pDisplay->iPage = byPage;
    SendCommand(0xB0 | byPage);
}

void SetPixel(int x, int y, PixelOperation iPOP)
{
    uint8_t page, bitIdx, val;
    
    x &= 0x7F;
    y &= 0x3F;
    
    page = y >> 3;
	//page = y / 8;
    
    bitIdx =  y & 7;
	// bitIdx =  y % 8
    
    val = g_pDisplayData[page * SEGMENT_COUNT + x];
    
    switch (iPOP) {
    case pxlSet:
        val |= (1 << bitIdx);
        break;
    case pxlClear:
        val &= ~(1 << bitIdx);
        break;
    case pxlInvert:
        val ^= (1 << bitIdx);
        break;
    }
    
    SetPage(page);
    SetSegment(x);
    SendData(val);         // val deðerini tekrar g_pDisplayData dizisine yazýyor
}

void SetPixelData(int x, int y, PixelOperation iPOP)
{
    uint8_t page, bitIdx, val;
  
    x &= 0x7F;
    y &= 0x3F;
  
    page = y >> 3;
    //page = y / 8;
  
    bitIdx = y & 7;
    //bitIdx = y % 8;
  
    val = g_pDisplayData[page * SEGMENT_COUNT + x];
  
    switch (iPOP) {
    case pxlSet:
        val |= (1 << bitIdx);
        break;
    case pxlClear:
        val &= ~(1 << bitIdx);
        break;
    case pxlInvert:
        val ^= (1 << bitIdx);
        break;
    }
  
    g_pDisplayData[page * SEGMENT_COUNT + x] = val;
}

void SetSegment(uint8_t bySegment)
{
    bySegment &= 0x7F;
    
    g_pDisplay->iSegment = bySegment;
    
    SendCommand(bySegment & 0x0F);
    SendCommand(0x10 | (bySegment >> 4));
}

void StartOLEDEx(int iRotate)
{
    InitGPIO(gpiopOLED_DC, modeOutput);

    // SSD1306 reset pulse
    WriteGPIO(gpiopOLED_RES, 0);
    InitGPIO(gpiopOLED_RES, modeOutput);
    DelayMs(10);
    WriteGPIO(gpiopOLED_RES, 1);
}

void StartOLED(int iRotate)
{
    WriteGPIO(g_pDisplay->iCSPin, 1);
    InitGPIO(g_pDisplay->iCSPin, modeOutput);
    
    InitGPIO(gpiopOLED_DC, modeOutput);

    // SSD1306 reset pulse
    /*WriteGPIO(gpiopOLED_RES, 0);
    InitGPIO(gpiopOLED_RES, modeOutput);
    DelayMs(10);
    WriteGPIO(gpiopOLED_RES, 1);*/

    DelayMs(80);
    
    SendCommand(0xAE);              // Set display OFF		
	
    SendCommand(0xD4);     	        // Set Display Clock Divide Ratio / OSC Frequency
    SendCommand(0x80);     	        // Display Clock Divide Ratio / OSC Frequency 

    SendCommand(0xA8);              // Set Multiplex Ratio
    SendCommand(0x3F);              // Multiplex Ratio for 128x64 (64-1)
    
    SendCommand(0xD3);     	        // Set Display Offset
    SendCommand(0x00);     	        // Display Offset
    
    SendCommand(0x40);     	        // Set Display Start Line

    SendCommand(0x8D);              // Set Charge Pump
    SendCommand(0x14);              // Charge Pump (0x10 External, 0x14 Internal DC/DC)
    
    Rotate(iRotate);
    
	SendCommand(0xDA);              // Set COM Hardware Configuration
	SendCommand(0x12);              // COM Hardware Configuration

	SendCommand(0x81);              // Set Contrast
	SendCommand(0xCF);              // Contrast

	SendCommand(0xD9);              // Set Pre-Charge Period
	SendCommand(0xF1);              // Set Pre-Charge Period (0x22 External, 0xF1 Internal)

	SendCommand(0xDB);              // Set VCOMH Deselect Level
	SendCommand(0x40);              // VCOMH Deselect Level

	SendCommand(0xA4);              // Enable display outputs according to the GDDRAM contents
	SendCommand(0xA6);              // Set display not inverted

	SendCommand(0xAF);              // Set display On

    ClearDisplay();
    
    //////////////////////////////////////
    // Set default font
    SetFont(fntSmall);
}

void UpdateDisplay(void)
{
    int i, iPage, iSegment;
  
    for (i = iPage = 0; iPage < PAGE_COUNT; ++iPage) {
        SetPage(iPage);
        SetSegment(0);
        for (iSegment = 0; iSegment < SEGMENT_COUNT; ++iSegment)
            SendData(g_pDisplayData[i++]);
    }
}
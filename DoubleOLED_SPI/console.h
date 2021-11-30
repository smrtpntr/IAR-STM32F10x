#ifndef CONSOLE_H
#define CONSOLE_H

typedef enum {
    ddDisplay0,
    ddDisplay1,
} DisplayDevice;

void InitConsole(void);
void SetDisplayDevice(DisplayDevice iDisplayDevice);

#endif  // CONSOLE_H
#ifndef LCD_H
#define LCD_H

void HourHandSetup(void);

void MinuteHandSetup(void);

void drawStopwatch(unsigned char Hour, unsigned short Minute, unsigned short Second);

void drawDigital(unsigned char Hour, unsigned short Minute);

void drawAnalog(unsigned char Hour, unsigned short Minute, unsigned short color);

void drawAnalogSecond(unsigned short second, unsigned short color);

void drawClock(void);
#endif

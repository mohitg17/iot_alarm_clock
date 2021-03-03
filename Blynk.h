#ifndef BLYNK_H
#define BLYNK_H
void Blynk_Init(void(*setMode)(), void(*setAlarm)(), void(*setMinute)(), void(*setHour)(), void(*sound)());
void UpdateTime (unsigned char hour, unsigned short minute, unsigned short second);

#endif

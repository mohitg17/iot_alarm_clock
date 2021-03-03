#ifndef SWITCH_H
#define SWITCH_H

void SwitchInit(void(*setMode)(), void(*setAlarm)(), void(*setMinute)(), void(*setHour)(), void(*sound)());

void GPIOPortB_Handler(void);

#endif

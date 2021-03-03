#ifndef SPEAKER_H
#define SPEAKER_H

void PortDInit(void);

void SpeakerTimerInit(void);

void SysTick_Handler(void);

void SpeakerOff(void);

void SpeakerOn(void);

#endif

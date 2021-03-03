// Lab3Main.c
// Runs on LM4F120/TM4C123
// Mohit Gupta & Hans Robles
// Feb 26, 2021

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/PLL.h"
#include "../inc/ST7735.h"
#include "../inc/ST7735_PortD.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"
#include "../inc/Texas.h"
#include "LCD.h"
#include "LED.h"
#include "Timer.h"
#include "Switch.h"
#include "Speaker.h"
#include "Blynk.h"

uint32_t counter = 0;           // counter for LED heartbeat

uint8_t Mode = 0;               // 0 - clock, 1 - alarm, 2 - stopwatch
uint8_t AlarmSetting = 0;       // 0 - unset, 1 - set
uint8_t Sound = 0;              // 0 - off, 1 - on

struct Time {
	uint8_t Hour;
	uint16_t Minute;
	uint16_t Second;
};

struct Time ClockTime;
struct Time AlarmTime;
struct Time StopwatchTime;

uint8_t AlarmHour = 0;
uint16_t AlarmMinute = 0; 

bool StopwatchStart = false;         // flag for stopwatch status
bool Ticked = false;

// toggles between normal and alarm mode
void setMode(void) {
  Mode = (Mode + 1) % 3;
}

void setAlarm(void) {
	DisableInterrupts();
  switch(Mode){
    case 1:
      AlarmSetting = 1;                      // set alarm flag      
      AlarmHour = AlarmTime.Hour;                // store alarm hour
      AlarmMinute = AlarmTime.Minute;            // store alarm minute
      break;
    case 2:
      StopwatchStart = !StopwatchStart;      // flip stopwatch status
      break;
  }
	EnableInterrupts();
}

//move hour hand
void setHour(void) {
  switch(Mode){
    case 0:
      ClockTime.Hour = (ClockTime.Hour + 1)%12;
      break;
    case 1:
      AlarmTime.Hour = (AlarmTime.Hour + 1)%12;
      break;
  }
}

// move minute hand
void setMinute(void) {
  switch(Mode){
    case 0:
      ClockTime.Minute = (ClockTime.Minute + 1)%60;
      break;
    case 1:
      AlarmTime.Minute = (AlarmTime.Minute + 1) %60;
      break;
  }
}

void turnOffAlarm(void) {
  switch(Mode){
    case 2:
      if(!StopwatchStart){
        StopwatchTime.Hour = 0;
        StopwatchTime.Minute = 0;
        StopwatchTime.Second = 0;
      }
      break;
    default:
			DisableInterrupts();
      AlarmSetting = 0;                           // clear alarm flag
      Mode = 0;                                   // set to clock mode
      Sound = 0;
      SpeakerOff();
			EnableInterrupts();
      break;
  }
}

void tick(void) {  
  Ticked = true;
  ClockTime.Second = (ClockTime.Second + 1) % 60;
  
  // increments stopwatch value if stopwatch is playing
  if(StopwatchStart){
    StopwatchTime.Second = (StopwatchTime.Second + 1) % 60;
    if(StopwatchTime.Second == 0){
      StopwatchTime.Minute = (StopwatchTime.Minute + 1) % 60;
    }
    if(StopwatchTime.Minute == 0 && StopwatchTime.Second == 0){
      StopwatchTime.Hour = (StopwatchTime.Hour + 1) % 100;
    }
  }  
  
  // changes minute value if 60 seconds pass by
  if(ClockTime.Second == 0){
    ClockTime.Minute = (ClockTime.Minute + 1) % 60;
    if(ClockTime.Minute == 0){
      ClockTime.Hour = (ClockTime.Hour + 1) % 12;
    }
  }
  
  //rings alarm if time is reached
  if(AlarmSetting){
    if(ClockTime.Hour == AlarmHour){
      if(ClockTime.Minute == AlarmMinute){
        Sound = 1;
        SpeakerOn();
      }
    }
  }
}

int main(void){
	Blynk_Init();
//  ST7735_InitR(INITR_REDTAB);   // initialize ST7735 display
//  Timer0A_Init(&tick);          // initialize clock timer
//  PortDInit();                  // initialize PortD for speaker
//  SpeakerTimerInit();           // initialize speaker SysTick timer     
//  SwitchInit(&setMode, &setAlarm, &setMinute, &setHour, &turnOffAlarm);  // initialize 5 switches
//  LED_Init();                   // initialize LED heartbeat
//	
//	ClockTime.Hour = 10;
//	ClockTime.Minute = 10;
//	ClockTime.Second = 50;

//	AlarmTime.Hour = 10;             // holds hour value in alarm mode
//	AlarmTime.Minute = 11;          // hold minute value in alarm mode

//	StopwatchTime.Hour = 0;
//	StopwatchTime.Minute = 0;
//	StopwatchTime.Second = 0;
//  
//  HourHandSetup();              // setup hour hand coordinates
//  MinuteHandSetup();            // setup minute hand coordinates
//  drawClock();                  																// draw analog clock face
//  drawDigital(ClockTime.Hour, ClockTime.Minute);    						// draw digital time
//  drawAnalog(ClockTime.Hour, ClockTime.Minute, ST7735_WHITE);   // draw clock hands
//  drawAnalogSecond(ClockTime.Second, ST7735_RED);     					// draw second hand
//  ST7735_SetCursor(8,0);
//  ST7735_OutString("Clock");
//  
//  while(1) {
//    if(counter == 2000000) {            //flip the LED bit (PF2) every 2000000 runs of while loop
//      LED_Beat();
//      counter = 0;
//    } else {
//      counter++;
//    }
//    if(Ticked){
//      if(Sound == 1) {
//        ST7735_FillScreen(ST7735_BLACK);
//        ST7735_SetCursor(6, 8);
//        ST7735_OutString("ALARM RING");
//      }
//      else {
//        ST7735_FillScreen(ST7735_BLACK);
//        drawClock();
//        ST7735_SetCursor(6, 0);
//        switch(Mode){
//          case 0:
//            ST7735_OutString("  Clock  ");
//					  DisableInterrupts();
//            drawAnalog(ClockTime.Hour, ClockTime.Minute, ST7735_WHITE);                      // draw clock hands
//            drawAnalogSecond(ClockTime.Second, ST7735_RED);                                  // draw second hand
//            drawDigital(ClockTime.Hour, ClockTime.Minute);                                   // write digital time
//					  EnableInterrupts();
//            break;
//          case 1:
//            ST7735_OutString("  Alarm  ");
//					  DisableInterrupts();
//            drawAnalog(AlarmTime.Hour, AlarmTime.Minute, ST7735_WHITE);          // draw alarm hour and minute hands
//            drawDigital(AlarmTime.Hour, AlarmTime.Minute);                       // write digital alarm time
//					  EnableInterrupts();
//            break;
//          case 2:
//            ST7735_OutString("Stopwatch");
//					  DisableInterrupts();
//            drawAnalog(StopwatchTime.Hour, StopwatchTime.Minute, ST7735_WHITE);    // draw stopwatch hands
//            drawAnalogSecond(StopwatchTime.Second, ST7735_RED);
//            drawStopwatch(StopwatchTime.Hour, StopwatchTime.Minute, StopwatchTime.Second);    // draw stopwatch digital time
//					  EnableInterrupts();
//            break;
//        }
//        if(AlarmSetting == 1){
//          ST7735_SetCursor(20, 0);
//          ST7735_SetTextColor(ST7735_RED);
//          ST7735_OutString("*");                 // display alarm set indicator
//          ST7735_SetTextColor(ST7735_YELLOW);    // reset text color
//        }
//      }
//      Ticked = false;
//    }
//  }
	while (1) {
		WaitForInterrupt();
	}
}

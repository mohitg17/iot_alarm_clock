// hardware connections
// SW1 connected to PB3, 3.3V, 10k resistor, ground
// SW2 connected to PB4, 3.3V, 10k resistor, ground
// SW3 connected to PB5, 3.3V, 10k resistor, ground
// SW4 connected to PB6/PB2 (PB2 backup for PB6), 3.3V, 10k resistor, ground
// SW5 connected to PB7, 3.3V, 10k resistor, ground

#include <stdio.h>
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/PLL.h"
#include "../inc/ST7735.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"

void (*SetMode)();
void (*SetAlarm)();
void (*SetMinute)();
void (*SetHour)();
void (*TurnOffAlarm)();

static void GPIOArm(){
  GPIO_PORTB_ICR_R = 0xBC;      // (e) clear flag
  GPIO_PORTB_IM_R |= 0xBC;      // (f) arm interrupts
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFFF00FFF)|0x00002000; // (g) priority 1
  NVIC_EN0_R = 0x00000002;      // (h) enable interrupt 1 in NVIC	
}

void SwitchInit(void(*setMode)(), void(*setAlarm)(), void(*setMinute)(), void(*setHour)(), void(*sound)()){
	SYSCTL_RCGCGPIO_R |= 0x00000002;
	while((SYSCTL_RCGCGPIO_R & 0x00000002) == 0) {};
		
	SetMode = setMode;
	SetAlarm = setAlarm;
	SetMinute = setMinute;
	SetHour = setHour;
	TurnOffAlarm = sound;

	GPIO_PORTB_CR_R = 0xBC;				// allow changes to PB7-3
	GPIO_PORTB_AMSEL_R &= ~0xBC;	// disable analog functionality on PD0	
	GPIO_PORTB_DIR_R = 0x43;			// make PB7-3 inputs
	GPIO_PORTB_DEN_R = 0xBC;			// enable digital I/O on PB7-3
	GPIO_PORTB_PCTL_R &= ~0xFFFFF000; // configure PB7-3 as GPIO
  GPIO_PORTB_IS_R &= ~0xBC;     // (d) PB7-3 is edge-sensitive
  GPIO_PORTB_IBE_R &= ~0xBC;    //     PB7-3 is not both edges
  GPIO_PORTB_IEV_R |= 0xBC;     //     PB7-3 rising edge event
  GPIO_PORTB_ICR_R = 0xBC;      // (e) clear flag7-3
	GPIO_PORTB_IM_R |= 0xBC;      // (f) arm interrupt on PB7-3 *** No IME bit as mentioned in Book ***

	NVIC_PRI0_R = (NVIC_PRI0_R&0xFFF00FFF)|0x00002000; // (g) priority 1
  NVIC_EN0_R = 0x00000002;      // (h) enable interrupt 1 in NVIC	
	EnableInterrupts();
}

// ***************** TIMER1A_Init ****************
// Activate TIMER1A interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
//          priority 0 (highest) to 7 (lowest)
// Outputs: none
void Timer1A_Init(){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x0000001;    // 3) 1-SHOT mode
  TIMER1_TAILR_R = 20000000;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|(4<<13); // priority 
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

void Timer1A_Handler(void){
  TIMER1_IMR_R = 0x00000000;    // disarm timeout interrupt
	GPIOArm();                    // re-arm switch interrupts
}

void GPIOPortB_Handler(void){
	if(GPIO_PORTB_RIS_R & 0x80) {    // if SW5 pressed, change mode
		GPIO_PORTB_IM_R &= ~0x80;
		(*SetMode)();
		Timer1A_Init();                // one-shot timer initialization for debouncing
	}
	if(GPIO_PORTB_RIS_R & 0x04) {    // if SW4 pressed, set alarm
		GPIO_PORTB_IM_R &= ~0x04;
		(*SetAlarm)();
		Timer1A_Init();                // one-shot timer initialization for debouncing
	}
	if(GPIO_PORTB_RIS_R & 0x20) {    // if SW3 pressed, increment minute
		GPIO_PORTB_IM_R &= ~0x20;
		(*SetMinute)();
		Timer1A_Init();                // one-shot timer initialization for debouncing
	}
	if(GPIO_PORTB_RIS_R & 0x10) {    // if SW2 pressed, increment hour
		GPIO_PORTB_IM_R &= ~0x10;
		(*SetHour)();
		Timer1A_Init();                // one-shot timer initialization for debouncing
	}
	if(GPIO_PORTB_RIS_R & 0x08) {    // if SW1 pressed, turn off alarm or start stopwatch
		GPIO_PORTB_IM_R &= ~0x08;
		(*TurnOffAlarm)();
		Timer1A_Init();                // one-shot timer initialization for debouncing
	}
}



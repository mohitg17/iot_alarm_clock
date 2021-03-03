// hardware connections
// **********Speaker*******************
// PD0 connected to MOSFET
// MOSFET connected to speaker
// MOSTFET connected to ground
// 3.3V connected to speaker

#include <stdio.h>
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/PLL.h"
#include "../inc/ST7735.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"

void PortDInit(void){
	SYSCTL_RCGCGPIO_R |= 0x08;        			// activate clock for Port D
	while((SYSCTL_PRGPIO_R&0x08) == 0){};		// Wait for clock to be ready
	GPIO_PORTD_LOCK_R = 0x4C4F434B;
	GPIO_PORTD_CR_R = 0x01;				          // allow changes to PD0		
	GPIO_PORTD_AMSEL_R &= ~0x01;						// disable analog functionality on PD0
	GPIO_PORTD_PCTL_R &= ~0x0000FFFF;				// GPIO
	GPIO_PORTD_DIR_R |= 0x01;								// Make PD0 an output
	GPIO_PORTD_DEN_R |= 0x01;								// PD0 digital port enabled
}

void SpeakerTimerInit(void){
	NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 90909;  			// maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it   
  NVIC_SYS_PRI3_R = ((NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x20000000);
	// NVIC_ST_CTRL_R = 0x07;								// enable SysTick with core clock
}

void SpeakerOn(void) {
	NVIC_ST_CTRL_R = 0x07;					// enable SysTick with core clock
}

void SpeakerOff(void) {
	NVIC_ST_CTRL_R = 0;							// disable SysTick with core clock
}

void SysTick_Handler(void){
	GPIO_PORTD_DATA_R ^= 0x01;
}

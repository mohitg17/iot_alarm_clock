#include <stdio.h>
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/PLL.h"
#include "../inc/ST7735.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"

void LED_Init(void) {
	SYSCTL_RCGCGPIO_R |= 0x00000020;
	while((SYSCTL_RCGCGPIO_R & 0x00000020) == 0) {};
		
	GPIO_PORTF_DIR_R = 0x04;			// make PE3 output
	GPIO_PORTF_DEN_R = 0x04;			// enable digital I/O on PE3
	GPIO_PORTF_PCTL_R &= ~0x00000F00; // configure PE3 as GPIO
}

void LED_Beat(void) {
	GPIO_PORTF_DATA_R ^= 0x04;
}

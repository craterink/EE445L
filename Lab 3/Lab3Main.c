#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../tm4c123gh6pm.h"
#include "PLL.h"
#include "stddef.h"
#include "ST7735.h"
#include "plotter.h"
#include "EventWatch.h"
#include "ClockTime.h"
#include "Screens.h"
#include "Alarm.h"

#define SWITCHES                (*((volatile unsigned long *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board
#define PF1       (*((volatile uint32_t *)0x40025008))


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

/* 	PortF will be used for the enter key (PF4)
		and heartbeat (PF1)
		
		PortE will be used for up and down keys (PE1up, PE2 down)
		PE3 will be used for speaker
*/
void initForLab3(void) {
	PLL_Init();                           // 80 MHz
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // activate port F
	SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
  GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in (built-in button)
	GPIO_PORTF_DIR_R |= 0x02;			//			make PF1 LED output (heartbeat
  GPIO_PORTF_AFSEL_R &= ~0x12;  //     disable alt funct on PF4, PF1
  GPIO_PORTF_DEN_R |= 0x12;     //     enable digital I/O on PF4, PF1   
  GPIO_PORTF_PCTL_R &= ~0x000F00F0; // configure PF4, PF1 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    //     falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flags
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00200000; // (g) priority 1
  NVIC_EN0_R |= 0x40000000;      // (h) enable interrupt 30 in NVIC
	
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE; // activate port E
	SYSCTL_RCGCGPIO_R |= 0x00000010; // (a) activate clock for port E
  GPIO_PORTE_DIR_R &= ~0x06;    // (c) make PE1 and PE2 in 
	GPIO_PORTE_DIR_R |= 0x08;			//			PE3 used as output
  GPIO_PORTE_AFSEL_R &= ~0x0E;  //     disable alt funct on PE1, PE2
  GPIO_PORTE_DEN_R |= 0x0E;     //     enable digital I/O on PE1, PE2 
  GPIO_PORTE_PCTL_R &= ~0x0000FFF0; // configure PE1, PE2 as GPIO
  GPIO_PORTE_AMSEL_R = 0;       //     disable analog functionality on PE
  GPIO_PORTE_PUR_R |= 0x06;     //     enable weak pull-up on PE1, PE0
  GPIO_PORTE_IS_R &= ~0x06;     // (d) edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x06;    //     not both edges
  GPIO_PORTE_IEV_R &= ~0x06;    //     falling edge event
  GPIO_PORTE_ICR_R = 0x06;      // (e) clear flags
  GPIO_PORTE_IM_R |= 0x06;      // (f) arm interrupt on PE0, PE1 
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x00000020; // (g) priority 1
  NVIC_EN0_R |= 0x00000010;      // (h) enable interrupt 29 in NVIC
  EnableInterrupts();           // (i) Clears the I bit
	
}


int main(void){
  initForLab3();
	Screens_Init();
	ClockInit();
	EventWatch_Init();
	
	while(1){
		WaitForInterrupt();
	}

}

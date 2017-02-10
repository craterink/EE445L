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

#define SWITCHES                (*((volatile unsigned long *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

/* 	PortF will be used for all 3 buttons (polling system)
		PF0 (SW2) -  down
		PF1 (external) - enter
		PF4 (SW1) - up
*/
void initForLab3(void) {
	PLL_Init();                           // 80 MHz
	
	
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // activate port F
 // SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
 // while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
      /*                              // 2) no need to unlock PF2, PF4
  GPIO_PORTF_PCTL_R &= ~0x000F00FF; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x13;      // 4) disable analog function on PF0, PF1, PF4
  GPIO_PORTF_PUR_R |= 0x13;         // 5) pullup for PF4, PF1, PF0
  GPIO_PORTF_DIR_R &= ~0x13;         // 5) set direction to input
  GPIO_PORTF_AFSEL_R &= ~0x13;      // 6) regular port function
	GPIO_PORTF_DEN_R |= 0x13;         // 7) enable digital port
	GPIO_PORTF_IS_R &= ~0x13;					//		edge senstive
	GPIO_PORTF_IBE_R &= ~0x13;				//		not both edges
	GPIO_PORTF_IEV_R |= 0x13;					//		rising edge triggers
	GPIO_PORTF_ICR_R = 0x13;					// 		clear flags
	GPIO_PORTF_IM_R |= 0x13;					//		enable interrupts
	NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00200000;		//priority 1
	NVIC_EN0_R = 0x40000000;					//		enable interrupts for PortF
	EnableInterrupts();
		*/
		  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
  GPIO_PORTF_DIR_R &= ~0x13;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x13;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x13;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000F00FF; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x13;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x13;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x13;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x13;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x13;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x13;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00200000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
  EnableInterrupts();           // (i) Clears the I bit
	
}

void GPIOPortF_Handler(void){
 	GPIO_PORTF_IM_R &= ~0x13;     // disarm interrupts to debounce/prevent critical sections, if any 	
	if(GPIO_PORTF_RIS_R&0x01){
		GPIO_PORTF_ICR_R = 0x01;			//clear flag
		down();												//act accordingly
		drawCurrentScreen();		
	}
	if(GPIO_PORTF_RIS_R&0x02){
		GPIO_PORTF_ICR_R = 0x02;
		enter();
		drawCurrentScreen();
	}
	if(GPIO_PORTF_RIS_R&0x10){
		GPIO_PORTF_ICR_R = 0x10;
		up();
		drawCurrentScreen();
	}
	
	GPIO_PORTF_IM_R |= 0x13;     // re-enable
}


int main(void){
  initForLab3();
	Screens_Init();
	ClockInit();
	EventWatch_Init();
	
	while(1){
		WaitForInterrupt();
	}
	/*
	RegisterHandler(EVENT_MINUTE_TICK, MinuteTick);
	RegisterHandler(EVENT_MINUTE_TICK, drawCurrentScreen);
	*/
}

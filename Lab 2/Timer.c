/*	timer.c
		Rikin and Cooper
		this file focuses on timer initiliazations
*/

#include <stdint.h>
#include "ADCSWTrigger.h"
#include "../tm4c123gh6pm.h"
#include "PLL.h"
#include "stddef.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

void (*PeriodicTask0A)(void);   
void (*PeriodicTask0B)(void);   
void (*PeriodicTask1A) (void);
void (*PeriodicTask1B)(void);   
void (*PeriodicTask2A)(void);   
void (*PeriodicTask2B) (void);


void Timer_Init(void(*task)(void), int period, int timer, int priority){
	if(timer == 0x0A){		//timer0A init
		long sr;
		sr = StartCritical(); 
		SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
		PeriodicTask0A = task;          // user function
		TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
		TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
		TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
		TIMER0_TAILR_R = period-1;    // 4) reload value
		TIMER0_TAPR_R = 0;            // 5) bus clock resolution
		TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
		TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
		
		NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // 8) priority 2
	// interrupts enabled in the main program after all devices initialized
	// vector number 35, interrupt number 19
		NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
		TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
		EndCritical(sr);
	}
}
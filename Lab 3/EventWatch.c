#include <stdint.h>
#include "../tm4c123gh6pm.h"

// event registry
#define NUM_EVENTS 5
#define NUM_HANDLERS 8
int8_t handlerCounts[NUM_EVENTS]; // keeps track of number of registered handlers for each event
void(*eventRegistry[NUM_EVENTS][NUM_HANDLERS])(void); // can register up to 8 handlers for each event

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


void (*PeriodicTask)(void);   // user function

// ***************** Timer0A_Init ****************
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer0A_Init(void(*task)(void), uint32_t period){long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  PeriodicTask = task;          // user function
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = period-1;    // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
  EndCritical(sr);
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
  (*PeriodicTask)();                // execute user task
}

void Timer0A_UpdateTask(void(*task)(void)){
  PeriodicTask = task;
}

// TO DO
void eventCheck(void) {
	
}

void notifyEvent(uint8_t event) {
	int i;
	if(event < NUM_EVENTS) {
		
		// call all handlers for particular event
		for(i = 0; i < handlerCounts[event]; i++) {
			(eventRegistry[event][i])();
		}
	}
}

// TO CHECK
void EventWatch_Init(void) {
	Timer0A_Init(eventCheck, 80000); // interrupt every 80000 * 12.5ns = 1 millisecond
}

// TO CHECK
void RegisterHandler(uint8_t event, void (*handlerFn)(void)) {
	if(event < NUM_EVENTS && handlerCounts[event] < NUM_HANDLERS) {
		eventRegistry[event][handlerCounts[event]] = handlerFn;
		handlerCounts[event] += 1;
	}
}



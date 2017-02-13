#include <stdint.h>
#include "../tm4c123gh6pm.h"
#include "EventWatch.h"
#include "Screens.h"
#include "ClockTime.h"
#include "Alarm.h"


// event registry
#define NUM_EVENTS 5
#define NUM_HANDLERS 8
#define PF1       (*((volatile uint32_t *)0x40025008))

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
  DisableInterrupts();
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
  NVIC_EN0_R |= 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
	EnableInterrupts();
  
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
  (*PeriodicTask)();                // execute user task
}

void Timer0A_UpdateTask(void(*task)(void)){
  PeriodicTask = task;
}


uint16_t counter = 0;
void minuteUpdate(void) {
	struct time update;
	counter++;
	if(counter == 60000) {
		counter = 0;
		MinuteTick();
		checkAlarm(); 
		drawCurrentScreen();
	}
	if(counter % 800 == 0) {	//heartbeat
		PF1 ^= 0x02;
	}
	if(PF1 == 0x02 && counter % 7 == 0){
		PF1 = 0x00;
	}
	
}


void EventWatch_Init(void) {
	Timer0A_Init(minuteUpdate, 80000); // interrupt every 80000 * 12.5ns = 1 millisecond
}



/* ***************** BUTTONS *******************
		the following functions edit the current screen based 
		a button press.
*/


void up(void){
	struct time update;
	
	if(getCurrentScreen() == MAIN_MENU){
		if(getCurrentMenuOpt() == MENU_ANALOG){
			setCurrentMenuOpt(MENU_SETTINGS);
		}
		else{
			setCurrentMenuOpt(getCurrentMenuOpt()-1);
		}
	}
	
	if(getCurrentScreen() == SETTINGS){
		if(getCurrentSettingsOpt2() == SETTINGS_DEFAULT){
			if(getCurrentSettingsOpt() == SETTINGS_CLOCK) setCurrentSettingsOpt(SETTINGS_ALARM);
			else setCurrentSettingsOpt((SETTINGS_CLOCK));
		}
		else{
			if(getCurrentSettingsOpt() == SETTINGS_CLOCK){
				if(getCurrentSettingsOpt2() == SETTINGS_HOURS){
					update = getSettingTime();
					update.hour = (update.hour+1)%24;
					setSettingTime(update);
				}
				else if(getCurrentSettingsOpt() == SETTINGS_MINUTES){
					update = getSettingTime();
					update.minute = (update.minute+1)%60;
					setSettingTime(update);
				}
			}
			else if(getCurrentSettingsOpt() == SETTINGS_ALARM){
				if(getCurrentSettingsOpt() == SETTINGS_CLOCK){
					if(getCurrentSettingsOpt2() == SETTINGS_HOURS){
						update = getAlarmSettingTime();
						update.hour = (update.hour+1)%24;
						setAlarmSettingTime(update);
					}
					else if(getCurrentSettingsOpt() == SETTINGS_MINUTES){
						update = getAlarmSettingTime();
						update.minute = (update.minute+1)%60;
						setAlarmSettingTime(update);
					}
				}
			}
		}
	}
	
	else if(getCurrentScreen() == ANALOG_CLOCK){
		setCurrentScreen(DIGITAL_CLOCK);
	}
	
	else if(getCurrentScreen() == DIGITAL_CLOCK){
		setCurrentScreen(ANALOG_CLOCK);
	}
}

void down(void){
	struct time update;
	if(getCurrentScreen() == MAIN_MENU){
			setCurrentMenuOpt((getCurrentMenuOpt()+1)%3);
	}
	
	else if(getCurrentScreen() == SETTINGS){
		if(getCurrentSettingsOpt2() == SETTINGS_DEFAULT){
			setCurrentSettingsOpt((getCurrentSettingsOpt()+1)%2);
		}
		else{
			if(getCurrentSettingsOpt() == SETTINGS_CLOCK){
				if(getCurrentSettingsOpt2() == SETTINGS_HOURS){
					update = getSettingTime();
					if(update.hour == 0) update.hour = 23;
					else update.hour++;
					setSettingTime(update);
				}
				else if(getCurrentSettingsOpt() == SETTINGS_MINUTES){
					update = getSettingTime();
					if(update.minute == 0) update.hour = 59;
					else update.minute++;
					setSettingTime(update);
				}
			}
			else if(getCurrentSettingsOpt() == SETTINGS_ALARM){
				if(getCurrentSettingsOpt() == SETTINGS_CLOCK){
					if(getCurrentSettingsOpt2() == SETTINGS_HOURS){
						update = getAlarmSettingTime();
						if(update.hour == 0) update.hour = 23;
						else update.hour++;
						setAlarmSettingTime(update);
					}
					else if(getCurrentSettingsOpt() == SETTINGS_MINUTES){
						update = getAlarmSettingTime();
						if(update.minute == 0) update.hour = 59;
						else update.minute++;
						setAlarmSettingTime(update);
					}
				}
			}
		}
	}
	
	else if(getCurrentScreen() == ANALOG_CLOCK){
		setCurrentScreen(DIGITAL_CLOCK);
	}
	
	else if(getCurrentScreen() == DIGITAL_CLOCK){
		setCurrentScreen(ANALOG_CLOCK);
	}
}

void enter(void){
	if(getCurrentScreen() == MAIN_MENU){
		uint8_t menu = getCurrentMenuOpt();
		if(menu == MENU_DIGITAL){
			setCurrentScreen(DIGITAL_CLOCK);
		}
		else if(menu == MENU_ANALOG){
			setCurrentScreen(ANALOG_CLOCK);
		}
		else if(menu == MENU_SETTINGS){
			setCurrentScreen(SETTINGS);
		}
	}
	
	else if(getCurrentScreen() == SETTINGS){
		setCurrentSettingsOpt2(getCurrentSettingsOpt2()+1);
		if(getCurrentSettingsOpt2() == SETTINGS_DONE){
			setCurrentSettingsOpt2(SETTINGS_DEFAULT);
			if(getCurrentSettingsOpt() == SETTINGS_CLOCK) setCurrentTime(getSettingTime());
			else setAlarmTime(getAlarmSettingTime());
		}
	}
	
	else if(getCurrentScreen() == ANALOG_CLOCK){
		setCurrentScreen(MAIN_MENU);
	}
	
	else if(getCurrentScreen() == DIGITAL_CLOCK){
		setCurrentScreen(MAIN_MENU);
	}
}


/* ******************HANDLERS*************
		THE FOLLOWING FUNCTIONS ARE HANDLERS FOR BUTTONS INTERRUPTS
*/

void GPIOPortF_Handler(void){
	GPIO_PORTF_IM_R &= ~0x10;     // disarm interrupts to debounce/prevent critical sections, if any 	

	if(GPIO_PORTF_RIS_R&0x10){
		GPIO_PORTF_ICR_R = 0x10;
		enter();
		drawCurrentScreen();
	}
	
	GPIO_PORTF_IM_R |= 0x10;     // re-enable
}

void GPIOPortE_Handler(void){
	GPIO_PORTE_IM_R &= ~0x06;		// disarm interrupts to debounce/prevent critical sections, if any 
	
	if(GPIO_PORTE_RIS_R&0x04){
		GPIO_PORTE_ICR_R = 0x04;			//clear flag
		down();												//act accordingly
		drawCurrentScreen();		
	}
	
	if(GPIO_PORTE_RIS_R&0x02){
		GPIO_PORTE_ICR_R = 0x02;
		up();
		drawCurrentScreen();
	}
	
	GPIO_PORTE_IM_R |= 0x06;     // re-enable
	
}





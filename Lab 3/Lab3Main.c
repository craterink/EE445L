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

#define SWITCHES                (*((volatile unsigned long *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board


// TODO: What ports/stuff do we need to initialize here?
void initForLab3(void) {
	PLL_Init();                           // 80 MHz
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // activate port F
  SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2, PF4
  GPIO_PORTF_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x14;      // 4) disable analog function on PF2, PF4
  GPIO_PORTF_PUR_R |= 0x10;         // 5) pullup for PF4
  GPIO_PORTF_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x14;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x14;         // 7) enable digital port
}

void initST7735() {
	ST7735_InitR(INITR_REDTAB);
}

int main(void){
  initForLab3();
	initST7735();
	
	EventWatch_Init();
	RegisterHandler(EVENT_MINUTE_TICK, &MinuteTick);
}

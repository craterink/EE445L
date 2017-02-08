#include <stdint.h>
#include "../tm4c123gh6pm.h"

_Bool pc4Val = 0;
void InitPC4(void) {
	int delay;
	SYSCTL_RCGC2_R |= 0x00000000;     // 1) activate clock for Port C
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTC_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port C
  GPIO_PORTC_CR_R = 0x1F;           // allow changes to PC4-0
  GPIO_PORTC_AMSEL_R = 0x00;        // 3) disable analog on PC
  GPIO_PORTC_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PC4-0
  GPIO_PORTC_DIR_R = 0x0E;          // 5) PC4 out
  GPIO_PORTC_AFSEL_R = 0x00;        // 6) disable alt funct on PC4
  GPIO_PORTC_DEN_R = 0x1F;          // 7) enable digital I/O on PC4
}

void alarmInit(void) {
	InitPC4();
}

void toggleAlarm(void) {
	pc4Val = 1 - pc4Val; // toggle
	
	pc4Val ? (GPIO_PORTC_DATA_R |= 0x10) : (GPIO_PORTC_DATA_R &= 0xEF); // set bit 4 to pc4Val
}
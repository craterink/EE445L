#include <stdint.h>
#include "../tm4c123gh6pm.h"

#define PE3  (*((volatile unsigned long *)0x40024020))



void toggleAlarm(void) {
	PE3 ^= 0x08;
}
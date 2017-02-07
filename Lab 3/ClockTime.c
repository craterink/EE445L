#include <stdint.h>
#include "../tm4c123gh6pm.h"
#include "EventWatch.h"

struct time {
   uint8_t hour;
   uint8_t minute;
};

// clock
struct time currentTime;

// alarm
struct time alarmTime;
_Bool alarmEnabled;

void checkAlarm(void) {
	if(alarmEnabled) {
		if(currentTime.hour == alarmTime.hour &&
						currentTime.minute == alarmTime.minute) {
			
		}
	}
}

void MinuteTick(void) {
	currentTime.minute += 1;
	if(currentTime.minute == 60) {
		currentTime.minute = 0;
		currentTime.hour += 1;
		if(currentTime.hour == 24) {
			currentTime.hour = 0;
		}
	}
	
	checkAlarm();
}

void ClockInit(void) {
	currentTime.hour = 0;
	currentTime.minute = 0;
	
	alarmTime.hour = 0;
	alarmTime.minute = 0;
	alarmEnabled = 0;
	
	RegisterHandler(EVENT_MINUTE_TICK, &MinuteTick);
}


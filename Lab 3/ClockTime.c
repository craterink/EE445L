#include <stdint.h>
#include "../tm4c123gh6pm.h"
#include "EventWatch.h"
#include "ClockTime.h"

// clock
struct time currentTime;

// alarm
struct time alarmTime;
_Bool alarmEnabled;

void checkAlarm(void) {
	if(alarmEnabled) {
		if(currentTime.hour == alarmTime.hour &&
						currentTime.minute == alarmTime.minute) {
			//ring alarm
		}
	}
}
/*
void HourTickAlarm(void){
	alarmTime.hour = (currentTime.hour + 1)%24;
}

void MinuteTickAlarm(void){
	alarmTime.minute += 1;
	if(alarmTime.minute == 60) {
		alarmTime.minute = 0;
		alarmTime.hour += 1;
		if(alarmTime.hour == 24) {
			alarmTime.hour = 0;
		}
	}
	
}

void HourTick(void){
	currentTime.hour = (currentTime.hour + 1)%24;
}
*/
void MinuteTick(void) {
	currentTime.minute += 1;
	if(currentTime.minute == 60) {
		currentTime.minute = 0;
		currentTime.hour += 1;
		if(currentTime.hour == 24) {
			currentTime.hour = 0;
		}
	}
}

void ClockInit(void) {
	currentTime.hour = 0;
	currentTime.minute = 0;
	
	alarmTime.hour = 0;
	alarmTime.minute = 0;
	alarmEnabled = 0;
	
	//RegisterHandler(EVENT_MINUTE_TICK, &MinuteTick);
}

struct time getCurrentTime(void) {
	return currentTime;
}

void setCurrentTime(struct time t) {
	currentTime = t;
}

struct time getAlarmTime(void) {
	return alarmTime;
}

void setAlarmTime(struct time t) { 
	alarmTime = t;
}

_Bool isAlarmEnabled(void) {
	return alarmEnabled;
}

void setAlarmEnabled(_Bool enabled) {
	alarmEnabled = enabled;
}
#include <stdint.h>

typedef struct time {
	uint8_t hour;
  uint8_t minute;
} Time;

void MinuteTick(void);

Time getCurrentTime(void);
void setCurrentTime(Time);
Time getAlarmTime(void);
void setAlarmTime(Time);
_Bool isAlarmEnabled(void);
void setAlarmEnabled(_Bool);
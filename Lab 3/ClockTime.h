#include <stdint.h>

struct time {
   uint8_t hour;
   uint8_t minute;
};

// clock
struct time currentTime;

// alarm
struct time alarmTime;
_Bool alarmEnabled;

void MinuteTick(void);
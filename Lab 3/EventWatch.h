#include <stdint.h>

// event definitions
#define EVENT_MINUTE_TICK 0
#define EVENT_LEFT_BTN 1
#define EVENT_UP_BTN 2
#define EVENT_RIGHT_BTN 3
#define EVENT_DOWN_BTN 4

void EventWatch_Init(void);

void RegisterHandler(uint8_t event, uint8_t screen, void (*handlerFn)(void));

void notifyEvent(uint8_t event);
#include <stdint.h>

// screen definitions
#define MAIN_MENU 0
#define SETTINGS 1
#define ANALOG_CLOCK 2
#define DIGITAL_CLOCK 3

uint8_t currentScreen;

void Screens_Init(void);

// screen drawing functions
void drawCurrentScreen(void);


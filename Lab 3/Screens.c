#include <stdint.h>
#include "../tm4c123gh6pm.h"
#include "Screens.h"
#include "ST7735.h"


#define NUM_SCREENS 4

void Screens_Init(void){
	currentScreen = MAIN_MENU;
	
	ST7735_InitR(INITR_REDTAB); // init LCD
	drawCurrentScreen();
}

// screen drawing functions
void drawCurrentScreen(void){
	
}

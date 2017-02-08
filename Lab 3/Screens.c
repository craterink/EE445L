#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../tm4c123gh6pm.h"
#include "Screens.h"
#include "ST7735.h"
#include "ClockTime.h"


uint8_t currentScreen;

uint8_t getCurrentScreen(void){
	return currentScreen;
}

char convertASCII(uint8_t x){
	return '0' + x;
}

void Screens_Init(void){
	currentScreen = MAIN_MENU;
	
	ST7735_InitR(INITR_REDTAB); // init LCD
	drawCurrentScreen();
	
	// RegisterHandler(EVENT_BTN_DOWN, &mainMenu_handleDown);
	// RegisterHandler(EVENT_BTN_UP, &handleUp);
	// RegisterHandler(EVENT_BTN_RIGHT, &handleRight);
}

void drawAnalogClock(void){
}

void drawDigitalClock(void){
	uint8_t minOnes, minTens, hour, hourOnes, hourTens, i;
	char min_ones, min_tens, hour_ones, hour_tens;
	char display[8]; 
	struct time current;
	current = getCurrentTime();  
	
	minOnes = current.minute%10;
	min_ones = convertASCII(minOnes);
	minTens = current.minute/10;
	min_tens = convertASCII(minTens);
	
	display[6] = 'A';		
	hour = current.hour;
	if(hour >= 12){
		display[6] = 'P';
		if(hour != 12) hour -= 12;			//variable state is in military time
	}
	hourOnes = hour%10;
	hour_ones = convertASCII(hourOnes);
	hourTens = hour/10;
	hour_tens = convertASCII(hour_tens);
	
	if(hourTens == 0){
		display[0] = ' ';
	}
	else{
		display[0] = hour_tens;
	}
	
	display[1] = hour_ones;
	display[2] = ':';
	display[3] = min_tens;
	display[4] = min_ones;
	display[5] = ' ';
	display[7] = 'M';
	
	ST7735_FillScreen(ST7735_BLACK);
	for(i = 0; i < 8; i++){
		ST7735_DrawChar(DIGITAL_XLOC + (DIGITAL_XMOVE * i), DIGITAL_YLOC, display[i], ST7735_RED, ST7735_BLACK, DIGITAL_FONT_SIZE);    
	}
}

// main menu vars
uint8_t currentMenuOpt = 0;
uint16_t menuColor;
uint16_t menuFontColor;
char *menuPointer = ">";
char menuItems[3][20] = {MENU_ANALOG_STR, MENU_DIGITAL_STR, MENU_SETTINGS_STR};

uint8_t getCurrentMenuOpt(void){
	return currentMenuOpt;
}

void setCurrentMenuOpt(uint8_t menu){
	currentMenuOpt = menu;
}

void drawMainMenu(void) {
	int i;
	/*menuColor = ST7735_Color565(0,191,255);
	menuFontColor = ST7735_Color565(0,0,139);
	
	ST7735_FillScreen(menuColor);
	Output_Color(menuColor);
	ST7735_SetTextColor(menuFontColor);
	ST7735_SetCursor(0,0);
	ST7735_OutString(MENU_STR);*/
	
	ST7735_FillScreen(ST7735_BLACK);;
	ST7735_SetCursor(0,0);
	ST7735_OutString(MENU_STR);
	ST7735_OutString("\r\r\r");
	
	ST7735_SetCursor(0,50);
	for(i = 0; i < NUM_MENU_ITEMS; i++) {
		if(i == currentMenuOpt) {
			ST7735_OutString(menuPointer);
		} else {
			ST7735_OutString(" ");
		}
		ST7735_OutString(menuItems[i]);
		ST7735_OutString("\r");
	}
}

void setCurrentScreen(uint8_t newScreen) {
	currentScreen = newScreen;
}

void handleDown(void) {
	currentMenuOpt = (currentMenuOpt + 1) % NUM_MENU_ITEMS;
}

void handleUp(void) {
	currentMenuOpt--;
	if(currentMenuOpt < 0)
		currentMenuOpt = NUM_MENU_ITEMS - 1;
}

void handleRight(void) {
	switch(currentMenuOpt) {
		case MENU_ANALOG:
			setCurrentScreen(ANALOG_CLOCK);
			break;
		case MENU_DIGITAL:
			setCurrentScreen(DIGITAL_CLOCK);
			break;
		case MENU_SETTINGS:
			setCurrentScreen(SETTINGS);
			break;
	}
}

// settings vars
uint8_t currentSettingsOpt = 0;
Time settingTime;
Time alarmSettingTime;
_Bool alarmSettingEnabled;
uint8_t currentSettingsOpt2 = 0;

uint8_t getCurrentSettingsOpt(void){
	return currentSettingsOpt;
}

void setCurrentSettingsOpt(uint8_t set){
	currentSettingsOpt = set;
}

uint8_t getCurrentSettingsOpt2(void){
	return currentSettingsOpt2;
}

void setCurrentSettingsOpt2(uint8_t set){
	currentSettingsOpt2 = set;
}

struct time getSettingTime(void){
	return settingTime;
}

void setSettingTime(struct time input){
	settingTime.hour = input.hour;
	settingTime.minute = input.minute;
}

struct time getAlarmSettingTime(void){
	return alarmSettingTime;
}

void setAlarmSettingTime(struct time input){
	alarmSettingTime.hour = input.hour;
	alarmSettingTime.minute = input.minute;
}

void drawSettings(void) {
	char hourStr[5] = "     ", minuteStr[5] = "     ", aHourStr[5] = "     ", aMinuteStr[5] = "     ", *aEnabled, *ampm, *aampm;
	settingTime = getCurrentTime();
	alarmSettingTime = getAlarmTime();
	alarmSettingEnabled = isAlarmEnabled();
	
	// create string versions of time numbers
	ampm = ((settingTime.hour >= 12) ? "PM" : "AM");
	sprintf(hourStr, "%d", ((11 + settingTime.hour) % 12) + 1);
	sprintf(minuteStr, "%d", settingTime.minute);
	aampm = ((alarmSettingTime.hour >= 12) ? "PM" : "AM");
	sprintf(aHourStr, "%d", ((11 + alarmSettingTime.hour) % 12) + 1);
	sprintf(aMinuteStr, "%d", alarmSettingTime.minute);
	aEnabled = ((alarmSettingEnabled == 0) ? "Disabled" : "Enabled");
	
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0,0);
	
	ST7735_OutString("445L Clock Settings\r\r\r");
	
	// ====== Current Time =====
	currentSettingsOpt == 0 ? ST7735_OutString(">") : ST7735_OutString(" "); 
	ST7735_OutString("Current Hour: ");
	ST7735_OutString(hourStr);
	ST7735_OutString(ampm);
	ST7735_OutString("\r");
	currentSettingsOpt == 1 ? ST7735_OutString(">") : ST7735_OutString(" "); 
	ST7735_OutString("Current Minute: ");
	ST7735_OutString(minuteStr);
	ST7735_OutString("\r\r");
	
	// ====== Alarm Time =====
	currentMenuOpt == 2 ? ST7735_OutString(">") : ST7735_OutString(" "); 
	ST7735_OutString("Alarm: ");
	ST7735_OutString(aEnabled);
	ST7735_OutString("\r");
	currentMenuOpt == 3 ? ST7735_OutString(">") : ST7735_OutString(" "); 
	ST7735_OutString("Alarm Hour: ");
	ST7735_OutString(aHourStr);
	ST7735_OutString(aampm);
	ST7735_OutString("\r");
	currentMenuOpt == 4 ? ST7735_OutString(">") : ST7735_OutString(" "); 
	ST7735_OutString("Alarm Minute: ");
	ST7735_OutString(aMinuteStr);
	ST7735_OutString("\r\r");
	
	ST7735_OutString(" ...back to main menu");
}



// screen drawing functions
void drawCurrentScreen(void){
	if(getCurrentScreen() == MAIN_MENU){
		drawMainMenu();
	}
	
	if(getCurrentScreen() == SETTINGS){
		drawSettings();
	}
	
	if(getCurrentScreen() == ANALOG_CLOCK){
		drawAnalogClock();
	}
	
	if(getCurrentScreen() == DIGITAL_CLOCK){
		drawDigitalClock();
	}
	
}



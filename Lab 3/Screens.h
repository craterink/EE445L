#include <stdint.h>

// screen definitions
#define MAIN_MENU 0
#define SETTINGS 1
#define ANALOG_CLOCK 2
#define DIGITAL_CLOCK 3
#define NUM_SCREENS 4

//menu definitions
#define MENU_ANALOG 0
#define MENU_DIGITAL 1
#define MENU_SETTINGS 2
#define NUM_MENU_ITEMS 3
#define MENU_STR "445L Clock Menu"
#define MENU_ANALOG_STR "Analog Clock"
#define MENU_DIGITAL_STR "Digital Clock"
#define MENU_SETTINGS_STR "Settings"
uint8_t getCurrentMenuOpt(void);
void setCurrentMenuOpt(uint8_t menu);

//digital definitions
#define DIGITAL_XLOC 20			//x location for first number of digital clock. Optimize later
#define DIGITAL_YLOC 60			// same
#define DIGITAL_FONT_SIZE 10	
#define DIGITAL_XMOVE 10

//settings
#define SETTINGS_CLOCK 0
#define SETTINGS_ALARM 1
#define SETTINGS_DEFAULT 0
#define SETTINGS_HOURS 1
#define SETTINGS_MINUTES 2
#define SETTINGS_DONE 3
uint8_t getCurrentSettingsOpt(void);
void setCurrentSettingsOpt(uint8_t set);
uint8_t getCurrentSettingsOpt2(void);
void setCurrentSettingsOpt2(uint8_t set);
struct time getSettingTime(void);
struct time getAlarmSettingTime(void);
void setSettingTime(struct time input);
void setAlarmSettingTime(struct time input);

void Screens_Init(void);
void drawCurrentScreen(void);
void drawDigitalClock(void);
void drawMainMenu(void);
void setCurrentScreen(uint8_t newScreen);
void handleDown(void);
void handleUp(void);
void handleRight(void);
void drawSettings(void);
void drawAnalogClock(void);
uint8_t getCurrentScreen(void);


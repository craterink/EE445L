/*
* Lab2.c
*
* By Rikin and Cooper
*
*/

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "ADCSWTrigger.h"
#include "../tm4c123gh6pm.h"
#include "PLL.h"
#include "stddef.h"
#include "ST7735.h"
#include "plotter.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

volatile unsigned long ADCvalue;
// The digital number ADCvalue is a representation of the voltage on PE4 
// voltage  ADCvalue
// 0.00V     0
// 0.75V    1024
// 1.50V    2048
// 2.25V    3072
// 3.00V    4095
unsigned long volatile delay;


// time logging vars
uint32_t lastTime = 0, thisTime = 0;
uint32_t timeLog[1000];
uint32_t timeLogIndex = 0;
_Bool timeLogFull = 0;


// data logging vars
uint32_t pmf[4096];
uint32_t pmfMax = 0xFFFFFFFF, pmfMin = 0;
uint32_t ADCLog[1000];
uint32_t ADCLogIndex = 0;
_Bool ADCLogFull = 0;

void (*PeriodicTask)(void);   // user function
void (*PeriodicTask0)(void);   // user function

// ***************** TIMER1_Init ****************
// Activate TIMER1 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer1_Init(void(*task)(void), uint32_t period){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  PeriodicTask = task;          // user function
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = period-1;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  if(task != NULL) {
		TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
		NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
	}
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}


void Timer1A_Handler(void){
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
  (*PeriodicTask)();                // execute user task
}

// ***************** Timer0A_Init ****************
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer0A_Init(void(*task)(void), uint32_t period){long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  PeriodicTask0 = task;          // user function
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = period-1;    // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
  EndCritical(sr);
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
  (*PeriodicTask0)();                // execute user task
}

void Timer0A_UpdateTask(void(*task)(void)){
  PeriodicTask0 = task;
}

// ***************** Timer2_Init ****************
// Activate Timer2 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer2_Init(void(*task)(void), unsigned long period){
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  PeriodicTask = task;          // user function
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x20000000; // 8) priority 1
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}

void Timer2A_Handler(void){
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER2A timeout
  (*PeriodicTask)();                // execute user task
}

void timer2(void){
	int x = 0;
	x += x;
}

/*void logValues(void){
	if(logindex < 1000) {
		thisTime = TIMER1_TAR_R; // get current time
		ADCtimes[logindex] = thisTime - lastTime; // log time elapsed;
		ADCvals[logindex] = ADCvalue; // log ADC value
		
		lastTime = thisTime; // iterate
		logindex++;
	}
}*/

// processes collected elapsed time data 
uint32_t processTimes(void){
	uint32_t low = 0xFFFFFFFF, max = 0;
	uint32_t timeJitter;
	int i;
	
	for(i = 0; i < 999; i++) {
		if(timeLog[i] < low) {
			low = timeLog[i];
		}
		else if (timeLog[i] > max) {
			max = timeLog[i];
		}
	}
	
	return max - low;
}

// processes collected ADC data
void processData(uint32_t *ADCLog, uint32_t logSize, uint32_t multiplier){
	int i;
	uint32_t max = 0xFFFFFFFF, min = 0;

	// create probability mass function
	for(i = 0; i < logSize; i++) {
		pmf[ADCLog[i]] += multiplier;
		if(ADCLog[i] < min) {
			min = ADCLog[i];
		}
		else if (ADCLog[i] > max) {
			max = ADCLog[i];
		}
	}
	
	pmfMin = min;
	pmfMax = max;
}

void logTimes(void) {
	// beginning corner case
	if(lastTime == 0) {
		lastTime = TIMER1_TAR_R;
	}
	
	// log times 
	if(timeLogFull == 0) {
		thisTime = TIMER1_TAR_R;
		timeLog[timeLogIndex++] =  lastTime - thisTime;
		lastTime = thisTime;
	}
	if(timeLogIndex == 1000) {
		timeLogFull = 1;
	}
}

void logADCData(void) {
	if(ADCLogFull == 0) {
		ADCLog[ADCLogIndex++] = ADCvalue;
	}
	
	if(ADCLogIndex == 1000) {
		ADCLogFull = 1;
	}
}

// shows (1) time jitter w/o "extra interrupts"
//   and (2) time jitter w/ "extra interrupts"
void screenOne_TimeJitter(void) {
	// init vars
	uint32_t timeJitterOne = 0, timeJitterTwo = 0;
	char *messageOne, *messageTwo;
	
	// display screen one
	ST7735_FillScreen(ST7735_BLACK); 
  ST7735_SetCursor(0,0);
	ST7735_OutString("Logging times with one interrupt...\r");
	
	// init counting/stopwatch timer
	Timer1_Init(NULL, 0xFFFFFFFF);
	
	// init interrupt timer for logging.
	// interrupts at 100Hz.
	Timer0A_Init(&logTimes, 8000);
	
	// wait to log 1000 time deltas
	while(timeLogFull == 0);
	
	// process time deltas to find max - min
	timeJitterOne = processTimes();
	sprintf(messageOne, "Time Jitter: %d \r", timeJitterOne);
	ST7735_OutString(messageOne);
	
	ST7735_OutString("Logging times with higher priority interrupt...\r");
	
	Timer2_Init(&timer2, 7500);
	timeLogFull = 0;
	timeLogIndex = 0;
	
	while(timeLogFull == 0);
	
	timeJitterTwo = processTimes();
	sprintf(messageTwo, "Time Jitter: %d \r", timeJitterTwo);
	ST7735_OutString(messageTwo);
}

// global "inputs": (full) pmf[], and pmfMax, pmfMin
void drawPMF(char *title) {
	int32_t xMin, xMax, yMin, yMax, maxPmfVal = 0;
	int32_t xBuf[4096], yBuf[4096], bufIndex = 0;
	int i;
	xMin = pmfMin;
	xMax = pmfMax;
	yMin = 0;
	
	// construct xBuf, yBuf, yMax
	for(i = pmfMin; i <= pmfMax; i++) {
		xBuf[bufIndex] = i;
		yBuf[bufIndex] = pmf[i];
		
		if(pmf[i] > maxPmfVal) {
			maxPmfVal = pmf[i];
		}
		
		bufIndex++;
	}
	yMax = maxPmfVal;
	
	ST7735_XYplotInit(title, xMin, xMax, yMin, yMax);
	ST7735_XYplot(bufIndex, (int32_t *)xBuf, (int32_t *)yBuf);
}


// shows PMF w/o hardware averaging
void screenTwo_DefaultPMF(void) {
	ADCvalue = ADC0_InSeq3();
	Timer0A_UpdateTask(&logADCData);
	
	// display screen two
	ST7735_FillScreen(ST7735_BLACK); 
  ST7735_SetCursor(0,0);
	ST7735_OutString("Logging ADC values without hardware averaging...\r");
	
	while(ADCLogFull == 0) {
		// following code updates ADCvalue every 100,000 cycles
    GPIO_PORTF_DATA_R |= 0x04;          // profile
    ADCvalue = ADC0_InSeq3();
    GPIO_PORTF_DATA_R &= ~0x04;
    for(delay=0; delay<100000; delay++){};
	};
	
	processData(ADCLog, 1000, 1);
	drawPMF("PMF w/o hardware averaging");
}


// shows a PMF of the hardware-averaged ADC values,
//   where each point is an average of "samples_to_avg"
//   number of ADC values.
// NOTE: 16 <= samples_to_avg <= 64.
uint32_t avgData[63];
uint32_t runningAvg;
void screenTwo_HardAvgPMF(uint32_t samples_to_avg) {
	uint32_t avgIndex = 0, numLeftOver, dataLength, i;
	char *messageOne;
	runningAvg = 0;
	for(i = 0; i < 1000; i++) {
		runningAvg += ADCLog[i];
		if(i % samples_to_avg == samples_to_avg - 1) {
			avgData[avgIndex] = runningAvg / samples_to_avg;
			avgIndex++;
			runningAvg = 0;
		}
	}
	if(runningAvg != 0) {
		numLeftOver = 1000 % samples_to_avg;
		avgData[avgIndex] = runningAvg / numLeftOver;
	}
	
	dataLength = ((1000 % samples_to_avg) == 0) ? (1000 / samples_to_avg) : (1000 / samples_to_avg + 1);
	processData(avgData, dataLength, samples_to_avg);
	
	sprintf(messageOne, "PMF w/ %d-bit hardware avging", samples_to_avg);
	drawPMF(messageOne);
}




// shows a demonstration of our line drawing method
void screenThree_LineDrawing(void) {
	// display screen two
	ST7735_FillScreen(ST7735_BLACK); 
  ST7735_SetCursor(0,0);
	
	ST7735_Line(10, 10, 80, 80, ST7735_RED);
	ST7735_Line(80, 80, 10, 10, ST7735_RED);
	ST7735_Line(40, 40, 40, 100, ST7735_RED); // vertical line test
}

// waits until a rising edge from PF4
_Bool lastPF4Val = 0;
void waitForSwitchToggle(void){
	while(((GPIO_PORTF_DATA_R & 0x08) == lastTime) ||	((GPIO_PORTF_DATA_R & 0x08) == 0)) {
		lastPF4Val = (GPIO_PORTF_DATA_R & 0x08) > 0 ? 1 : 0;
	}
}

void initForADCTestMain(void) {
	PLL_Init();                           // 80 MHz
  ADC0_InitSWTriggerSeq3_Ch1();         // ADC initialization PE2/AIN1
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // activate port F
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTF_DIR_R |= 0x04;             // make PF2 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x04;          // disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;             // enable digital I/O on PF2
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
}

void initST7735() {
	ST7735_InitR(INITR_REDTAB);
}

_Bool processed = 0;
int main(void){
  initForADCTestMain();
	initST7735();
	
	// Lab demo screens
	screenOne_TimeJitter();
	waitForSwitchToggle();
	
	screenTwo_DefaultPMF();
	waitForSwitchToggle();
	
	screenTwo_HardAvgPMF(16);
	waitForSwitchToggle();
	
	screenTwo_HardAvgPMF(64);
	waitForSwitchToggle();
	
	screenThree_LineDrawing();
	waitForSwitchToggle();
	
  /*while(1){
		if(logindex >= 1000 && processed == 0) {
			processData();
			processTime();
			processed = 1;
		}
		
		// following code updates ADCvalue every 100,000 cycles
    GPIO_PORTF_DATA_R |= 0x04;          // profile
    ADCvalue = ADC0_InSeq3();
    GPIO_PORTF_DATA_R &= ~0x04;
    for(delay=0; delay<100000; delay++){};
  }*/
}

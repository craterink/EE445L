#include <stdint.h>
#include <math.h>
#include "fixed.h"

void init(void){
	
}


// version 1: C floating point
// run with compiler options selected for floating-point hardware
volatile float T1;    // temperature in C
volatile uint32_t N1; // 12-bit ADC value
void Test1(void){
  for(N1=0; N1<4096; N1++){
    T1 = 10.0+ 0.009768*N1; 	
  }
}


// version 2: C fixed-point
volatile uint32_t T2;    // temperature in 0.01 C
volatile uint32_t N2;    // 12-bit ADC value
void Test2(void){
  for(N2=0; N2<4096; N2++){
    T2 = 1000 + ((125*N2+64)>>7); 	
  }
}

extern void Test3(void);
extern void Test4(void);

void test() {
	Test1();
	Test2();
	Test3();
	Test4();
}



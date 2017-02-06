#include <stdint.h>
#include <math.h>
#include "fixed.h"
#include "ST7735.h"

//DecOut
#define OUT_OF_RANGE " *.***"
#define DEC_RANGE 9999

//BinOut
#define RANGE_ERROR "***.**"
#define BIN_RANGE 256000

char sign(int32_t n) {
	if (n < 0) {
		return '-';
	} else {
		return ' ';
	}
}

_Bool outOfDecRange(int32_t n) {
	return n > DEC_RANGE || n < -DEC_RANGE;
}

void ST7735_sDecOut3(int32_t n) {
	uint8_t index;
	if(outOfDecRange(n)) {
		ST7735_OutString(OUT_OF_RANGE);
	}
	else {
		// init out string
		char out[6];
		out[0] = sign(n);
		
		// take absolute value of n
		if(n < 0) {
			n = ~n + 1;
		}
		
		// calculate decimal value
		index = 2;
		while(index < 6) {
			uint32_t pwr = pow(10,(5 - index));
			uint32_t val = n / pwr;
			out[index] = '0' + val;
			
			index++;
			n %= pwr;
		}
		
		// shift and insert decimal point
		out[1] = out[2];
		out[2] = '.';
		
		ST7735_OutString(out);
	}
}

_Bool outOfBinRange(uint32_t n) {
	return n >= BIN_RANGE;
}

void ST7735_uBinOut8(uint32_t n) {
	uint32_t frac_part;
	if(outOfBinRange(n)) {
		ST7735_OutString(RANGE_ERROR);
	}
	else {
		// init out string
		char out[6];
		_Bool zero = 0;
		
		uint32_t whole_part = n >> 8;
		if(whole_part / 100 == 0) {
			out[0] = ' ';
			zero = 1;
		}
		else out[0] = '0' + whole_part / 100;
		whole_part %= 100;
		if(zero && whole_part / 10 == 0) out[1] = ' ';
		else out[1] = '0' + whole_part / 10;
		whole_part %= 10;
		out[2] = '0' + whole_part;
		
		out[3] = '.';

		frac_part = ((n & 0x000000FF) * 100) / 256;
		out[4] = '0' + (frac_part / 10);
		frac_part %= 10;
		out[5] = '0' + frac_part;
		
		ST7735_OutString(out);
	}
}

int32_t minX_global;
int32_t maxX_global;
int32_t minY_global;
int32_t maxY_global;

const uint32_t width = 128;
const uint32_t height = 128;

void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY) {
	
	minX_global = minX;
	maxX_global = maxX;
	minY_global = minY;
	maxY_global = maxY;
	
	// clear plot
	ST7735_FillScreen(ST7735_BLACK);
	
	// draw title
	ST7735_SetCursor(minX / 1000, maxY / 1000);
	ST7735_OutString(title);
}

void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]) {
	uint32_t index = 0;
	uint32_t xLCD;
	uint32_t yLCD;
	while(index < num) {
		// check for out-of-bounds
		if(bufX[index] < minX_global || bufX[index] > maxX_global
			 || bufY[index] < minY_global || bufY[index] > maxY_global) {
			continue;
		}
			 
		// calculate location of point transformed to LCD pixel coordinates
		xLCD = width*(bufX[index] - minX_global)/(maxX_global - minX_global);
		yLCD = height - height*(bufY[index] - minY_global)/(maxY_global - minY_global);
		
		// draw point and iterate
		ST7735_FillRect(xLCD, yLCD + 10, 1, 1, ST7735_RED);
		index++;
	}
}


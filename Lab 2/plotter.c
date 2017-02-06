#include <stdint.h>
#include <math.h>
#include "ST7735.h"

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
	uint32_t index = 0, xLCD, yLCD;
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

//************* ST7735_Line********************************************
//  Draws one line on the ST7735 color LCD
//  Inputs: (x1,y1) is the start point
//          (x2,y2) is the end point
// x1,x2 are horizontal positions, columns from the left edge
//               must be less than 128
//               0 is on the left, 126 is near the right
// y1,y2 are vertical positions, rows from the top edge
//               must be less than 160
//               159 is near the wires, 0 is the side opposite the wires
//        color 16-bit color, which can be produced by ST7735_Color565() 
// Output: none
void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color){
	uint32_t xMin, xMax, yMin, yMax, i, j, dotProd;
	uint32_t sfVecX, sfVecY;
	uint32_t seVecX, seVecY;
	double cosTheta2, threshold = 0.9, sfMag2, seMag2;
	
	xMin = x1 < x2 ? x1 : x2;
	xMax = x1 < x2 ? x2 : x1;
	yMin = y1 < y2 ? y1 : y2;
	yMax = y1 < y2 ? y2 : y1;
	
	// start-to-finish vector
	sfVecX = x2 - x1;
	sfVecY = y2 - y1;
	sfMag2 = sfVecX * sfVecX + sfVecY * sfVecY; 
	
	for(i = xMin; i <= xMax; i++) {
		for(j = yMin; j <= yMax; j++) {
			
			// compute vector
			seVecX = i - x1;
			seVecY = j - y1;
			
			// compute sfVec [dot] seVec
			dotProd = seVecY * sfVecY + seVecX * sfVecX;
			
			// compute cosTheta2
			seMag2 = seVecX * seVecX + seVecY * seVecY; 
			cosTheta2 = dotProd / (sfMag2 * seMag2);
			
			if(cosTheta2 > threshold) {
				ST7735_FillRect(i, j, 1, 1, color);
			}
		}
	}
	
	
}





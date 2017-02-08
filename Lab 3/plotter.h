#include <stdint.h>
#include <math.h>
#include "ST7735.h"

void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY);

void ST7735_XYplot(uint32_t num, int16_t bufX[], int16_t bufY[], _Bool stem);

void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

void setLineThreshold(double newThreshold);


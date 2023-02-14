#include "touch.h"
#include "touch_io.h"

#include "stm32f1xx.h"

#include <math.h>
#include <stdlib.h>


#define READ_TIMES  15
#define SKIP_VALUES 5

#define X_SCALE 7.7F
#define Y_SCALE 5.7F

#define X_MIN 90.0F
#define Y_MIN 90.0F

#define WIDTH  240
#define HEIGHT 320

static uint8_t touch_rotation = 0;

void _bubbleSort(uint16_t a[], uint8_t array_size)
{
    for (uint8_t i = 0; i < (array_size - 1); ++i)
    {
        for (uint8_t j = 0; j < array_size - 1 - i; ++j)
        {
            if (a[j] > a[j+1])
            {
            	uint16_t temp = a[j+1];
                a[j+1] = a[j];
                a[j] = temp;
            }
        }
    }
}

float _constrain(float value, float value_min, float value_max)
{
      if(value >= value_max)
      {
           return value_max;
      }
      else if(value <= value_min)
      {
           return value_min;
      }
      else
      {
           return value;
      }
}

void Touch_Init(void)
{
	TOUCH_IO_Init();
}

void Touch_SetRotation(uint8_t rotation)
{
	touch_rotation = rotation;
}

bool Touch_GetPoint(uint16_t *x, uint16_t *y)
{
	uint16_t xValues[READ_TIMES];
	uint16_t yValues[READ_TIMES];

	uint8_t reads = 0;

	// read several touch points
	while(reads < READ_TIMES && !TOUCH_PEN_GetState())
	{
		xValues[reads] = TOUCH_IO_Read(CMD_RDX);
		yValues[reads] = TOUCH_IO_Read(CMD_RDY);
		++reads;
	}

	// all reads correct?
	if (reads != READ_TIMES) return false;

	// sort touch points
	_bubbleSort(xValues, READ_TIMES);
	_bubbleSort(yValues, READ_TIMES);

	// calculate mean of significant points
	float avgX = 0;
	float avgY = 0;

	for(reads = SKIP_VALUES; reads < READ_TIMES - SKIP_VALUES; reads++)
	{
		avgX += xValues[reads];
		avgY += yValues[reads];
	}

	avgX /= (READ_TIMES - 2*SKIP_VALUES);
	avgY /= (READ_TIMES - 2*SKIP_VALUES);

	// constrain touch point to LCD dimensions and rotation
	uint16_t rX = _constrain((avgX - X_MIN) / X_SCALE, 0, WIDTH - 1);
	uint16_t rY = _constrain((avgY - Y_MIN) / Y_SCALE, 0, HEIGHT); // HEIGHT if special area is touched

	switch (touch_rotation % 4)
	{
		case 0:
			*x = rX;
			*y = rY;
			break;
		case 1:
			*x = (rY == HEIGHT) ? HEIGHT : HEIGHT - 1 - rY;
			*y = rX;
			break;
		case 2:
			*x = WIDTH - 1 - rX;
			*y = (rY == HEIGHT) ? HEIGHT : HEIGHT - 1 - rY;
			break;
		case 3:
			*x = rY;
			*y = WIDTH - 1 - rX;
			break;
		default:
			break;
	}

	return true;
}

bool Touch_isTouched()
{
	return !TOUCH_PEN_GetState();
}


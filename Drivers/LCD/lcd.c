/**
 ******************************************************************************
 * @file    lcd.c
 * @author  xdzmkus
 * @version 1.0
 * @date    2023
 * @brief   This file includes the driver for Liquid Crystal Display (LCD) module.
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================  
  [..] 
   (#) This driver is used to drive indirectly an LCD TFT.

   (#) This driver supports the LCD_ILI9325 LCD mounted on board

   (#) The ILI9325 components driver MUST be included with this driver.

   (#) Initialization steps:
       (++) Initialize the LCD using the LCD_Init() function.

   (#) Display on LCD
       (++) Clear the hole LCD using the LCD_Clear() function or only one specified
            string line using the LCD_ClearStringLine() function.
       (++) Display a character on the specified line and column using the LCD_DisplayChar()
            function or a complete string line using the LCD_DisplayStringAtLine() function.
       (++) Display a string line on the specified position (x,y in pixel) and align mode
            using the LCD_DisplayStringAtLine() function.
       (++) Draw and fill a basic shapes (dot, line, rectangle, circle, ellipse, .. bitmap, raw picture) 
            on LCD using a set of functions.    
  @endverbatim
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/

#include "lcd.h"
#include "lcd_io.h"
#include "lcd_drv.h"
#include "ili9325/ili9325.h"

#include <stddef.h>

#define POLY_X(Z)              ((int32_t)((Points + (Z))->X))
#define POLY_Y(Z)              ((int32_t)((Points + (Z))->Y))

#define MAX_HEIGHT_FONT         17
#define MAX_WIDTH_FONT          24
#define OFFSET_BITMAP           54

#define ABS(X)  ((X) > 0 ? (X) : -(X)) 

#define LINE(x) ((x) * (((sFONT *)LCD_GetFont())->Height))

LCD_DrawPropTypeDef DrawProp;

LCD_DrvTypeDef  *lcd_drv;

/* Max size of bitmap will based on a font24 (17x24) */
uint8_t bitmap[MAX_HEIGHT_FONT*MAX_WIDTH_FONT*2+OFFSET_BITMAP] = {0};

uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b)
{
    return (((((uint16_t)r) & 0x1F) << 11) | ((((uint16_t)g) & 0x3f) << 5) | (((uint16_t)b) & 0x1F));
}

/**
 * @brief  Initializes the LCD.
 * @retval LCD state
 */
uint8_t LCD_Init(void)
{ 
	/* Initialize low level bus layer */
	LCD_IO_Init();

	uint8_t ret = LCD_ERROR;

	if(ili9325_drv.ReadID() == ILI9325_ID)
	{
		lcd_drv = &ili9325_drv;

		/* LCD Init */
		lcd_drv->Init();

		ret = LCD_OK;
	}

	/* Default value for draw propriety */
	LCD_SetBackColor(LCD_COLOR_BLACK);
	LCD_SetFont(&LCD_DEFAULT_FONT);
	LCD_SetTextColor(LCD_COLOR_WHITE);

	return ret;
}

/**
 * @brief  Clears the hole LCD.
 * @param  Color: Color of the background
 */
void LCD_Clear(uint16_t Color)
{
	lcd_drv->Clear(Color);
}

/**
 * @brief  Clears the selected line.
 * @param  Line: Line to be cleared
 *          This parameter can be one of the following values:
 *            @arg  0..9: if the Current fonts is Font16x24
 *            @arg  0..19: if the Current fonts is Font12x12 or Font8x12
 *            @arg  0..29: if the Current fonts is Font8x8
 */

/**
 * @brief  Sets rotation.
 * @param  rotation: May be 0, 1, 2 or 3
 */
void LCD_SetRotation(uint8_t rotation)
{
	lcd_drv->SetRotation(rotation);
}

/**
 * @brief  Sets display window.
 * @param  Xpos: LCD X position
 * @param  Ypos: LCD Y position
 * @param  Width: LCD window width
 * @param  Height: LCD window height
 */
void LCD_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
	lcd_drv->SetDisplayWindow(Xpos, Ypos, Width, Height);
}

/**
 * @brief  Enables the display.
 */
void LCD_DisplayOn(void)
{
	lcd_drv->DisplayOn();
}

/**
 * @brief  Disables the display.
 */
void LCD_DisplayOff(void)
{
	lcd_drv->DisplayOff();
}

/**
 * @brief  Gets the LCD X size.
 * @retval Used LCD X size
 */
uint16_t LCD_GetWidth(void)
{
	return lcd_drv->GetLcdPixelWidth();
}

/**
 * @brief  Gets the LCD Y size.
 * @retval Used LCD Y size
 */
uint16_t LCD_GetHeight(void)
{
	return lcd_drv->GetLcdPixelHeight();
}

/**
 * @brief  Gets the LCD text color.
 * @retval Used text color.
 */
uint16_t LCD_GetTextColor(void)
{
	return DrawProp.TextColor;
}

/**
 * @brief  Gets the LCD background color.
 * @retval Used background color
 */
uint16_t LCD_GetBackColor(void)
{
	return DrawProp.BackColor;
}

/**
 * @brief  Sets the LCD text color.
 * @param  Color: Text color code RGB(5-6-5)
 */
void LCD_SetTextColor(uint16_t Color)
{
	DrawProp.TextColor = Color;
}

/**
 * @brief  Sets the LCD background color.
 * @param  Color: Background color code RGB(5-6-5)
 */
void LCD_SetBackColor(uint16_t Color)
{
	DrawProp.BackColor = Color;
}

/**
 * @brief  Sets the LCD text font.
 * @param  pFonts: Font to be used
 */
void LCD_SetFont(sFONT *pFonts)
{
	DrawProp.pFont = pFonts;
}

/**
 * @brief  Gets the LCD text font.
 * @retval Used font
 */
sFONT *LCD_GetFont(void)
{
	return DrawProp.pFont;
}

/**
 * @brief  Displays one character.
 * @param  Xpos: Line where to display the character shape
 * @param  Ypos: Start column address
 * @param  Ascii: Character ascii code
 *           This parameter must be a number between Min_Data = 0x20 and Max_Data = 0x7E
 */
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
	const uint8_t *pChar = &DrawProp.pFont->table[(Ascii-' ') * DrawProp.pFont->Height * ((DrawProp.pFont->Width + 7) / 8)];

	uint32_t counterh = 0, counterw = 0, index = 0;
	uint16_t height = 0, width = 0;
	uint8_t offset = 0;
	uint8_t *pchar = NULL;
	uint32_t line = 0;

	height = DrawProp.pFont->Height;
	width  = DrawProp.pFont->Width;

	/* Fill bitmap header*/
	*(uint16_t *) (bitmap + 2) = (uint16_t)(height*width*2+OFFSET_BITMAP);
	*(uint16_t *) (bitmap + 4) = (uint16_t)((height*width*2+OFFSET_BITMAP)>>16);
	*(uint16_t *) (bitmap + 10) = OFFSET_BITMAP;
	*(uint16_t *) (bitmap + 18) = (uint16_t)(width);
	*(uint16_t *) (bitmap + 20) = (uint16_t)((width)>>16);
	*(uint16_t *) (bitmap + 22) = (uint16_t)(height);
	*(uint16_t *) (bitmap + 24) = (uint16_t)((height)>>16);

	offset =  8 *((width + 7)/8) -  width ;

	for(counterh = 0; counterh < height; counterh++)
	{
		pchar = ((uint8_t *)pChar + (width + 7)/8 * counterh);

		if(((width + 7)/8) == 3)
		{
			line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];
		}

		if(((width + 7)/8) == 2)
		{
			line =  (pchar[0]<< 8) | pchar[1];
		}

		if(((width + 7)/8) == 1)
		{
			line =  pchar[0];
		}

		for (counterw = 0; counterw < width; counterw++)
		{
			/* Image in the bitmap is written from the bottom to the top */
			/* Need to invert image in the bitmap */
			index = ((counterh*width)+(counterw))*2+OFFSET_BITMAP;
			if(line & (1 << (width- counterw + offset- 1)))
			{
				bitmap[index] = (uint8_t)DrawProp.TextColor;
				bitmap[index+1] = (uint8_t)(DrawProp.TextColor >> 8);
			}
			else
			{
				bitmap[index] = (uint8_t)DrawProp.BackColor;
				bitmap[index+1] = (uint8_t)(DrawProp.BackColor >> 8);
			}
		}
	}

	LCD_DrawBitmap(Xpos, Ypos, bitmap);
}

/**
 * @brief  Displays characters on the LCD.
 * @param  Xpos: X position (in pixel)
 * @param  Ypos: Y position (in pixel)
 * @param  pText: Pointer to string to display on LCD
 * @param  Mode: Display mode
 *          This parameter can be one of the following values:
 *            @arg  CENTER_MODE
 *            @arg  RIGHT_MODE
 *            @arg  LEFT_MODE
 */
void LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *pText, Line_ModeTypdef Mode)
{
	uint16_t refcolumn = 1, counter = 0;
	uint32_t size = 0, xsize = 0;
	uint8_t  *ptr = pText;

	/* Get the text size */
	while (*ptr++) size ++ ;

	/* Characters number per line */
	xsize = (LCD_GetWidth()/DrawProp.pFont->Width);

	switch (Mode)
	{
	case CENTER_MODE:
	{
		refcolumn = Xpos + ((xsize - size)* DrawProp.pFont->Width) / 2;
		break;
	}
	case LEFT_MODE:
	{
		refcolumn = Xpos;
		break;
	}
	case RIGHT_MODE:
	{
		refcolumn =  - Xpos + ((xsize - size)*DrawProp.pFont->Width);
		break;
	}
	default:
	{
		refcolumn = Xpos;
		break;
	}
	}

	/* Send the string character by character on lCD */
	while ((*pText != 0) & (((LCD_GetWidth() - (counter*DrawProp.pFont->Width)) & 0xFFFF) >= DrawProp.pFont->Width))
	{
		/* Display one character on LCD */
		LCD_DisplayChar(refcolumn, Ypos, *pText);
		/* Decrement the column position by 16 */
		refcolumn += DrawProp.pFont->Width;
		/* Point on the next character */
		pText++;
		counter++;
	}
}

/**
 * @brief  Displays a character on the LCD.
 * @param  Line: Line where to display the character shape
 *          This parameter can be one of the following values:
 *            @arg  0..9: if the Current fonts is Font16x24
 *            @arg  0..19: if the Current fonts is Font12x12 or Font8x12
 *            @arg  0..29: if the Current fonts is Font8x8
 * @param  pText: Pointer to string to display on LCD
 */
void LCD_DisplayStringAtLine(uint16_t Line, uint8_t *pText)
{
	LCD_DisplayStringAt(0, LINE(Line),pText, LEFT_MODE);
}


void LCD_ClearStringLine(uint16_t Line)
{
	uint16_t colorbackup = DrawProp.TextColor;
	DrawProp.TextColor = DrawProp.BackColor;;

	/* Draw a rectangle with background color */
	LCD_FillRect(0, (Line * DrawProp.pFont->Height), LCD_GetWidth(), DrawProp.pFont->Height);

	DrawProp.TextColor = colorbackup;
}

/**
 * @brief  Reads an LCD pixel.
 * @param  Xpos: X position
 * @param  Ypos: Y position
 * @retval RGB pixel color
 */
uint16_t LCD_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
	return lcd_drv->ReadPixel(Xpos, Ypos);
}

/**
 * @brief  Draws a pixel on LCD.
 * @param  Xpos: X position
 * @param  Ypos: Y position
 * @param  RGBCode: Pixel color in RGB mode (5-6-5)
 */
void LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode)
{
	lcd_drv->WritePixel(Xpos, Ypos, RGBCode);
}

/**
 * @brief  Draws an uni-line (between two points).
 * @param  x1: Point 1 X position
 * @param  y1: Point 1 Y position
 * @param  x2: Point 2 X position
 * @param  y2: Point 2 Y position
 */
void LCD_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
			yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;

	deltax = ABS(x2 - x1);        /* The difference between the x's */
	deltay = ABS(y2 - y1);        /* The difference between the y's */
	x = x1;                       /* Start x off at the first pixel */
	y = y1;                       /* Start y off at the first pixel */

	if (x2 >= x1)                 /* The x-values are increasing */
	{
		xinc1 = 1;
		xinc2 = 1;
	}
	else                          /* The x-values are decreasing */
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1)                 /* The y-values are increasing */
	{
		yinc1 = 1;
		yinc2 = 1;
	}
	else                          /* The y-values are decreasing */
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay)         /* There is at least one x-value for every y-value */
	{
		xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
		yinc2 = 0;                  /* Don't change the y for every iteration */
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;         /* There are more x-values than y-values */
	}
	else                          /* There is at least one y-value for every x-value */
	{
		xinc2 = 0;                  /* Don't change the x for every iteration */
		yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;         /* There are more y-values than x-values */
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
		if (x >= 0 && y >= 0)
		{
			LCD_DrawPixel(x, y, DrawProp.TextColor);  /* Draw the current pixel */
		}
		num += numadd;                            /* Increase the numerator by the top of the fraction */
		if (num >= den)                           /* Check if numerator >= denominator */
		{
			num -= den;                             /* Calculate the new numerator value */
			x += xinc1;                             /* Change the x as appropriate */
			y += yinc1;                             /* Change the y as appropriate */
		}
		x += xinc2;                               /* Change the x as appropriate */
		y += yinc2;                               /* Change the y as appropriate */
	}
}

/**
 * @brief  Draws an horizontal line.
 * @param  Xpos: X position
 * @param  Ypos: Y position
 * @param  Length: Line length
 */
void LCD_DrawHLine(int16_t Xpos, int16_t Ypos, uint16_t Length)
{
	lcd_drv->DrawHLine(Xpos, Ypos, Length, DrawProp.TextColor);
}

/**
 * @brief  Draws a vertical line.
 * @param  Xpos: X position
 * @param  Ypos: Y position
 * @param  Length: Line length
 */
void LCD_DrawVLine(int16_t Xpos, int16_t Ypos, uint16_t Length)
{
	lcd_drv->DrawVLine(Xpos, Ypos, Length, DrawProp.TextColor);
}

/**
 * @brief  Draws a rectangle.
 * @param  Xpos: X position
 * @param  Ypos: Y position
 * @param  Width: Rectangle width
 * @param  Height: Rectangle height
 */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
	/* Draw horizontal lines */
	LCD_DrawHLine(Xpos, Ypos, Width);
	LCD_DrawHLine(Xpos, (Ypos+ Height), Width);

	/* Draw vertical lines */
	LCD_DrawVLine(Xpos, Ypos, Height);
	LCD_DrawVLine((Xpos + Width - 1), Ypos, Height);
}

/**
 * @brief  Draws a circle.
 * @param  Xpos: X position
 * @param  Ypos: Y position
 * @param  Radius: Circle radius
 */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
	int32_t  D;       /* Decision Variable */
	uint32_t  CurX;   /* Current X Value */
	uint32_t  CurY;   /* Current Y Value */

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY)
	{
		LCD_DrawPixel((Xpos + CurX), (Ypos - CurY), DrawProp.TextColor);

		LCD_DrawPixel((Xpos - CurX), (Ypos - CurY), DrawProp.TextColor);

		LCD_DrawPixel((Xpos + CurY), (Ypos - CurX), DrawProp.TextColor);

		LCD_DrawPixel((Xpos - CurY), (Ypos - CurX), DrawProp.TextColor);

		LCD_DrawPixel((Xpos + CurX), (Ypos + CurY), DrawProp.TextColor);

		LCD_DrawPixel((Xpos - CurX), (Ypos + CurY), DrawProp.TextColor);

		LCD_DrawPixel((Xpos + CurY), (Ypos + CurX), DrawProp.TextColor);

		LCD_DrawPixel((Xpos - CurY), (Ypos + CurX), DrawProp.TextColor);

		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

/**
 * @brief  Draws an poly-line (between many points).
 * @param  Points: Pointer to the points array
 * @param  PointCount: Number of points
 */
void LCD_DrawPolygon(pPoint Points, uint16_t PointCount)
{
	int16_t X = 0, Y = 0;

	if(PointCount < 2)
	{
		return;
	}

	LCD_DrawLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y);

	while(--PointCount)
	{
		X = Points->X;
		Y = Points->Y;
		Points++;
		LCD_DrawLine(X, Y, Points->X, Points->Y);
	}

}

/**
 * @brief  Draws an ellipse on LCD.
 * @param  Xpos: X position
 * @param  Ypos: Y position
 * @param  XRadius: Ellipse X radius
 * @param  YRadius: Ellipse Y radius
 */
void LCD_DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius)
{
	int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
	float K = 0, rad1 = 0, rad2 = 0;

	rad1 = XRadius;
	rad2 = YRadius;

	K = (float)(rad2/rad1);

	do {
		LCD_DrawPixel((Xpos-(uint16_t)(x/K)), (Ypos+y), DrawProp.TextColor);
		LCD_DrawPixel((Xpos+(uint16_t)(x/K)), (Ypos+y), DrawProp.TextColor);
		LCD_DrawPixel((Xpos+(uint16_t)(x/K)), (Ypos-y), DrawProp.TextColor);
		LCD_DrawPixel((Xpos-(uint16_t)(x/K)), (Ypos-y), DrawProp.TextColor);

		e2 = err;
		if (e2 <= x) {
			err += ++x*2+1;
			if (-y == x && e2 <= y) e2 = 0;
		}
		if (e2 > y) err += ++y*2+1;
	}
	while (y <= 0);
}

/**
 * @brief  Draws a bitmap picture (16 bpp).
 * @param  Xpos: Bmp X position in the LCD
 * @param  Ypos: Bmp Y position in the LCD
 * @param  pbmp: Pointer to Bmp picture address.
 */
void LCD_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp)
{
	uint32_t height = 0;
	uint32_t width  = 0;

	/* Read bitmap width */
	width = *(uint16_t *) (pbmp + 18);
	width |= (*(uint16_t *) (pbmp + 20)) << 16;

	/* Read bitmap height */
	height = *(uint16_t *) (pbmp + 22);
	height |= (*(uint16_t *) (pbmp + 24)) << 16;

	LCD_SetDisplayWindow(Xpos, Ypos, width, height);

	lcd_drv->DrawBitmap(Xpos, Ypos, pbmp);

	LCD_SetDisplayWindow(0, 0, LCD_GetWidth(), LCD_GetHeight());
}

/**
 * @brief  Draws RGB Image (16 bpp).
 * @param  Xpos:  X position in the LCD
 * @param  Ypos:  Y position in the LCD
 * @param  Xsize: X size in the LCD
 * @param  Ysize: Y size in the LCD
 * @param  pdata: Pointer to the RGB Image address.
 */
void LCD_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t *pdata)
{
	LCD_SetDisplayWindow(Xpos, Ypos, Width, Height);

	lcd_drv->DrawRGBImage(Xpos, Ypos, pdata, Width * Height);

	LCD_SetDisplayWindow(0, 0, LCD_GetWidth(), LCD_GetHeight());
}

/**
 * @brief  Draws a full rectangle.
 * @param  Xpos: X position
 * @param  Ypos: Y position
 * @param  Width: Rectangle width
 * @param  Height: Rectangle height
 */
void LCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
	do
	{
		LCD_DrawHLine(Xpos, Ypos++, Width);
	}
	while(Height--);
}

/**
 * @brief  Draws a full circle.
 * @param  Xpos: X position
 * @param  Ypos: Y position
 * @param  Radius: Circle radius
 */
void LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
	int32_t  D;        /* Decision Variable */
	uint32_t  CurX;    /* Current X Value */
	uint32_t  CurY;    /* Current Y Value */

	D = 3 - (Radius << 1);

	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY)
	{
		if(CurY > 0)
		{
			LCD_DrawHLine(Xpos - CurY, Ypos + CurX, 2*CurY);
			LCD_DrawHLine(Xpos - CurY, Ypos - CurX, 2*CurY);
		}

		if(CurX > 0)
		{
			LCD_DrawHLine(Xpos - CurX, Ypos - CurY, 2*CurX);
			LCD_DrawHLine(Xpos - CurX, Ypos + CurY, 2*CurX);
		}
		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}

	LCD_DrawCircle(Xpos, Ypos, Radius);
}

/**
 * @brief  Draws a full ellipse.
 * @param  Xpos: X position
 * @param  Ypos: Y position
 * @param  XRadius: Ellipse X radius
 * @param  YRadius: Ellipse Y radius
 */
void LCD_FillEllipse(int Xpos, int Ypos, int XRadius, int YRadius)
{
	int x = 0, y = -YRadius, err = 2-2*XRadius, e2;
	float K = 0, rad1 = 0, rad2 = 0;

	rad1 = XRadius;
	rad2 = YRadius;

	K = (float)(rad2/rad1);

	do
	{
		LCD_DrawHLine((Xpos-(uint16_t)(x/K)), (Ypos+y), (2*(uint16_t)(x/K) + 1));
		LCD_DrawHLine((Xpos-(uint16_t)(x/K)), (Ypos-y), (2*(uint16_t)(x/K) + 1));

		e2 = err;
		if (e2 <= x)
		{
			err += ++x*2+1;
			if (-y == x && e2 <= y) e2 = 0;
		}
		if (e2 > y) err += ++y*2+1;
	}
	while (y <= 0);
}


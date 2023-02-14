/**
  ******************************************************************************
  * @file    lcd.h
  * @author  xdzmkus
  * @version 1.0
  * @date    2023
  * @brief   This file contains the common defines and functions prototypes for
  *          the lcd driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _LCD_H_
#define _LCD_H_

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "Fonts/fonts.h"

 typedef struct
{ 
  uint16_t TextColor;
  uint16_t BackColor;
  sFONT    *pFont; 

}LCD_DrawPropTypeDef;

/** 
  * @brief  LCD status structure definition  
  */     
#define LCD_OK         0x00
#define LCD_ERROR      0x01
#define LCD_TIMEOUT    0x02
    
typedef struct 
{
  int16_t X;
  int16_t Y;

}Point, * pPoint; 

/** 
  * @brief  Line mode structures definition
  */ 
typedef enum
{
  CENTER_MODE             = 0x01,    /*!< Center mode */
  RIGHT_MODE              = 0x02,    /*!< Right mode  */
  LEFT_MODE               = 0x03     /*!< Left mode   */

}Line_ModeTypdef;

/** 
  * @brief  LCD color  
  */ 
#define LCD_COLOR_BLUE          0x001F
#define LCD_COLOR_GREEN         0x07E0
#define LCD_COLOR_RED           0xF800
#define LCD_COLOR_CYAN          0x07FF
#define LCD_COLOR_MAGENTA       0xF81F
#define LCD_COLOR_YELLOW        0xFFE0
#define LCD_COLOR_LIGHTBLUE     0x841F
#define LCD_COLOR_LIGHTGREEN    0x87F0
#define LCD_COLOR_LIGHTRED      0xFC10
#define LCD_COLOR_LIGHTCYAN     0x87FF
#define LCD_COLOR_LIGHTMAGENTA  0xFC1F
#define LCD_COLOR_LIGHTYELLOW   0xFFF0
#define LCD_COLOR_DARKBLUE      0x0010
#define LCD_COLOR_DARKGREEN     0x0400
#define LCD_COLOR_DARKRED       0x8000
#define LCD_COLOR_DARKCYAN      0x0410
#define LCD_COLOR_DARKMAGENTA   0x8010
#define LCD_COLOR_DARKYELLOW    0x8400
#define LCD_COLOR_WHITE         0xFFFF
#define LCD_COLOR_LIGHTGRAY     0xD69A
#define LCD_COLOR_GRAY          0x8410
#define LCD_COLOR_DARKGRAY      0x4208
#define LCD_COLOR_BLACK         0x0000
#define LCD_COLOR_BROWN         0xA145
#define LCD_COLOR_ORANGE        0xFD20

/** 
  * @brief LCD default font 
  */ 
#define LCD_DEFAULT_FONT         Font8

uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b);

uint8_t  LCD_Init(void);

void     LCD_Clear(uint16_t Color);

void     LCD_SetRotation(uint8_t rotation);

void     LCD_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);

void     LCD_DisplayOn(void);
void     LCD_DisplayOff(void);

uint16_t LCD_GetWidth(void);
uint16_t LCD_GetHeight(void);
 
uint16_t LCD_GetTextColor(void);
void     LCD_SetTextColor(uint16_t Color);
uint16_t LCD_GetBackColor(void);
void     LCD_SetBackColor(uint16_t Color);
sFONT*   LCD_GetFont(void);
void     LCD_SetFont(sFONT *fonts);

void     LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii);
void     LCD_DisplayStringAtLine(uint16_t Line, uint8_t *ptr);
void     LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, Line_ModeTypdef Mode);
void     LCD_ClearStringLine(uint16_t Line);

uint16_t LCD_ReadPixel(uint16_t Xpos, uint16_t Ypos);
void     LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode);
void     LCD_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void     LCD_DrawHLine(int16_t Xpos, int16_t Ypos, uint16_t Length);
void     LCD_DrawVLine(int16_t Xpos, int16_t Ypos, uint16_t Length);
void     LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void     LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void     LCD_DrawPolygon(pPoint Points, uint16_t PointCount);
void     LCD_DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius);
void     LCD_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp);
void     LCD_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t *pbmp);

void     LCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void     LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void     LCD_FillEllipse(int Xpos, int Ypos, int XRadius, int YRadius);


#ifdef __cplusplus
}
#endif

#endif


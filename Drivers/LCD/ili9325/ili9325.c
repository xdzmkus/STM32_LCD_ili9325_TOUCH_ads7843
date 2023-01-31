/**
 ******************************************************************************
 * @file    ili9325.c
 * @author  xdzmkus
 * @version V1.0
 * @date    2023
 * @brief   This file includes the LCD driver for ILI9325 LCD.
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
#include "ili9325.h"

extern void 	LCD_IO_Init(void);
extern uint16_t LCD_IO_ReadReg(uint8_t Reg);
extern void 	LCD_IO_WriteReg(uint8_t Reg, uint16_t Data);
extern void 	LCD_IO_WriteRegN(uint8_t Reg, uint16_t Data, uint32_t Count);
extern void 	LCD_IO_WriteRegArray(uint8_t Reg, uint16_t *pData, uint32_t Size);

static uint8_t lcd_rotation = 0;


/**
 * @brief  Get the LCD pixel Width.
 * @param  None
 * @retval The Lcd Pixel Width
 */
uint16_t ili9325_GetLcdPixelWidth(void)
{
	return lcd_rotation % 2 == 0 ? ILI9325_LCD_PIXEL_WIDTH : ILI9325_LCD_PIXEL_HEIGHT;
}

/**
 * @brief  Get the LCD pixel Height.
 * @param  None
 * @retval The Lcd Pixel Height
 */
uint16_t ili9325_GetLcdPixelHeight(void)
{
	return lcd_rotation % 2 == 0 ? ILI9325_LCD_PIXEL_HEIGHT : ILI9325_LCD_PIXEL_WIDTH;
}

/**
 * @brief  Initialize the ILI9325 LCD Component.
 * @param  None
 * @retval None
 */
void ili9325_Init(void)
{  
	  /* Start Initial Sequence --------------------------------------------------*/
	  LCD_IO_WriteReg(LCD_REG_0, 0x0001); /* Start internal OSC. */
	  LCD_IO_WriteReg(LCD_REG_1, 0x0100); /* Set SS and SM bit */
	  LCD_IO_WriteReg(LCD_REG_2, 0x0700); /* Set 1 line inversion */

	  LCD_IO_WriteReg(LCD_REG_4, 0x0000); /* Resize register */
	  LCD_IO_WriteReg(LCD_REG_8, 0x0202); /* Set the back porch and front porch */
	  LCD_IO_WriteReg(LCD_REG_9, 0x0000); /* Set non-display area refresh cycle ISC[3:0] */
	  LCD_IO_WriteReg(LCD_REG_10, 0x0000); /* FMARK function */
	  LCD_IO_WriteReg(LCD_REG_12, 0x0001); /* RGB interface setting */
	  LCD_IO_WriteReg(LCD_REG_13, 0x0000); /* Frame marker Position */
	  LCD_IO_WriteReg(LCD_REG_15, 0x0000); /* RGB interface polarity */

	  /* Adjust the Gamma Curve (ILI9325) ----------------------------------------*/
	  LCD_IO_WriteReg(LCD_REG_48, 0x0007);
	  LCD_IO_WriteReg(LCD_REG_49, 0x0302);
	  LCD_IO_WriteReg(LCD_REG_50, 0x0105);
	  LCD_IO_WriteReg(LCD_REG_53, 0x0206);
	  LCD_IO_WriteReg(LCD_REG_54, 0x0808);
	  LCD_IO_WriteReg(LCD_REG_55, 0x0206);
	  LCD_IO_WriteReg(LCD_REG_56, 0x0504);
	  LCD_IO_WriteReg(LCD_REG_57, 0x0007);
	  LCD_IO_WriteReg(LCD_REG_60, 0x0105);
	  LCD_IO_WriteReg(LCD_REG_61, 0x0808);

	  /* Set GRAM area -----------------------------------------------------------*/
	  LCD_IO_WriteReg(LCD_REG_96,  0xA700); /* Gate Scan Line(GS=1, scan direction is G320~G1) */
	  LCD_IO_WriteReg(LCD_REG_97,  0x0001); /* NDL,VLE, REV */
	  LCD_IO_WriteReg(LCD_REG_106, 0x0000); /* set scrolling line */

	  /* Partial Display Control -------------------------------------------------*/
	  LCD_IO_WriteReg(LCD_REG_128, 0x0000);
	  LCD_IO_WriteReg(LCD_REG_129, 0x0000);
	  LCD_IO_WriteReg(LCD_REG_130, 0x0000);
	  LCD_IO_WriteReg(LCD_REG_131, 0x0000);
	  LCD_IO_WriteReg(LCD_REG_132, 0x0000);
	  LCD_IO_WriteReg(LCD_REG_133, 0x0000);

	  /* Panel Control -----------------------------------------------------------*/
	  LCD_IO_WriteReg(LCD_REG_144, 0x0010);
	  LCD_IO_WriteReg(LCD_REG_146, 0x0000);
	  LCD_IO_WriteReg(LCD_REG_147, 0x0003);
	  LCD_IO_WriteReg(LCD_REG_149, 0x0110);
	  LCD_IO_WriteReg(LCD_REG_151, 0x0000);
	  LCD_IO_WriteReg(LCD_REG_152, 0x0000);

	  ili9325_SetRotation(0);

	  ili9325_SetDisplayWindow(0, 0, ili9325_GetLcdPixelWidth(), ili9325_GetLcdPixelHeight());

	  ili9325_SetCursor(0,0);

	  ili9325_DisplayOn();
}

/**
 * @brief  Get the ILI9325 ID.
 * @param  None
 * @retval The ILI9325 ID
 */
uint16_t ili9325_ReadID(void)
{
	LCD_IO_WriteReg(LCD_REG_0, 0x0001);
	return LCD_IO_ReadReg(LCD_REG_0);
}

void ili9325_Clear(uint16_t RGBCode)
{
	ili9325_SetDisplayWindow(0, 0, ili9325_GetLcdPixelWidth(), ili9325_GetLcdPixelHeight());

	ili9325_SetCursor(0, 0);

	LCD_IO_WriteRegN(LCD_REG_34, RGBCode, ILI9325_LCD_PIXEL_WIDTH * ILI9325_LCD_PIXEL_HEIGHT);
}

/**
 * @brief  Enables the Display.
 * @param  None
 * @retval None
 */
void ili9325_DisplayOn(void)
{
	/* Power On sequence -------------------------------------------------------*/
	LCD_IO_WriteReg(LCD_REG_16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
	LCD_IO_WriteReg(LCD_REG_17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
	LCD_IO_WriteReg(LCD_REG_18, 0x0000); /* VREG1OUT voltage */
	LCD_IO_WriteReg(LCD_REG_19, 0x0000); /* VDV[4:0] for VCOM amplitude*/

	LCD_IO_WriteReg(LCD_REG_16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
	LCD_IO_WriteReg(LCD_REG_17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
	LCD_IO_WriteReg(LCD_REG_18, 0x0139); /* VREG1OUT voltage */
	LCD_IO_WriteReg(LCD_REG_19, 0x1d00); /* VDV[4:0] for VCOM amplitude */

	LCD_IO_WriteReg(LCD_REG_41, 0x0013); /* VCM[4:0] for VCOMH */

	/* Display On */
	LCD_IO_WriteReg(LCD_REG_7, 0x0173); /* 262K color and display ON */
}

/**
 * @brief  Disables the Display.
 * @param  None
 * @retval None
 */
void ili9325_DisplayOff(void)
{
	/* Power Off sequence ------------------------------------------------------*/
	LCD_IO_WriteReg(LCD_REG_16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
	LCD_IO_WriteReg(LCD_REG_17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
	LCD_IO_WriteReg(LCD_REG_18, 0x0000); /* VREG1OUT voltage */
	LCD_IO_WriteReg(LCD_REG_19, 0x0000); /* VDV[4:0] for VCOM amplitude*/

	LCD_IO_WriteReg(LCD_REG_41, 0x0000); /* VCM[4:0] for VCOMH */

	/* Display Off */
	LCD_IO_WriteReg(LCD_REG_7, 0x0);
}

void ili9325_SetRotation(uint8_t rotation)
{
	lcd_rotation = rotation;

	/* set GRAM write direction and BGR = 1 */
	/* (D5:D4) I/D[1:0] = 00 Horizontal:decrement  Vertical:decrement */
	/* (D5:D4) I/D[1:0] = 01 Horizontal:increment  Vertical:decrement */
	/* (D5:D4) I/D[1:0] = 10 Horizontal:decrement  Vertical:increment */
	/* (D5:D4) I/D[1:0] = 11 Horizontal:increment  Vertical:increment */
	/* (D3) AM=0 (the address is updated in horizontal writing direction) */
	/* (D3) AM=1 (the address is updated in vertical writing direction) */
	switch (rotation % 4)
	{
		case 0:
			LCD_IO_WriteReg(LCD_REG_3, 0x1030);
			break;
		case 1:
			LCD_IO_WriteReg(LCD_REG_3, 0x1018);
			break;
		case 2:
			LCD_IO_WriteReg(LCD_REG_3, 0x1000);
			break;
		case 3:
			LCD_IO_WriteReg(LCD_REG_3, 0x1028);
			break;
		default:
			break;
	}
}

/**
 * @brief  Set Cursor position.
 * @param  Xpos: specifies the X position.
 * @param  Ypos: specifies the Y position.
 * @retval None
 */
void ili9325_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	if(Xpos >= ili9325_GetLcdPixelWidth() || Ypos >= ili9325_GetLcdPixelHeight())
		return;

	uint16_t x = 0;
	uint16_t y = 0;

	switch (lcd_rotation % 4)
	{
		case 0:
			x = Xpos;
			y = Ypos;
			break;
		case 1:
			x = Ypos;
			y = ILI9325_LCD_PIXEL_HEIGHT - 1 - Xpos;
			break;
		case 2:
			x = ILI9325_LCD_PIXEL_WIDTH - 1 - Xpos;
			y = ILI9325_LCD_PIXEL_HEIGHT - 1 - Ypos;
			break;
		case 3:
			x = ILI9325_LCD_PIXEL_WIDTH - 1 - Ypos;;
			y = Xpos;
			break;
		default:
			break;
	}

	LCD_IO_WriteReg(LCD_REG_32, x);
	LCD_IO_WriteReg(LCD_REG_33, y);
}

/**
 * @brief  Sets a display window
 * @param  Xpos:   specifies the X bottom left position.
 * @param  Ypos:   specifies the Y bottom left position.
 * @param  Height: display window height.
 * @param  Width:  display window width.
 * @retval None
 */
void ili9325_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
	if(Xpos >= ili9325_GetLcdPixelWidth() || Ypos >= ili9325_GetLcdPixelHeight())
		return;

	if (Xpos + Width > ili9325_GetLcdPixelWidth() || Ypos + Height > ili9325_GetLcdPixelHeight())
		return;

	uint16_t x1 = 0;
	uint16_t y1 = 0;
	uint16_t x2 = ILI9325_LCD_PIXEL_WIDTH;
	uint16_t y2 = ILI9325_LCD_PIXEL_HEIGHT;

	switch (lcd_rotation % 4)
	{
		case 0:
			x1 = Xpos;
			y1 = Ypos;
			x2 = Xpos + Width;
			y2 = Ypos + Height;
			break;
		case 1:
			x1 = Ypos;
			y1 = ILI9325_LCD_PIXEL_HEIGHT - Width - Xpos;
			x2 = Ypos + Height;
			y2 = ILI9325_LCD_PIXEL_HEIGHT - Xpos;
			break;
		case 2:
			x1 = ILI9325_LCD_PIXEL_WIDTH - Width - Xpos;
			y1 = ILI9325_LCD_PIXEL_HEIGHT - Height - Ypos;
			x2 = ILI9325_LCD_PIXEL_WIDTH - Xpos;
			y2 = ILI9325_LCD_PIXEL_HEIGHT - Ypos;
			break;
		case 3:
			x1 = ILI9325_LCD_PIXEL_WIDTH - Height - Ypos;
			y1 = Xpos;
			x2 = ILI9325_LCD_PIXEL_WIDTH - Ypos;
			y2 = Xpos + Width;
			break;
		default:
			break;
	}

	/* Horizontal GRAM Start Address */
	LCD_IO_WriteReg(LCD_REG_80, x1);
	/* Horizontal GRAM End Address */
	LCD_IO_WriteReg(LCD_REG_81, x2 - 1);

	/* Vertical GRAM Start Address */
	LCD_IO_WriteReg(LCD_REG_82, y1);
	/* Vertical GRAM End Address */
	LCD_IO_WriteReg(LCD_REG_83, y2 - 1);
}

/**
 * @brief  Write pixel.
 * @param  Xpos: specifies the X position.
 * @param  Ypos: specifies the Y position.
 * @param  RGBCode: the RGB pixel color
 * @retval None
 */
void ili9325_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode)
{
	if(Xpos >= ili9325_GetLcdPixelWidth() || Ypos >= ili9325_GetLcdPixelHeight())
		return;

	/* Set Cursor */
	ili9325_SetCursor(Xpos, Ypos);

	/* Prepare to write GRAM */
	LCD_IO_WriteReg(LCD_REG_34, RGBCode);
}

/**
 * @brief  Read pixel.
 * @param  None
 * @retval The RGB pixel color
 */
uint16_t ili9325_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
	if(Xpos >= ili9325_GetLcdPixelWidth() || Ypos >= ili9325_GetLcdPixelHeight())
		return 0;

	/* Set Cursor */
	ili9325_SetCursor(Xpos, Ypos);

	/* Read 16-bit Reg */
	return LCD_IO_ReadReg(LCD_REG_34);
}

/**
 * @brief  Draw vertical line.
 * @param  Xpos:     specifies the X position.
 * @param  Ypos:     specifies the Y position.
 * @param  Length:   specifies the Line length.
 * @param  RGBCode:  specifies the RGB color
 * @retval None
 */
void ili9325_DrawHLine(int16_t Xpos, int16_t Ypos, uint16_t Length, uint16_t RGBCode)
{
	if(Ypos < 0 || Xpos >= ili9325_GetLcdPixelWidth() || Ypos >= ili9325_GetLcdPixelHeight())
		return;

	int16_t newLength = Length;

    if (Xpos < 0)
	{
		newLength += Xpos;
		if (newLength <= 0) return;
		Xpos = 0;
	}
    else if (Xpos + Length > ili9325_GetLcdPixelWidth())
    {
    	newLength = ili9325_GetLcdPixelWidth() - Xpos;
    }

	/* Set Cursor */
	ili9325_SetCursor(Xpos, Ypos);

	/* Write 16-bit GRAM Reg */
	LCD_IO_WriteRegN(LCD_REG_34, RGBCode, newLength);
}

/**
 * @brief  Draw vertical line.
 * @param  Xpos:     specifies the X position.
 * @param  Ypos:     specifies the Y position.
 * @param  Length:   specifies the Line length.
 * @param  RGBCode:  specifies the RGB color
 * @retval None
 */
void ili9325_DrawVLine(int16_t Xpos, int16_t Ypos, uint16_t Length, uint16_t RGBCode)
{
	if(Xpos < 0 || Xpos >= ili9325_GetLcdPixelWidth() || Ypos >= ili9325_GetLcdPixelHeight())
		return;

	int16_t newLength = Length;

    if (Ypos < 0)
	{
		newLength += Ypos;
		if (newLength <= 0) return;
		Ypos = 0;
	}
    else if (Ypos + Length > ili9325_GetLcdPixelHeight())
    {
    	newLength = ili9325_GetLcdPixelHeight() - Ypos;
    }

	/* Set Cursor */
	ili9325_SetCursor(Xpos, Ypos);

	// save direction
	uint16_t direction = LCD_IO_ReadReg(LCD_REG_3);

	// invert direction
	LCD_IO_WriteReg(LCD_REG_3, direction ^ 0b1000);

	/* Write 16-bit GRAM Reg */
	LCD_IO_WriteRegN(LCD_REG_34, RGBCode, Length);

	// restore direction
	LCD_IO_WriteReg(LCD_REG_3, direction);
}

/**
 * @brief  Displays a bitmap picture.
 * @param  BmpAddress: Bmp picture address.
 * @param  Xpos: Bmp X position in the LCD
 * @param  Ypos: Bmp Y position in the LCD
 * @retval None
 */
void ili9325_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp)
{
	uint32_t index = 0, size = 0;
	/* Read bitmap size */
	size = *(volatile uint16_t *) (pbmp + 2);
	size |= (*(volatile uint16_t *) (pbmp + 4)) << 16;
	/* Get bitmap data address offset */
	index = *(volatile uint16_t *) (pbmp + 10);
	index |= (*(volatile uint16_t *) (pbmp + 12)) << 16;
	size = (size - index)/2;
	pbmp += index;

	/* Set Cursor */
	ili9325_SetCursor(Xpos, Ypos);

	/* Write 16-bit GRAM Reg */
	LCD_IO_WriteRegArray(LCD_REG_34, (uint16_t*)pbmp, size);
}

/**
 * @brief  Displays picture.
 * @param  Xpos: Image X position in the LCD
 * @param  Ypos: Image Y position in the LCD
 * @param  pdata: picture address.
 * @param  Size: Image size in the LCD
 * @retval None
 */
void ili9325_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t *pdata, uint32_t Size)
{
	/* Set Cursor */
	ili9325_SetCursor(Xpos, Ypos);

	/* Write 16-bit GRAM Reg */
	LCD_IO_WriteRegArray(LCD_REG_34, pdata, Size);
}

LCD_DrvTypeDef ili9325_drv =
{
	ili9325_GetLcdPixelWidth,
	ili9325_GetLcdPixelHeight,
	ili9325_Init,
	ili9325_ReadID,
	ili9325_Clear,
	ili9325_DisplayOn,
	ili9325_DisplayOff,
	ili9325_SetRotation,
	ili9325_SetCursor,
	ili9325_SetDisplayWindow,
	ili9325_WritePixel,
	ili9325_ReadPixel,
	ili9325_DrawHLine,
	ili9325_DrawVLine,
	ili9325_DrawBitmap,
	ili9325_DrawRGBImage
};


/*
 * lcd_io.c
 *
 */

#include "lcd_io.h"

#include "stm32f1xx.h"

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))


#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08


#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)


#define LCD_CS  PCout(8)
#define LCD_RS	PCout(9)
#define LCD_WR  PCout(10)
#define LCD_RD	PCout(11)

void _LCD_IO_Config_DATA_Input()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  	GPIO_InitStructure.Pull = GPIO_PULLUP;
  	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  	GPIO_InitStructure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  	GPIO_InitStructure.Pull = GPIO_PULLUP;
  	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void _LCD_IO_Config_DATA_Output()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  	GPIO_InitStructure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief  Configures the LCD_SPI interface.
  */
void LCD_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
  	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  	_LCD_IO_Config_DATA_Output();
}

//========================================================================
// **                                                                    **
// ** nCS       ----\__________________________________________/-------  **
// ** RS        ------\____________/-----------------------------------  **
// ** nRD       -------------------------\_____/-----------------------  **
// ** nWR       --------\_______/--------------------------------------  **
// ** DB[0:15]  ---------[index]----------[data]-----------------------  **
// **                                                                    **
//========================================================================
uint16_t LCD_IO_ReadReg(uint8_t Reg)
{
	LCD_CS = 0;

	// write register index
	LCD_RS = 0;
	LCD_RD = 1;
	GPIOC->ODR = (GPIOC->ODR&0xff00)|(Reg&0x00ff);
	GPIOB->ODR = (GPIOB->ODR&0x00ff)|(Reg&0xff00);
	LCD_WR = 0;
	LCD_WR = 1;
	LCD_RS = 1;

	// read data
	uint16_t readvalue = 0;
	LCD_RD = 0;

	_LCD_IO_Config_DATA_Input();

	readvalue = ((GPIOB->IDR&0xff00)|(GPIOC->IDR&0x00ff));

	_LCD_IO_Config_DATA_Output();

	LCD_RD = 1;

	LCD_CS = 1;

	return readvalue;
}

/************************************************************************
 **                                                                    **
 ** nCS       ----\__________________________________________/-------  **
 ** RS        ------\____________/-----------------------------------  **
 ** nRD       -------------------------------------------------------  **
 ** nWR       --------\_______/--------\_____/-----------------------  **
 ** DB[0:15]  ---------[index]----------[data]-----------------------  **
 **                                                                    **
 ************************************************************************/
void LCD_IO_WriteReg(uint8_t Reg, uint16_t Data)
{
	LCD_IO_WriteRegN(Reg, Data, 1);
}

void LCD_IO_WriteRegN(uint8_t Reg, uint16_t Data, uint32_t Count)
{
	LCD_CS = 0;

	// write register index
	LCD_RS = 0;
	LCD_RD = 1;
	GPIOC->ODR = (GPIOC->ODR&0xff00)|(Reg&0x00ff);
	GPIOB->ODR = (GPIOB->ODR&0x00ff)|(Reg&0xff00);
	LCD_WR = 0;
	LCD_WR = 1;
	LCD_RS = 1;

	// write register data
	for(uint32_t index=0; index < Count; index++)
	{
		GPIOC->ODR = (GPIOC->ODR&0xff00)|(Data&0x00ff);
		GPIOB->ODR = (GPIOB->ODR&0x00ff)|(Data&0xff00);
		LCD_WR = 0;
		LCD_WR = 1;
	}

	LCD_CS = 1;
}

void LCD_IO_WriteRegArray(uint8_t Reg, uint16_t *pData, uint32_t Size)
{
	LCD_CS = 0;

	// write register index
	LCD_RS = 0;
	LCD_RD = 1;
	GPIOC->ODR = (GPIOC->ODR&0xff00)|(Reg&0x00ff);
	GPIOB->ODR = (GPIOB->ODR&0x00ff)|(Reg&0xff00);
	LCD_WR = 0;
	LCD_WR = 1;
	LCD_RS = 1;

	// write register data
	for(uint32_t index=0; index < Size; index++)
	{
		GPIOC->ODR = (GPIOC->ODR&0xff00)|(pData[index]&0x00ff);
		GPIOB->ODR = (GPIOB->ODR&0x00ff)|(pData[index]&0xff00);
		LCD_WR = 0;
		LCD_WR = 1;
	}

	LCD_CS = 1;
}


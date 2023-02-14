/*
 * touch_io.c
 *
 */

#include "touch_io.h"

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

#define PEN  PCin(13)  //PC13 INT
#define DOUT PAin(6)   //PA6  MISO
#define TDIN PAout(7)  //PA7  MOSI
#define TCLK PAout(5)  //PA5  SCLK
#define TCS  PAout(4)  //PA4  CS

#define TP_CS_Pin GPIO_PIN_4
#define TP_CS_GPIO_Port GPIOA
#define TP_SCLK_Pin GPIO_PIN_5
#define TP_SCLK_GPIO_Port GPIOA
#define TP_MISO_Pin GPIO_PIN_6
#define TP_MISO_GPIO_Port GPIOA
#define TP_MOSI_Pin GPIO_PIN_7
#define TP_MOSI_GPIO_Port GPIOA

void TOUCH_INT_Enable()
{
	SET_BIT(EXTI->IMR, EXTI_IMR_IM13);
}

void TOUCH_INT_Disable()
{
	CLEAR_BIT(EXTI->IMR, EXTI_IMR_IM13);
}

bool TOUCH_PEN_GetState()
{
	return READ_BIT(GPIOC->IDR, GPIO_IDR_IDR13); // PEN
}

/**
  * @brief  Configures the TOUCH SPI interface.
  */
void TOUCH_IO_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(TP_CS_GPIO_Port, TP_CS_Pin, GPIO_PIN_SET);

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(GPIOA, TP_SCLK_Pin|TP_MOSI_Pin, GPIO_PIN_RESET);

	  /*Configure GPIO pins : TP_CS_Pin TP_SCLK_Pin TP_MOSI_Pin */
	  GPIO_InitStruct.Pin = TP_CS_Pin|TP_SCLK_Pin|TP_MOSI_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	  /*Configure GPIO pin : TP_MISO_Pin */
	  GPIO_InitStruct.Pin = TP_MISO_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_PULLUP;
	  HAL_GPIO_Init(TP_MISO_GPIO_Port, &GPIO_InitStruct);
}

uint16_t TOUCH_IO_Read(uint8_t CMD)
{
	TCLK = 0;
	TCS = 0;

	for(uint8_t count=0; count<8; count++)
	{
		if(CMD&0x80) TDIN = 1;
		else TDIN = 0;
		CMD <<= 1;
		TCLK = 0;
		TCLK = 1;
	}

	for(uint8_t i=100; i>0; i--); // delay_us(6);

	TCLK = 1;
	TCLK = 0;

	uint16_t Num = 0;

	for(uint8_t count=0; count<16; count++)
	{
		Num <<= 1;
		TCLK = 0;
		TCLK = 1;
		if(DOUT) ++Num;
	}

	Num >>= 4;
	TCS = 1;

	return Num;
}

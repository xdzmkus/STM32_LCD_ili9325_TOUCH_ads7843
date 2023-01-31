/*
 * lcd_io.h
 *
 */

#ifndef _LCD_IO_H_
#define _LCD_IO_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

/* LCD IO functions */

void     LCD_IO_Init(void);

uint16_t LCD_IO_ReadReg(uint8_t Reg);

void     LCD_IO_WriteReg(uint8_t Reg, uint16_t Data);
void     LCD_IO_WriteRegN(uint8_t Reg, uint16_t Data, uint32_t Count);
void     LCD_IO_WriteRegArray(uint8_t Reg, uint16_t *pData, uint32_t Size);

#ifdef __cplusplus
}
#endif

#endif /* _LCD_IO_H_ */

/*
 * touch_io.h
 *
 */

#ifndef _TOUCH_IO_H_
#define _TOUCH_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define CMD_RDX 0XD0
#define CMD_RDY	0X90

void TOUCH_INT_Enable();
void TOUCH_INT_Disable();

bool TOUCH_PEN_GetState();

void TOUCH_IO_Init(void);

uint16_t TOUCH_IO_Read(uint8_t CMD);

#ifdef __cplusplus
}
#endif

#endif /* _TOUCH_IO_H_ */

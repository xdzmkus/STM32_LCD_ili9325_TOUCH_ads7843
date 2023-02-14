#ifndef __TOUCH_H
#define __TOUCH_H	

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void Touch_Init(void);

void Touch_SetRotation(uint8_t rotation);

bool Touch_GetPoint(uint16_t *x, uint16_t *y);

bool Touch_isTouched();

#ifdef __cplusplus
}
#endif

#endif

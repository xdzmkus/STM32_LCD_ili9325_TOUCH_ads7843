#ifndef __TOUCH_H
#define __TOUCH_H	

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void Touch_callback();

void Touch_Init(void);

void Touch_SetRotation(uint8_t rotation);

bool Touch_GetPoint(uint16_t *x, uint16_t *y);

#ifdef __cplusplus
}
#endif

#endif

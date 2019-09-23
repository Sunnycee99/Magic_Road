#ifndef __PM2_5_H
#define __PM2_5_H
#include "stm32f10x.h"

void PM2_5_Init(void);
u16 PM2_5_GetADC(u8 flag);
u16 PM2_5_GetValue(u8 flag);

#endif


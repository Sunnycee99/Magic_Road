#ifndef __LSENSOR_H
#define __LSENSOR_H
#include "stm32f10x.h"

void Light_Sensor_Init(void);
float Light_Sensor_GetValue(u8 ch);

#endif


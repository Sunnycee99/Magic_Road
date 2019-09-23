#ifndef __STATUS_H
#define __STATUS_H
#include "stm32f10x.h"


//水泵控制
  //水泵开
#define WATER1_ON (GPIOC->BRR |= 1<<10)
  //水泵关
#define WATER1_OFF (GPIOC->BSRR |= 1<<10)
#define WATER1_STATUS ((GPIOC->ODR>>10)&1)


typedef struct
{
	float temp1;
	float temp2;
	u16 dust1;
	u16 dust2;
	
	//路灯控制 0-自适应 1-人工控制
	u8 light1_1;
	u8 light1_2;
	u8 light1_3;
	u8 light2_1;
	u8 light2_2;
	u8 light2_3;
	
	//水泵控制 0-自适应 1-人工控制
	u8 water1;
	u8 water2;
	
	//深夜模式标志位 0-非深夜 1-深夜
	u8 deep_light;
} STATUS_INFO;

extern STATUS_INFO All_Status;  //外部引用

#endif


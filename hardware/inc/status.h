#ifndef __STATUS_H
#define __STATUS_H
#include "stm32f10x.h"


//ˮ�ÿ���
  //ˮ�ÿ�
#define WATER1_ON (GPIOC->BRR |= 1<<10)
  //ˮ�ù�
#define WATER1_OFF (GPIOC->BSRR |= 1<<10)
#define WATER1_STATUS ((GPIOC->ODR>>10)&1)


typedef struct
{
	float temp1;
	float temp2;
	u16 dust1;
	u16 dust2;
	
	//·�ƿ��� 0-����Ӧ 1-�˹�����
	u8 light1_1;
	u8 light1_2;
	u8 light1_3;
	u8 light2_1;
	u8 light2_2;
	u8 light2_3;
	
	//ˮ�ÿ��� 0-����Ӧ 1-�˹�����
	u8 water1;
	u8 water2;
	
	//��ҹģʽ��־λ 0-����ҹ 1-��ҹ
	u8 deep_light;
} STATUS_INFO;

extern STATUS_INFO All_Status;  //�ⲿ����

#endif


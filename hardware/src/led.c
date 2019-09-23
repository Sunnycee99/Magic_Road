#include "stm32f10x.h"
#include "led.h"

void LED_Init(void)
{
	//定义IO结构体
	GPIO_InitTypeDef GPIOC_InitStructure, GPIOA_InitStructure;

	//使能IO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//初始化GPIOC
  GPIOC_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOC_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_10;
	GPIOC_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	//初始化GPIOA
	GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOA_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC, &GPIOC_InitStructure);
	GPIO_Init(GPIOA, &GPIOA_InitStructure);

	GPIO_SetBits(GPIOA, GPIO_Pin_12);
	GPIO_SetBits(GPIOC, GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_10);
}


#include "lsensor.h"
#include "stm32f10x.h"
#include "delay.h"

void Light_Sensor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);  //开启C和ADC时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);  //ADC时钟6分频，使其不超过12MHz

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;  //模拟输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;  //PA4 为通道4 PA5 5
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	ADC_DeInit(ADC1);  //复位ADC1
	
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;  //是否连续转换
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;  //数据对齐方式
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //使用软件触发
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_NbrOfChannel = 1;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;  //不开启扫描模式
	ADC_Init(ADC1, &ADC_InitStruct);
	
	ADC_Cmd(ADC1, ENABLE);  //使能ADC1
	ADC_ResetCalibration(ADC1);  //复位校准
	while(ADC_GetResetCalibrationStatus(ADC1));  //等待复位校准
	ADC_StartCalibration(ADC1);  //校准
	while(ADC_GetCalibrationStatus(ADC1));
}

u16 Light_Sensor_GetADC(u8 ch)
{
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //软件转换启动
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
//	delay_ms(5);
	return ADC_GetConversionValue(ADC1);
}
float Light_Sensor_GetValue(u8 ch)
{
	u8 i;
	u32 sum = 0;
	float temp;
	for(i=0;i<5;i++)
	{
		sum += Light_Sensor_GetADC(ch);
		DelayXms(5);
	}
	sum = sum/5;
	temp = (float)sum*3.3/4096;
	
	return temp;
}


#include "lsensor.h"
#include "stm32f10x.h"
#include "delay.h"

void Light_Sensor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);  //����C��ADCʱ��
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);  //ADCʱ��6��Ƶ��ʹ�䲻����12MHz

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;  //ģ������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;  //PA4 Ϊͨ��4 PA5 5
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	ADC_DeInit(ADC1);  //��λADC1
	
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;  //�Ƿ�����ת��
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;  //���ݶ��뷽ʽ
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //ʹ���������
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_NbrOfChannel = 1;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;  //������ɨ��ģʽ
	ADC_Init(ADC1, &ADC_InitStruct);
	
	ADC_Cmd(ADC1, ENABLE);  //ʹ��ADC1
	ADC_ResetCalibration(ADC1);  //��λУ׼
	while(ADC_GetResetCalibrationStatus(ADC1));  //�ȴ���λУ׼
	ADC_StartCalibration(ADC1);  //У׼
	while(ADC_GetCalibrationStatus(ADC1));
}

u16 Light_Sensor_GetADC(u8 ch)
{
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //���ת������
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


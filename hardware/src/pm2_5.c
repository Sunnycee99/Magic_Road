#include "pm2_5.h"
#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "status.h"

void PM2_5_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE);  //����C��ADCʱ��
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);  //ADCʱ��6��Ƶ��ʹ�䲻����12MHz

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;  //ģ������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  //PA6 Ϊͨ��6 PA7 7
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  //LED���
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;  //PB0 PB1
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
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

//��ȡһ��ADC��ֵ
u16 PM2_5_GetADC(u8 flag)
{
	if(flag==0)
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
		DelayUs(280);
		ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_239Cycles5);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //���ת������
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
		
		//��ʱ19us����Ϊ����������AD����������Ϊ239.5������ADת��һ�����ʱ21us��19��21�ټ�280�պ���320us,�������˵�ĸߵ�ƽ������ʱ��Ϊ0.32ms
		DelayUs(40);
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
		DelayUs(9680);
	}
	if(flag==1)
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);
		DelayUs(280);
		ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_239Cycles5);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //���ת������
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
		
		DelayUs(40);
		GPIO_SetBits(GPIOB, GPIO_Pin_1);
		DelayUs(9680);
	}
	return ADC_GetConversionValue(ADC1);
}

//��ÿ�������ֵ
u16 PM2_5_GetValue(u8 flag)
{
	u8 i;
	float vol;  //��ȡ��ѹֵ
	u16 data[9];
	u8 max_index = 0, min_index = 0;
	u16 sum = 0, dust;
	
	//��ȡADCֵ���õ����ֵ����Сֵ
	for(i=0;i<8;i++)
	{
		data[i] = PM2_5_GetADC(flag);
		if(data[i]>data[max_index])
			max_index = i;
		if(data[i]<data[min_index])
			min_index = i;
		sum += data[i];
	}
	//ȥ�����ֵ����Сֵ
	sum = sum -(data[min_index]+data[max_index]);
	
	vol = (double)sum/4096*3.3*2/6;  //��Ϊʹ���˷�ѹ  ֻȡ����һ��ĵ�ѹֵ  ȥ��һ�����ֵ����Сֵ��ʣ6��
	dust = (0.17*vol-0.1)*1000; ;
	if(flag==0)
		All_Status.dust1 = dust; 
	if(flag==1)
		All_Status.dust2 = dust; 
	UsartPrintf(USART_DEBUG, "Dust:%d\r\n", dust);
	return dust;
}


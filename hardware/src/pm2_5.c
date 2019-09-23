#include "pm2_5.h"
#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "status.h"

void PM2_5_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE);  //开启C和ADC时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);  //ADC时钟6分频，使其不超过12MHz

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;  //模拟输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  //PA6 为通道6 PA7 7
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  //LED输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;  //PB0 PB1
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
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

//读取一次ADC的值
u16 PM2_5_GetADC(u8 flag)
{
	if(flag==0)
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
		DelayUs(280);
		ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_239Cycles5);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //软件转换启动
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
		
		//延时19us，因为这里我设了AD采样的周期为239.5，所以AD转换一次需耗时21us，19加21再加280刚好是320us,这跟上面说的高电平持续的时间为0.32ms
		DelayUs(40);
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
		DelayUs(9680);
	}
	if(flag==1)
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);
		DelayUs(280);
		ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_239Cycles5);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //软件转换启动
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
		
		DelayUs(40);
		GPIO_SetBits(GPIOB, GPIO_Pin_1);
		DelayUs(9680);
	}
	return ADC_GetConversionValue(ADC1);
}

//获得空气质量值
u16 PM2_5_GetValue(u8 flag)
{
	u8 i;
	float vol;  //存取电压值
	u16 data[9];
	u8 max_index = 0, min_index = 0;
	u16 sum = 0, dust;
	
	//读取ADC值并得到最大值和最小值
	for(i=0;i<8;i++)
	{
		data[i] = PM2_5_GetADC(flag);
		if(data[i]>data[max_index])
			max_index = i;
		if(data[i]<data[min_index])
			min_index = i;
		sum += data[i];
	}
	//去掉最大值和最小值
	sum = sum -(data[min_index]+data[max_index]);
	
	vol = (double)sum/4096*3.3*2/6;  //因为使用了分压  只取到了一半的电压值  去掉一个最大值和最小值还剩6个
	dust = (0.17*vol-0.1)*1000; ;
	if(flag==0)
		All_Status.dust1 = dust; 
	if(flag==1)
		All_Status.dust2 = dust; 
	UsartPrintf(USART_DEBUG, "Dust:%d\r\n", dust);
	return dust;
}


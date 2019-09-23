//单片机头文件
#include "stm32f10x.h"

//网络协议层
#include "onenet.h"

//网络设备
#include "esp8266.h"

//硬件驱动
#include "delay.h"
#include "usart.h"
#include "beep.h"
#include "led.h"
#include "pm2_5.h"
#include "lsensor.h"
#include "ds18b20.h"

//C库
#include <string.h>

//状态结构体申明
#include "status.h"

STATUS_INFO All_Status;
/*
************************************************************
*	函数名称：	Hardware_Init
*
*	函数功能：	硬件初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		初始化单片机功能以及外接设备
************************************************************
*/
void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置

	Delay_Init();									//systick初始化
	
	Usart1_Init(115200);							//串口1，打印信息用
	
	Usart2_Init(115200);							//串口2，驱动ESP8266用
	
	Beep_Init();									//蜂鸣器初始化
	
	LED_Init();
	
	PM2_5_Init();
	
	DS18B20_Init();
	
	Light_Sensor_Init();
	
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	
	//状态初始化
	All_Status.deep_light = 0;
	All_Status.dust1 = 0;
	All_Status.dust2 = 0;
	All_Status.light1_1 = 0;
	All_Status.light1_2 = 0;
	All_Status.light1_3 = 0;
	All_Status.light2_1 = 0;
	All_Status.light2_2 = 0;
	All_Status.light2_3 = 0;
	All_Status.temp1 = 0;
	All_Status.temp2 = 0;
	All_Status.water1 = 0;
	All_Status.water2 = 0;
}

/*
************************************************************
*	函数名称：	main
*
*	函数功能：	
*
*	入口参数：	无
*
*	返回参数：	0
*
*	说明：		
************************************************************
*/
int main(void)
{
	
	unsigned short timeCount = 0;	//发送间隔变量
	
	unsigned char *dataPtr = NULL;
	
	u16 dust;
	
	float temp;
	
	Hardware_Init();				//初始化外围硬件
	
	ESP8266_Init();					//初始化ESP8266
	
	while(OneNet_DevLink())			//接入OneNET
		DelayXms(500);
	
	Beep_Set(BEEP_ON);				//鸣叫提示接入成功
	DelayXms(250);
	Beep_Set(BEEP_OFF);

	while(1)
	{
		
		if(++timeCount >= 150)									//发送间隔5s
		{
			//****************************灰尘代码****************************************//
			dust = PM2_5_GetValue(0);
			All_Status.dust1 = dust;
			
			//*******************************温度代码**************************************//
			temp = DS18B20_GetValue(0);
			All_Status.temp1 = temp;
			
			//扬尘浓度过高打开水泵，水泵自适应调节
			if(All_Status.water1==0)
			{
				if(dust>200 || temp>29)
					WATER1_ON;
				else
					WATER1_OFF;
			}
			
			UsartPrintf(USART_DEBUG, "OneNet_SendData\r\n");
			OneNet_SendData();									//发送数据
			
			timeCount = 0;
			ESP8266_Clear();
		}
		//非深夜模式
		if(All_Status.deep_light==0)
		{
			//亮度过低打开路灯
			if(Light_Sensor_GetValue(ADC_Channel_4)>0.8)  //电压值大于0.8V  亮度越大电压越低
			{
				if(All_Status.light1_1==0)
					LED1_1_ON;
				if(All_Status.light1_2==0)
					LED1_2_ON;
				if(All_Status.light1_3==0)
					LED1_3_ON;
			}
			else
			{
				if(All_Status.light1_1==0)
					LED1_1_OFF;
				if(All_Status.light1_2==0)
					LED1_2_OFF;
				if(All_Status.light1_3==0)
					LED1_3_OFF;
			}
		}
		//深夜模式
		else
		{
			if(All_Status.light1_1==0)
				LED1_1_ON;
			if(All_Status.light1_2==0)
				LED1_2_ON;
			//关掉一盏灯
			if(All_Status.light1_3==0)
				LED1_3_OFF;
		}
		
		dataPtr = ESP8266_GetIPD(10);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		
		DelayXms(10);
	
	}
}

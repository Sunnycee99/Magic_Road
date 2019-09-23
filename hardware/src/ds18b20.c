#include "ds18b20.h"
#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"

void DS18B20_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);  //开启端口复用
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);				//禁止JTAG功能
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void DS18B20_Reset(u8 flag)
{
	if(flag==0)
	{
		DS18_1_OUT;
		DS18_1_LOW;
		DelayUs(750);
		DS18_1_HIGH;
		//延时等待
		DelayUs(15);
	}
	else
	{
		DS18_2_OUT;
		DS18_2_LOW;
		DelayUs(750);
		DS18_2_HIGH;
		//延时等待
		DelayUs(15);
	}
}

//检测存在脉冲
u8 DS18B20_Check(u8 flag)
{
	if(flag==0)
	{
		u8 retry = 0;
		DS18_1_IN;
		while(DS18_1_STATUS && retry<240)  //等待拉低总线，（60-240us）
		{
			DelayUs(1);
			retry++;
		}
		if(retry>=240)
			return 1;
		retry = 0;
		DS18_1_IN;
		while(!DS18_1_STATUS && retry<240)  //判断是否释放总线
		{
			DelayUs(1);
			retry++;
		}
		if(retry>=240)
			return 1;
		else
			return 0;
	}
}

//flag判断写哪一个传感器
void DS18B20_Write_Byte(u8 flag, u8 order)
{
	u8 i;
	if(flag==0)
	{
		DS18_1_OUT;
		for(i=0;i<8;i++)
		{
			//写0时序
			if((order&0x01)==0)
			{
				DS18_1_LOW;
				DelayUs(60);
				DS18_1_HIGH;
				DelayUs(2);
			}
			//写1时序
			else
			{
				DS18_1_LOW;
				DelayUs(2);
				DS18_1_HIGH;
				DelayUs(60);
			}
			order >>= 1;
		}
		DS18_1_HIGH;
	}
	
	else
	{
		DS18_2_OUT;
		for(i=0;i<8;i++)
		{
			//写0时序
			if((order&0x01)==0)
			{
				DS18_2_LOW;
				DelayUs(65);
				DS18_2_HIGH;
				DelayUs(2);
			}
			//写1时序
			else
			{
				DS18_2_LOW;
				DelayUs(2);
				DS18_2_HIGH;
				DelayUs(65);
			}
			order >>= 1;
		}
		DS18_2_HIGH;
	}
}

u8 DS18B20_Read(u8 flag)
{
	u8 data = 0, temp;
	u8 i;
	
	if(flag==0)
	{
		for(i=0;i<8;i++)
		{
			DS18_1_OUT;
			DS18_1_LOW;
			DelayUs(2);
			DS18_1_HIGH;
			DS18_1_IN;
			DelayUs(12);
			temp = DS18_1_STATUS;
			data = (data>>1)|(temp<<7);
			DelayUs(60);
		}
	}
	else
	{
		for(i=0;i<8;i++)
		{
			DS18_1_LOW;
			DelayUs(2);
			DS18_1_HIGH;
			DelayUs(12);
			DS18_2_IN;
			temp = DS18_2_STATUS;
			data = (data>>1)|(temp<<7);
			DelayUs(60);
		}
	}
	
	return data;
}

float DS18B20_GetValue(u8 flag)
{
	u16 value1, value2, sum = 0;
	u8 i;
	float temp;
	
	if(flag==0)
	{
		for(i=0;i<3;i++)
		{
			DS18B20_Reset(0);
			DS18B20_Check(0);
			DS18B20_Write_Byte(0, 0xCC);  //跳过ROM寻址
			DS18B20_Write_Byte(0, 0x44);  //启动一次温度转换
			DS18B20_Reset(0);
			DS18B20_Check(0);
			DS18B20_Write_Byte(0,0xCC);
			DS18B20_Write_Byte(0, 0xBE);  //读值命令
			
			value1 = DS18B20_Read(0);  //先读低8位
			value2 = DS18B20_Read(0);  //读高8位
			value1 = (value2<<8) + value1;
			sum += value1&0x0fff;  //去掉符号位
		}
	}
	else
	{
		for(i=0;i<3;i++)
		{
			DS18B20_Reset(1);
			DS18B20_Check(1);
			DS18B20_Write_Byte(1, 0xCC);  //跳过ROM寻址
			DS18B20_Write_Byte(1, 0x44);  //启动一次温度转换
			DS18B20_Reset(1);
			DS18B20_Check(1);
			DS18B20_Write_Byte(1,0xCC);
			DS18B20_Write_Byte(1, 0xBE);  //读值命令
			
			value1 = DS18B20_Read(1);  //先读低8位
			value2 = DS18B20_Read(1);  //读高8位
			value1 = (value2<<8) + value1;
			sum += value1&0x0fff;  //去掉符号位
		}
	}
	
	
	//转换成温度值
	if((value2&0xf0)==0xf0)
	{
		//取反加一
		sum = ~sum+1;
		temp = (double)sum*0.0625/3*(-1);
	}
	else
		temp = (double)sum*0.0625/3;
	
	UsartPrintf(USART_DEBUG, "Temp1:%.1f\r\n", temp);
	return temp;
}


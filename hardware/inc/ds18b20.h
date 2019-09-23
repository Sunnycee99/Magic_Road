#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f10x.h"

#define DS18_1_IN {GPIOB->CRL &= 0xfff0ffff;GPIOB->CRL |= 0x00040000;}  //¸¡¿ÕÊäÈë
#define DS18_1_OUT {GPIOB->CRL &= 0xfff0ffff;GPIOB->CRL |= 0x00030000;}  //ÍÆÍìÊä³ö
#define DS18_1_HIGH (GPIOB->BSRR |= 1<<4)
#define DS18_1_LOW (GPIOB->BRR |= 1<<4)
#define DS18_1_STATUS ((GPIOB->IDR>>4)&1)

#define DS18_2_IN {GPIOB->CRL &= 0xff0fffff;GPIOB->CRL |= 0x00400000;}  //¸¡¿ÕÊäÈë
#define DS18_2_OUT {GPIOB->CRL &= 0xff0fffff;GPIOB->CRL |= 0x00300000;}  //ÍÆÍìÊä³ö
#define DS18_2_HIGH (GPIOB->BSRR |= 1<<5)
#define DS18_2_LOW (GPIOB->BRR |= 1<<5)
#define DS18_2_STATUS ((GPIOB->IDR>>5)&1)

void DS18B20_Init(void);
void DS18B20_Write_Byte(u8 flag, u8 order);
u8 DS18B20_Read(u8 flag);
float DS18B20_GetValue(u8 flag);

#endif


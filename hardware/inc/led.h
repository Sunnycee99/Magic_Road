#ifndef __LED_H
#define __LED_H

#define LED1_1_ON (GPIOC->BRR |= 1<<7)
#define LED1_1_OFF (GPIOC->BSRR |= 1<<7)
#define LED1_2_ON (GPIOC->BRR |= 1<<8)
#define LED1_2_OFF (GPIOC->BSRR |= 1<<8)
#define LED1_3_ON (GPIOA->BRR |= 1<<12)
#define LED1_3_OFF (GPIOA->BSRR |= 1<<12)
#define LED1_1_STATUS (GPIOC->ODR>>7 & 1)
#define LED1_2_STATUS (GPIOC->ODR>>8 & 1)
#define LED1_3_STATUS (GPIOA->ODR>>12 & 1)

void LED_Init(void);
#endif


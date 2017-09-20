#ifndef __TIM_H__
#define __TIM_H__
	#include "stm32f10x_type.h"
	#include "stm32f10x_conf.h"
	#include "stm32f10x_map.h"
	#include "stm32f10x_nvic.h"
	#include "SYS.h"
	#include "USART.h"
	#include "GPIO.h"

	#define Timer_Switch(Timer,a)		if(a != Off)Timer->CR1 |= 0x01;else Timer->CR1 &= ~(1<<0);
	
	#define	G_1		RGB[0][0]
	#define	G_2		RGB[0][1]
	#define	G_3		RGB[0][2]
	#define	G_4		RGB[1][0]
	
	void Timer_Init(TIM_TypeDef* Timer,u16 arr,u16 psc);
	
#endif








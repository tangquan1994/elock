#ifndef __PWM_H__
#define __PWM_H__
	#include <stm32f10x_map.h>
	#include <stm32f10x_nvic.h>
	
	//channel的值从0到3分别表示CH1到CH4
	#define Channel_X(TIM,channel,arr)		*((u32 *)(&(TIM->CCR1))+channel) = arr 			//调整PWM占空比的宏定义

	void PWM_Init(TIM_TypeDef* Timer,u16 arr,u16 psc);
#endif

#ifndef __PWM_H__
#define __PWM_H__
	#include <stm32f10x_map.h>
	#include <stm32f10x_nvic.h>
	
	//channel��ֵ��0��3�ֱ��ʾCH1��CH4
	#define Channel_X(TIM,channel,arr)		*((u32 *)(&(TIM->CCR1))+channel) = arr 			//����PWMռ�ձȵĺ궨��

	void PWM_Init(TIM_TypeDef* Timer,u16 arr,u16 psc);
#endif

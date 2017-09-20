#ifndef __DELAY_H__
#define __DELAY_H__
	#include "stm32f10x_type.h"
	#include "stm32f10x_conf.h"
	#include "stm32f10x_map.h"
	#include "timer.h"
	
	#define DELAY_TIM	TIM2
	#define TIM_RELOAD	1000

	void Delay_Init(void);
	void delay_us(u32 nus);
	void delay_ms(u16 nms);
	void delay_s(u16 s);

	
#endif

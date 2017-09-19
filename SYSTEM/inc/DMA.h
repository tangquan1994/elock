#ifndef __DMA_H__
#define __DMA_H__
	#include "stm32f10x_type.h"
	#include "stm32f10x_conf.h"
	#include "stm32f10x_map.h"
	#include "stm32f10x_nvic.h"
	#include "sys.h"
	#include "usart.h"
	#include "delay.h"
	
	#define DMA_ENABLE		DMA1_Channel5->CCR |= 1<<0
	#define DMA_DIAABLE		DMA1_Channel5->CCR &= ~(1<<0)

	void DMA_Init(void);
#endif


#ifndef __ADC_H__
#define __ADC_H__
	#include <stm32f10x_map.h>
	#include  "GPIO.h"
  
	#define CH0					0
	#define CH1					1
	#define CH2					2
	#define CH3					3
	#define CH4					4
	#define CH5					5
	#define CH6					6
	#define CH7					7
	
	#define GET_POS()		GET_ADC(CH4)
	
	void ADC_Init(void);
	int GET_ADC(u8 ch);
#endif



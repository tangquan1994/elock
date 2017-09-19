#ifndef __BTN_H__
#define __BTN_H__
	#include "stm32f10x_type.h"
	#include "stm32f10x_conf.h"
	#include "stm32f10x_map.h"
	#include "stm32f10x_nvic.h"
	#include "SYS.h"
	#include "GPIO.h"
	#include "DELAY.h"
	#include "USART.h"


	#define LED0		PFout(6)
	#define LED1		PFout(7)
	#define LED2		PFout(8)
	#define LED3		PFout(9)

	#define KEY0		PAin(0)
	#define KEY1		PCin(13)
	#define KEY2		PAin(8)
	#define KEY3		PDin(3)

	void KEY_Init(void);
	void LED_Init(void);
	u8 KEY_Read(volatile unsigned long* data);
#endif

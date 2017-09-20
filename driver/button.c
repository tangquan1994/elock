#include "button.h"
void LED_Init(void)
{
	RCC_SW(APB2_GPIOF,On);
	iGPIO_Init(GPIOF,Pin_6|Pin_7|Pin_8|Pin_9,iGPIO_Mode_Out_PP,iGPIO_Speed_2MHz);
}


void KEY_Init(void)
{
	RCC_SW(APB2_GPIOA|APB2_GPIOC|APB2_GPIOD,On);
	iGPIO_Init(GPIOA,Pin_0|Pin_8,iGPIO_Mode_IPU,iGPIO_Input);
	iGPIO_Init(GPIOC,Pin_13,iGPIO_Mode_IPU,iGPIO_Input);
	iGPIO_Init(GPIOD,Pin_3,iGPIO_Mode_IPU,iGPIO_Input);
}

u8 KEY_Read(volatile unsigned long* data)
{
	u16 i = 10000;
	if(*data == 0)
	{
		while(i--);
		if(*data == 0)
		{
// 			while(*data == 0);
			return 1;
		}
	}
	return 0;
}


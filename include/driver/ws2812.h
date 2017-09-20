#ifndef __WS2812B_H__
#define __WS2812B_H__

	
	#include "SYS.h"
	#include "GPIO.h"
	#include "DELAY.h"
	#include "USART.h"
	
	#define WS2812_Pin	PAout(11)

	void T0H(void);

	void T0L(void);

	void T1H(void);

	void T1L(void);


	void WS2812_Init(void);
	void Ws2812bWriteRGB(u8 r,u8 g,u8 b);
	void WS2812b_WritePixel(unsigned int color);
	void Ws2812bReset(void);

#endif

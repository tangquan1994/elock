#include "ws2812.h"

void WS2812_Init(void)
{
	RCC_SW(APB2_GPIOA,On);
	iGPIO_Init(GPIOA,GPIO_Pin_11,iGPIO_Mode_Out_PP,iGPIO_Speed_50MHz);
	
}

void T0H(void)	//@33.1776MHz
{
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
}

void T0L(void)	//@33.1776MHz
{
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
}

void T1H(void)	//@33.1776MHz
{
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
}

void T1L(void)	//@33.1776MHz
{
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
	__asm("NOP");
}

void Ws2812bWrite8Bit(u8 r)
{
	if(r & 0x80) 
	{
		WS2812_Pin = 1;
		T1H();
		WS2812_Pin = 0; 
		T1L();
	}
	else 
	{
		WS2812_Pin = 1; 
		T0H(); 
		WS2812_Pin = 0; 
		T0L();
	}

	if(r & 0x40) {WS2812_Pin = 1; T1H(); WS2812_Pin = 0; T1L();}
	else {WS2812_Pin = 1; T0H(); WS2812_Pin = 0; T0L();}

	if(r & 0x20) {WS2812_Pin = 1; T1H(); WS2812_Pin = 0; T1L();}
	else {WS2812_Pin = 1; T0H(); WS2812_Pin = 0; T0L();}

	if(r & 0x10) {WS2812_Pin = 1; T1H(); WS2812_Pin = 0; T1L();}
	else {WS2812_Pin = 1; T0H(); WS2812_Pin = 0; T0L();}
	
	if(r & 0x08) {WS2812_Pin = 1; T1H(); WS2812_Pin = 0; T1L();}
	else {WS2812_Pin = 1; T0H(); WS2812_Pin = 0; T0L();}

	if(r & 0x04) {WS2812_Pin = 1; T1H(); WS2812_Pin = 0; T1L();}
	else {WS2812_Pin = 1; T0H(); WS2812_Pin = 0; T0L();}

	if(r & 0x02) {WS2812_Pin = 1; T1H(); WS2812_Pin = 0; T1L();}
	else {WS2812_Pin = 1; T0H(); WS2812_Pin = 0; T0L();}

	if(r & 0x01) {WS2812_Pin = 1; T1H(); WS2812_Pin = 0; T1L();}
	else {WS2812_Pin = 1; T0H(); WS2812_Pin = 0; T0L();}
}

void Ws2812bWriteRGB(u8 r,u8 g,u8 b)
{
// 	if(r == g && r == b)
// 	{
// 		if(r != 0)
// 		{
// 			if(r < 255)
// 				r ++;
// 			else
// 				r --;
// 		}
// 	}
	Ws2812bWrite8Bit(g);
	Ws2812bWrite8Bit(r);
	Ws2812bWrite8Bit(b);
}

void WS2812b_WritePixel(unsigned int color)
{
	u8 *p = (u8*)(&color);
	Ws2812bWrite8Bit(*(p+1));
	Ws2812bWrite8Bit(*(p+2));
	Ws2812bWrite8Bit(*(p+0));
//	printf("%.2X %.2X %.2X\r\n",*(p+1),*(p+2),*(p+0));
}

void Ws2812bReset(void)
{
	delay_us(50);
}


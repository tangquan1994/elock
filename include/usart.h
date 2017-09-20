#ifndef __USART_H__
#define __USART_H__
	#include "sys.h"

	void Uart_Init(USART_TypeDef *USART,u32 BaudRate,u8 SYSCLK);
	void UartSendByte(USART_TypeDef* uart,u8 data);
	u8 UartGetByte(USART_TypeDef* uart);
	u8 UartGetByte2(USART_TypeDef* uart,u32 timeout,u8 *data);
	u8 UartGetByte3(USART_TypeDef* uart,u8 *data);
	void USART2_Send_Byte(u8 data);	
	void UartSendStr(USART_TypeDef* uart,u8 *str);
	void UartSendBuff(USART_TypeDef* uart,u8 *buff,int len);
	u8 ReadUSART2Data(u8 *data);
#endif

#define _USART_GLOBAL_
#include "usart.h" 
#include "util_queue.h"

Queue USART2ReciQueue;

//BaudRate表示需要的波特率
//SYSCLK表示串口所在总线的频率
//USART1挂在APB2高速总线上,频率为72Mhz
//USART2、USART3等挂在APB1低速总线上,频率为36Mhz
void Uart_Init(USART_TypeDef *USART,u32 BaudRate,u8 SYSCLK)
{
	if(USART == USART1)
	{
		RCC->APB2ENR |= 1<<2;       //使能PORTA端口时钟
		RCC->APB2ENR |= 1<<14;		  //使能USART1的时钟
		GPIOA->CRH	 &= 0XFFFFF00F;	//清除PA9(TX)，PA10(RX)复位时的状态(浮空输入)
		GPIOA->CRH	 |= 0X000008B0;	//设置PA9(TX)复用推免输出,PA10(RX)上下拉输入

		RCC->APB2RSTR|=1<<14;	  		//复位串口1
		RCC->APB2RSTR&=~(1<<14);  	//停止复位
// 		iNVIC_Init(USART1_IRQChannel,2,2,Enable);
	}
	else if(USART == USART2)
	{
		RCC->APB2ENR |= 1<<2;       //使能PORTA端口时钟
		RCC->APB1ENR |= 1<<17;		  //使能USART2的时钟
		GPIOA->CRL	 &= 0XFFFF00FF;	//清除PA2(TX)，PA3(RX)复位时的状态(浮空输入)
		GPIOA->CRL	 |= 0X00008B00;	//设置PA2(TX)复用推免输出,PA3(RX)上下拉输入
		
		RCC->APB1RSTR|=1<<17;	  		//复位串口1
		RCC->APB1RSTR&=~(1<<17);  	//停止复位   	   
//  		iNVIC_Init(USART2_IRQChannel,0,0,Enable);
	}
	
	USART->BRR =	 (u16)((u32)(SYSCLK*1000000)/(u32)BaudRate);
	USART->CR1 |= (1<<13);	//使能串口 UE
	USART->CR1 &=~(1<<12);	//一个起始位，8个数据位，n个停止位
	USART->CR1 &=~(1<<11);	//空闲总线唤醒USART
	USART->CR1 &=~(1<<10);	//禁止校验，位9失去作用
	USART->CR1 &=~(1<<8);		//禁止中断
	USART->CR1 &=~(1<<7);		//禁止发送缓冲区中断
	USART->CR1 &=~(1<<6);		//禁止发送完成中断
	USART->CR1 |= (1<<5);		//允许接受缓冲区非空中断
	USART->CR1 &=~(1<<4);	  //禁止IDLE中断
	USART->CR1 |= (1<<3);		//使能发送
	USART->CR1 |= (1<<2);		//使能接受
	USART->CR1 &=~(1<<1);	  //USART处于正常模式，而非静默模式
	USART->CR1 &=~(1<<0);	  //禁止发送断开帧
}

void UartSendByte(USART_TypeDef* uart,u8 data)
{
	//统一先等待上一次发送完成,即发送缓冲区为空
	//如果不统一的话,函数A先等待上次发送完成在发送,而函数B则先发送在等待发送完成
	//那么如果函数A先执行,紧接着函数B在执行,那么函数B将要发送的数据或者数据集将会丢失第一个字节
	//因为函数B发送第一个字节时,上一次函数A的发送可能还没完成.
	while(!(uart->SR&(1<<6)));
	uart->DR = (u8)data;	
}

u8 UartGetByte(USART_TypeDef* uart)
{
	while(!(uart->SR&(1<<5)));
	return (u8)uart->DR;	//必须对DR寄存器进行读操作，不然就要手动将接收标志位清空
}

u8 UartGetByte2(USART_TypeDef* uart,u32 timeout,u8 *data)
{
	while(!(uart->SR&(1<<5)))
	{
		if(--timeout == 0)
			return 0;
		delay_ms(1);
	}
	*data = (u8)uart->DR;
	return 1;	
}

u8 UartGetByte3(USART_TypeDef* uart,u8 *data)
{
	if(uart->SR&(1<<5))
	{
		*data = (u8)uart->DR;
		return 1;	
	}
	return 0;	
}

void UartSendStr(USART_TypeDef* uart,u8 *str)
{
	while(*str != '\0')
	{
		UartSendByte(uart,*str++);
	}
}

void UartSendBuff(USART_TypeDef* uart,u8 *buff,int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		UartSendByte(uart,buff[i]);
// 		delay_us(100);
	}
}

/**
*
* USART1的中断处理函数
* 
*/
void USART1_IRQHandler(void)
{
	if(USART1->SR & (1<<5)) 	//SR寄存器(status register)中的RXNE标志位为1
	{
		USART1->SR &= ~(1<<5); 	//清除RXNE标志位
// 		UartSendByte(USART2,USART1->DR);
	}
}

/**
*
* USART2的中断处理函数
* 
*/
// bool IsUSART2DataReady = false;
// u8 USART2Data;
void USART2_IRQHandler(void)
{
	if(USART2->SR & (1<<5)) 	//SR寄存器(status register)中的RXNE标志位为1
	{
		USART2->SR &= ~(1<<5); 	//清除RXNE标志位
// 		UartSendByte(USART1,USART2->DR);
		QueuePut(&USART2ReciQueue,USART2->DR);
	}
}




#define _USART_GLOBAL_
#include "usart.h" 
#include "util_queue.h"

Queue USART2ReciQueue;

//BaudRate��ʾ��Ҫ�Ĳ�����
//SYSCLK��ʾ�����������ߵ�Ƶ��
//USART1����APB2����������,Ƶ��Ϊ72Mhz
//USART2��USART3�ȹ���APB1����������,Ƶ��Ϊ36Mhz
void Uart_Init(USART_TypeDef *USART,u32 BaudRate,u8 SYSCLK)
{
	if(USART == USART1)
	{
		RCC->APB2ENR |= 1<<2;       //ʹ��PORTA�˿�ʱ��
		RCC->APB2ENR |= 1<<14;		  //ʹ��USART1��ʱ��
		GPIOA->CRH	 &= 0XFFFFF00F;	//���PA9(TX)��PA10(RX)��λʱ��״̬(��������)
		GPIOA->CRH	 |= 0X000008B0;	//����PA9(TX)�����������,PA10(RX)����������

		RCC->APB2RSTR|=1<<14;	  		//��λ����1
		RCC->APB2RSTR&=~(1<<14);  	//ֹͣ��λ
// 		iNVIC_Init(USART1_IRQChannel,2,2,Enable);
	}
	else if(USART == USART2)
	{
		RCC->APB2ENR |= 1<<2;       //ʹ��PORTA�˿�ʱ��
		RCC->APB1ENR |= 1<<17;		  //ʹ��USART2��ʱ��
		GPIOA->CRL	 &= 0XFFFF00FF;	//���PA2(TX)��PA3(RX)��λʱ��״̬(��������)
		GPIOA->CRL	 |= 0X00008B00;	//����PA2(TX)�����������,PA3(RX)����������
		
		RCC->APB1RSTR|=1<<17;	  		//��λ����1
		RCC->APB1RSTR&=~(1<<17);  	//ֹͣ��λ   	   
//  		iNVIC_Init(USART2_IRQChannel,0,0,Enable);
	}
	
	USART->BRR =	 (u16)((u32)(SYSCLK*1000000)/(u32)BaudRate);
	USART->CR1 |= (1<<13);	//ʹ�ܴ��� UE
	USART->CR1 &=~(1<<12);	//һ����ʼλ��8������λ��n��ֹͣλ
	USART->CR1 &=~(1<<11);	//�������߻���USART
	USART->CR1 &=~(1<<10);	//��ֹУ�飬λ9ʧȥ����
	USART->CR1 &=~(1<<8);		//��ֹ�ж�
	USART->CR1 &=~(1<<7);		//��ֹ���ͻ������ж�
	USART->CR1 &=~(1<<6);		//��ֹ��������ж�
	USART->CR1 |= (1<<5);		//������ܻ������ǿ��ж�
	USART->CR1 &=~(1<<4);	  //��ֹIDLE�ж�
	USART->CR1 |= (1<<3);		//ʹ�ܷ���
	USART->CR1 |= (1<<2);		//ʹ�ܽ���
	USART->CR1 &=~(1<<1);	  //USART��������ģʽ�����Ǿ�Ĭģʽ
	USART->CR1 &=~(1<<0);	  //��ֹ���ͶϿ�֡
}

void UartSendByte(USART_TypeDef* uart,u8 data)
{
	//ͳһ�ȵȴ���һ�η������,�����ͻ�����Ϊ��
	//�����ͳһ�Ļ�,����A�ȵȴ��ϴη�������ڷ���,������B���ȷ����ڵȴ��������
	//��ô�������A��ִ��,�����ź���B��ִ��,��ô����B��Ҫ���͵����ݻ������ݼ����ᶪʧ��һ���ֽ�
	//��Ϊ����B���͵�һ���ֽ�ʱ,��һ�κ���A�ķ��Ϳ��ܻ�û���.
	while(!(uart->SR&(1<<6)));
	uart->DR = (u8)data;	
}

u8 UartGetByte(USART_TypeDef* uart)
{
	while(!(uart->SR&(1<<5)));
	return (u8)uart->DR;	//�����DR�Ĵ������ж���������Ȼ��Ҫ�ֶ������ձ�־λ���
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
* USART1���жϴ�����
* 
*/
void USART1_IRQHandler(void)
{
	if(USART1->SR & (1<<5)) 	//SR�Ĵ���(status register)�е�RXNE��־λΪ1
	{
		USART1->SR &= ~(1<<5); 	//���RXNE��־λ
// 		UartSendByte(USART2,USART1->DR);
	}
}

/**
*
* USART2���жϴ�����
* 
*/
// bool IsUSART2DataReady = false;
// u8 USART2Data;
void USART2_IRQHandler(void)
{
	if(USART2->SR & (1<<5)) 	//SR�Ĵ���(status register)�е�RXNE��־λΪ1
	{
		USART2->SR &= ~(1<<5); 	//���RXNE��־λ
// 		UartSendByte(USART1,USART2->DR);
		QueuePut(&USART2ReciQueue,USART2->DR);
	}
}




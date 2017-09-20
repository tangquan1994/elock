#include "DMA.h"

u8 	DMA_USART1_DATA[10] = {0,0,0,0,0,0,0,0,0,0};

//通道值为1~7
void DMA_Init(void)
{
	RCC->AHBENR |= 1<<0;
	RCC_SW(AHB_DMA1,On);		//开启DMA1的时钟
	DMA1_Channel5->CCR &= ~(1<<0);
	DMA1_Channel5->CCR &= ~(1<<14);		//非存储器到存储器模式
	DMA1_Channel5->CCR |= 3<<12;			//最高优先级
	DMA1_Channel5->CCR &= ~(3<<10);		//存储器8位数据宽度
	DMA1_Channel5->CCR &= ~(3<<8);		//外设8位数据宽度
	DMA1_Channel5->CCR |= 1<<7;				//开启存储器地址增一
	DMA1_Channel5->CCR &= ~(1<<6);		//不开启外设地址增一
	DMA1_Channel5->CCR |= 1<<5;				//开启循环模式
	DMA1_Channel5->CCR &= ~(1<<4);		//从外设读数据
	DMA1_Channel5->CCR &= ~(3<<2);		//禁止多余中断
	DMA1_Channel5->CCR |= 1<<1;				//开启传输完成中断
	
	DMA1_Channel5->CNDTR = 10;					//10字节的数据长度
	
	DMA1_Channel5->CPAR = (u32)(&USART1->DR);		//源地址
		
	DMA1_Channel5->CMAR = (u32)DMA_USART1_DATA;			//目标地址

	iNVIC_Init(DMA1_Channel5_IRQChannel,2,2,Enable);
	
// 	DMA1_Channel5->CCR |= 1<<0;				//开启传输
	DMA_DIAABLE;
}

//
void DMA1_Channel5_IRQHandler()
{
	//清除传输完成中断标志位
	DMA1->IFCR |= 1<<17;

}






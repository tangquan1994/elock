#include "DMA.h"

u8 	DMA_USART1_DATA[10] = {0,0,0,0,0,0,0,0,0,0};

//ͨ��ֵΪ1~7
void DMA_Init(void)
{
	RCC->AHBENR |= 1<<0;
	RCC_SW(AHB_DMA1,On);		//����DMA1��ʱ��
	DMA1_Channel5->CCR &= ~(1<<0);
	DMA1_Channel5->CCR &= ~(1<<14);		//�Ǵ洢�����洢��ģʽ
	DMA1_Channel5->CCR |= 3<<12;			//������ȼ�
	DMA1_Channel5->CCR &= ~(3<<10);		//�洢��8λ���ݿ��
	DMA1_Channel5->CCR &= ~(3<<8);		//����8λ���ݿ��
	DMA1_Channel5->CCR |= 1<<7;				//�����洢����ַ��һ
	DMA1_Channel5->CCR &= ~(1<<6);		//�����������ַ��һ
	DMA1_Channel5->CCR |= 1<<5;				//����ѭ��ģʽ
	DMA1_Channel5->CCR &= ~(1<<4);		//�����������
	DMA1_Channel5->CCR &= ~(3<<2);		//��ֹ�����ж�
	DMA1_Channel5->CCR |= 1<<1;				//������������ж�
	
	DMA1_Channel5->CNDTR = 10;					//10�ֽڵ����ݳ���
	
	DMA1_Channel5->CPAR = (u32)(&USART1->DR);		//Դ��ַ
		
	DMA1_Channel5->CMAR = (u32)DMA_USART1_DATA;			//Ŀ���ַ

	iNVIC_Init(DMA1_Channel5_IRQChannel,2,2,Enable);
	
// 	DMA1_Channel5->CCR |= 1<<0;				//��������
	DMA_DIAABLE;
}

//
void DMA1_Channel5_IRQHandler()
{
	//�����������жϱ�־λ
	DMA1->IFCR |= 1<<17;

}






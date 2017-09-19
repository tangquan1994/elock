#include "DELAY.h"

//�������ʹ��Systick��Ϊ��ʱ�Ķ�ʱ��,��̫�û���ѡ����ʹ����ͨ��ʱ������ʱ�ķ�ʽ

// static int CntPerUs;
// static int TickLoad;

// //hclk��ʾϵͳʱ��
// void Delay_Init(int hclk)
// {
// 	if(SysTick->CTRL & (1 << 2))
// 		CntPerUs = hclk;
// 	else
// 		CntPerUs = hclk / 8;
// 	TickLoad = SysTick->LOAD;
// }

// //ʹ����ͨ��ʱ��ʵ�־�ȷ��ʱ
// //����ʹ��systick���еȴ�ʵ�ֶ�ʱ
// //ʹ�ö�ʱ�����о�ȷ��ʱʱ��ò�Ҫ���жϴ��,�����������ɹ��Ƶĺ��
// void delay_us(u32 nus)
// {
// 	//����bug:
// 	//���������ʱ��SysTick->VAL��ֵ��С����˵��1����2
// 	//��ʱ����ִ�е�whileѭ��֮ǰ�ͻ����һ�����,��ô
// 	//�ͻᵼ���������һ��,������ʱ�������1ms!
// 	//���Կ����ڵ�һ�ε����֮ǰ���Ϲرն�ʱ�������,
// 	//��֮���ѭ�����ڴ򿪶�ʱ��.
// 	
// 	//����С��us����ʱ��Ч!!
// 	u32 temp = SysTick->VAL,i = (nus * CntPerUs),j;
// 	if(nus == 0)
// 		return;
// 	i -= temp;				//ȥ����ǰ�ļ�ʱֵ
// 	j = i / TickLoad + 1;	//����ȥ���˼�����β��,��һ����������
// 	i = TickLoad - i % TickLoad;	//i����
// 	//printf("%d %d\r\n",i,j);
// 	
// 	//ѭ��j�μ���Ϊ������
// 	while(j--)
// 	{
// 		while(!(SysTick->CTRL & (1<<16)));
// 	}
// 	//�����µĲ���һ�����ڵ�ʱ��ȴ�����
// 	while(SysTick->VAL > i);
// }


//hclk��ʾϵͳʱ��
void Delay_Init(void)
{
	//���ö�ʱ�����������Ϊ1us
	Timer_Init(DELAY_TIM,TIM_RELOAD,8);
}

void delay_us(u32 nus)
{
	int i,j;
	i = nus % TIM_RELOAD;
	j = nus / TIM_RELOAD;
		
	DELAY_TIM->CNT = TIM_RELOAD;
	DELAY_TIM->CR1 |= 0x01;	//��ʼ����
	while(j--)
	{
		while(!(DELAY_TIM->SR & 0x01));	//�ȴ�һ�ε���ʱ����
		DELAY_TIM->SR &= ~(1<<0);	//��ձ�־λ
	}
	if(i != 0)
	{
		DELAY_TIM->CNT = i;
		while(!(DELAY_TIM->SR & 0x01));
		DELAY_TIM->SR &= ~(1<<0);
	}
	DELAY_TIM->CR1 &= ~0x01;	//ֹͣ����
}

void delay_ms(u16 nms)
{
	DELAY_TIM->CNT = TIM_RELOAD;
	DELAY_TIM->CR1 |= 0x01;	//��ʼ����
	while(nms--)
	{
		while(!(DELAY_TIM->SR & 0x01));
		DELAY_TIM->SR &= ~(1<<0);
	}
	DELAY_TIM->CR1 &= ~0x01;	//ֹͣ����
} 

void delay_s(u16 s)
{
	if(s == 0)
		return;
	while(s--)
		delay_ms(1000);
}							    

// void delay_us(u32 nus)
// {		
// 	int i;
// 	while(nus--)
// 		for(i=0;i<5;i++);
// }

// void delay_ms(u16 nms)
// {	
// 	while(nms--)
// 		delay_us(1000);
// }

// void delay_s(u16 ns)
// {	
// 	while(ns--)
// 		delay_ms(1000);
// }

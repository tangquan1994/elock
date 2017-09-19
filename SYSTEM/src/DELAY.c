#include "DELAY.h"

//下面的是使用Systick作为延时的定时器,不太好还是选用了使用普通定时器来延时的方式

// static int CntPerUs;
// static int TickLoad;

// //hclk表示系统时钟
// void Delay_Init(int hclk)
// {
// 	if(SysTick->CTRL & (1 << 2))
// 		CntPerUs = hclk;
// 	else
// 		CntPerUs = hclk / 8;
// 	TickLoad = SysTick->LOAD;
// }

// //使用普通定时器实现精确定时
// //或者使用systick进行等待实现定时
// //使用定时器进行精确定时时最好不要被中断打断,否则会产生不可估计的后果
// void delay_us(u32 nus)
// {
// 	//还有bug:
// 	//如果正好这时候SysTick->VAL的值很小比如说是1或者2
// 	//这时候在执行到while循环之前就会产生一次溢出,那么
// 	//就会导致溢出少算一次,导致延时精度误差1ms!
// 	//可以考虑在第一次的溢出之前加上关闭定时器的语句,
// 	//在之后的循环中在打开定时器.
// 	
// 	//对于小的us级延时无效!!
// 	u32 temp = SysTick->VAL,i = (nus * CntPerUs),j;
// 	if(nus == 0)
// 		return;
// 	i -= temp;				//去掉当前的计时值
// 	j = i / TickLoad + 1;	//由于去掉了计数的尾部,加一个计数周期
// 	i = TickLoad - i % TickLoad;	//i等于
// 	//printf("%d %d\r\n",i,j);
// 	
// 	//循环j次计数为零周期
// 	while(j--)
// 	{
// 		while(!(SysTick->CTRL & (1<<16)));
// 	}
// 	//将余下的不满一个周期的时间等待计数
// 	while(SysTick->VAL > i);
// }


//hclk表示系统时钟
void Delay_Init(void)
{
	//设置定时器的溢出周期为1us
	Timer_Init(DELAY_TIM,TIM_RELOAD,8);
}

void delay_us(u32 nus)
{
	int i,j;
	i = nus % TIM_RELOAD;
	j = nus / TIM_RELOAD;
		
	DELAY_TIM->CNT = TIM_RELOAD;
	DELAY_TIM->CR1 |= 0x01;	//开始计数
	while(j--)
	{
		while(!(DELAY_TIM->SR & 0x01));	//等待一次倒计时结束
		DELAY_TIM->SR &= ~(1<<0);	//清空标志位
	}
	if(i != 0)
	{
		DELAY_TIM->CNT = i;
		while(!(DELAY_TIM->SR & 0x01));
		DELAY_TIM->SR &= ~(1<<0);
	}
	DELAY_TIM->CR1 &= ~0x01;	//停止计数
}

void delay_ms(u16 nms)
{
	DELAY_TIM->CNT = TIM_RELOAD;
	DELAY_TIM->CR1 |= 0x01;	//开始计数
	while(nms--)
	{
		while(!(DELAY_TIM->SR & 0x01));
		DELAY_TIM->SR &= ~(1<<0);
	}
	DELAY_TIM->CR1 &= ~0x01;	//停止计数
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

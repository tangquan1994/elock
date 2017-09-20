#include  "TIM.h"

//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
void Timer_Init(TIM_TypeDef* Timer,u16 arr,u16 psc)
{
	psc -= 1;			//因为 CK_CNT=fCK_PSC/(PSC[15:0]+1);
	arr -= 1;			//因为一直向下计数到0,arr次计数的话ARR需赋值arr-1
	RCC_SW(APB1_TIM2,On);	//打开定时器3/4的时钟
	Timer->ARR 		= 	arr;      		//设定计数器自动重装值
	Timer->CR1 		|=	1<<4;    			//向下计数
	Timer->PSC 		= 	psc;      		//预分频器
	Timer->DIER		|=	1<<0;     		//允许更新中断	  
	Timer_Switch(Timer,Off);				//关闭定时器
// 	iNVIC_Init(TIM3_IRQChannel,2,2,Enable);	//作为PID运算中断源
}

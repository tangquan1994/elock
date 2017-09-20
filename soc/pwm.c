#include "PWM.h"

void PWM_Init(TIM_TypeDef* Timer,u16 arr,u16 psc)
{
	psc -= 1;			//因为 CK_CNT=fCK_PSC/(PSC[15:0]+1);
	arr -= 1;			//因为一直向下计数到0,arr次计数的话ARR需赋值arr-1

	RCC->APB1ENR |= 1<<0;     	//TIM2时钟使能    
// 	RCC->APB1ENR |= 1<<1;     	//TIM3时钟使能    
	RCC->APB2ENR |= 1<<2;     	//使能PORTA时钟	   	 
// 	RCC->APB2ENR |= 1<<3;     	//使能PORTB时钟	   	 
	
	GPIOA->CRL &= 0xffff0000; 	//PA0 PA1 PA2 PA3  配置为复用功能输出 	
	GPIOA->CRL |= 0x0000bbbb;

// 	GPIOB->CRL &= 0xffffff00; 	//PB0 PB1  配置为复用功能输出 	
// 	GPIOB->CRL |= 0x000000bb;

	Timer->ARR = arr;						//设定计数器自动重装值 
	Timer->PSC = psc;						//预分频器不分频

//不用channel 1 和 channel 2

	//组别1 channel 1 和 channel 2
	Timer->CCMR1|=6<<4;  	//CH1 PWM1模式		
	Timer->CCMR1|=1<<3; 		//CH1 预装载使能	 
	Timer->CCMR1|=6<<12;  	//CH2 PWM1模式		 
	Timer->CCMR1|=1<<11; 	//CH2 预装载使能	 
	Timer->CCER|=1<<4;   	//OC2 输出使能	   
	Timer->CCER|=1<<0;   	//OC1 输出使能

	//组别2 channel 3 和 channel 4
	Timer->CCMR2|=6<<4;  	//CH3 PWM1模式		
	Timer->CCMR2|=1<<3; 		//CH3 预装载使能	  
	Timer->CCMR2|=6<<12;  	//CH4 PWM1模式		 
	Timer->CCMR2|=1<<11; 	//CH4 预装载使能	 
	Timer->CCER|=1<<12;   	//OC4 输出使能	   
	Timer->CCER|=1<<8;   	//OC3 输出使能  
	Timer->CR1=0x0080;   	//ARPE使能 
	Timer->CR1|=0x01;    	//使能定时器4 											  
} 




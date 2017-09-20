#include "PWM.h"

void PWM_Init(TIM_TypeDef* Timer,u16 arr,u16 psc)
{
	psc -= 1;			//��Ϊ CK_CNT=fCK_PSC/(PSC[15:0]+1);
	arr -= 1;			//��Ϊһֱ���¼�����0,arr�μ����Ļ�ARR�踳ֵarr-1

	RCC->APB1ENR |= 1<<0;     	//TIM2ʱ��ʹ��    
// 	RCC->APB1ENR |= 1<<1;     	//TIM3ʱ��ʹ��    
	RCC->APB2ENR |= 1<<2;     	//ʹ��PORTAʱ��	   	 
// 	RCC->APB2ENR |= 1<<3;     	//ʹ��PORTBʱ��	   	 
	
	GPIOA->CRL &= 0xffff0000; 	//PA0 PA1 PA2 PA3  ����Ϊ���ù������ 	
	GPIOA->CRL |= 0x0000bbbb;

// 	GPIOB->CRL &= 0xffffff00; 	//PB0 PB1  ����Ϊ���ù������ 	
// 	GPIOB->CRL |= 0x000000bb;

	Timer->ARR = arr;						//�趨�������Զ���װֵ 
	Timer->PSC = psc;						//Ԥ��Ƶ������Ƶ

//����channel 1 �� channel 2

	//���1 channel 1 �� channel 2
	Timer->CCMR1|=6<<4;  	//CH1 PWM1ģʽ		
	Timer->CCMR1|=1<<3; 		//CH1 Ԥװ��ʹ��	 
	Timer->CCMR1|=6<<12;  	//CH2 PWM1ģʽ		 
	Timer->CCMR1|=1<<11; 	//CH2 Ԥװ��ʹ��	 
	Timer->CCER|=1<<4;   	//OC2 ���ʹ��	   
	Timer->CCER|=1<<0;   	//OC1 ���ʹ��

	//���2 channel 3 �� channel 4
	Timer->CCMR2|=6<<4;  	//CH3 PWM1ģʽ		
	Timer->CCMR2|=1<<3; 		//CH3 Ԥװ��ʹ��	  
	Timer->CCMR2|=6<<12;  	//CH4 PWM1ģʽ		 
	Timer->CCMR2|=1<<11; 	//CH4 Ԥװ��ʹ��	 
	Timer->CCER|=1<<12;   	//OC4 ���ʹ��	   
	Timer->CCER|=1<<8;   	//OC3 ���ʹ��  
	Timer->CR1=0x0080;   	//ARPEʹ�� 
	Timer->CR1|=0x01;    	//ʹ�ܶ�ʱ��4 											  
} 




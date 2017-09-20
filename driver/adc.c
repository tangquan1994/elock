#include "ADC.h"

void ADC_Init(void)
{
	RCC->APB2ENR |= 1<<2;		//使能PORTA口时钟 
	RCC->APB2ENR |= 1<<9;		//使能ADC1是时钟	
	RCC->APB2RSTR |= 1<<9;		//ADC1复位
	RCC->APB2RSTR &= ~(1<<9);	//ADC1复位结束
	//SYSCLK/DIV2=12M ADC时钟设置为12M,ADC最大时钟不能超过14M!
	//否则将导致ADC准确度下降! 
	RCC->CFGR &= ~(3<<14);		//分频因子清零
	RCC->CFGR |= 2<<14;			//6分频	 
	
	iGPIO_Init(GPIOA,GPIO_Pin_0,iGPIO_Mode_AIN,iGPIO_Input);
	
	ADC1->CR1 &= 0XF0FFFF;   	//工作模式清零 16~19位
	ADC1->CR1 |= 0<<16;      	//独立工作模式  
	ADC1->CR1 &= ~(1<<8);		//非扫描模式	  
	ADC1->CR2 &= ~(1<<1);		//单次转换模式
	ADC1->CR2 &= ~(7<<17);	   
	ADC1->CR2 |= 7<<17;			//软件控制转换  
	ADC1->CR2 |= 1<<20;			//使用用外部触发(SWSTART)!!!	必须使用一个事件来触发
	ADC1->CR2 &= ~(1<<11);		//右对齐	 
	
	ADC1->SQR1 &= ~(0XF<<20);
	ADC1->SQR1 |= 0<<20;	
	//设置通道0的采样时间
	ADC1->SMPR2 &= ~(7<<0);		//通道0采样时间清空	  
 	ADC1->SMPR2 |= 7<<0;			//通道0  239.5周期,提高采样时间可以提高精确度	 

	ADC1->CR2 |= 1<<0;	   	//开启AD转换器	 
	ADC1->CR2 |= 1<<3;       	//使能复位校准  
	while(ADC1->CR2&1<<3); 		//等待校准结束 			 
    //该位由软件设置并由硬件清除。在校准寄存器被初始化后该位将被清除。 		 
	ADC1->CR2 |= 1<<2;        	//开启AD校准	   
	while(ADC1->CR2&1<<2);		//等待校准结束
	//该位由软件设置以开始校准，并在校准结束时由硬件清除  
}

int GET_ADC(u8 ch)
{
	//设置转换序列	
	//规则转化组一共16个 这16个可以按要求转换 每一个可以设定一个通道 如channel_1 
	ADC1->SQR3 &= 0XFFFFFFE0;		//规则序列1 通道ch
	ADC1->SQR3 |= ch;		  			    
	ADC1->CR2 |= 1<<22;       	//启动规则转换通道 
	while(!(ADC1->SR&1<<1));	//等待转换结束	 	   
	return (int)(ADC1->DR);				//返回adc值	
}






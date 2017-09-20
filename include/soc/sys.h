#ifndef __INI_H__
#define __INI_H__
	#include "stm32f10x_type.h"
	#include "stm32f10x_conf.h"
	#include "stm32f10x_map.h"
	#include "stm32f10x_nvic.h"
	#include "usart.h"
	#include "gpio.h"
	#include "delay.h"
	#include "stdio.h"

	#define SYS_PORT			USART1			//系统使用的串口
	#define DEBUG											//使用调试
	
	/******************方式定义*********************/
	#define On						1		//使能
	#define Off						0		//失能
	#define FTIR   				1  	//下降沿触发
	#define RTIR   				2  	//上升沿触发
	#define Enable				1		//使能
	#define Disable				0		//失能

	
	/***************JTAG模式设置定义****************/
	#define JTAG_SWD_DISABLE   				0X02
	#define SWD_ENABLE         				0X01
	#define JTAG_SWD_ENABLE    				0X00	
	
	/*******************外部中断********************/
	#define ENABLE_INT(n)							EXTI->IMR |= (u32)(1<<n)						//使能某一个外部中断(不屏蔽)
	#define DISABLE_INT(n)						EXTI->IMR &= ~((u32)(1<<n))					//失能某一个外部中断(不屏蔽)	
	#define EXTI_SWIER(n)							{EXTI->SWIER |= (1<<n);EXTI->PR |= 1<<n;}				//对外部中断线n产生一个软件中断
	//这个用于在某一个不可被打断的处理中将中断关闭之后在处理完成之后又将中断打开,如果在不可被打断任务时
	//发生一次中断,这时中断会将PR寄存器相应位置位,但是不会产生中断响应,在又一次打开中断时,
	//也不会回头处理遗漏的中断信号,故采用软件中断的方式.软件中断参考STM32中文参考手册.
	#define SWIER_ON_PEND(n)					{EXTI->SWIER |= (EXTI->PR&(1<<n));EXTI->PR |= 1<<n;}			//根据PR寄存器对外部中断线n产生一个软件中断
	
	
	/*=============================================*/
	/*********************时钟**********************/	
	/*=============================================*/
	//定义指向RCC的静态指针变量
	static RCC_TypeDef* RP = RCC;
	//AHB总线的外设,最高四位为5,表示在RCC结构体内部的偏移为5个字(每个字4个字节)
	#define AHB_DMA1									((u32)0x50000001)
	#define AHB_DMA2									((u32)0x50000002)
	#define AHB_SRAM									((u32)0x50000004)
	#define AHB_FLITF									((u32)0x50000010)
	#define AHB_CRC										((u32)0x50000040)
	#define AHB_FSMC									((u32)0x50000100)
	#define AHB_SDIO									((u32)0x50000400)
	//APB2总线的外设,最高四位为6,表示在RCC结构体内部的偏移为6个字(每个字4个字节)
	#define	APB2_AFIO                 ((u32)0x60000001)
	#define	APB2_GPIOA             		((u32)0x60000004)
	#define	APB2_GPIOB             		((u32)0x60000008)
	#define	APB2_GPIOC               	((u32)0x60000010)
	#define	APB2_GPIOD            		((u32)0x60000020)
	#define	APB2_GPIOE             		((u32)0x60000040)
	#define	APB2_GPIOF             		((u32)0x60000080)
	#define	APB2_GPIOG             		((u32)0x60000100)
	#define	APB2_ADC1                 ((u32)0x60000200)
	#define	APB2_ADC2                 ((u32)0x60000400)
	#define	APB2_TIM1                 ((u32)0x60000800)
	#define	APB2_SPI1                 ((u32)0x60001000)
	#define	APB2_TIM8             		((u32)0x60004000)
	//APB2总线的外设,最高四位为7,表示在RCC结构体内部的偏移为7个字(每个字4个字节)
	#define APB1_TIM2									((u32)0x70000001)
	#define APB1_TIM3									((u32)0x70000002)
	#define APB1_TIM4									((u32)0x70000004)
	#define APB1_TIM5									((u32)0x70000008)
	#define APB1_TIM6									((u32)0x70000010)
	#define APB1_TIM7									((u32)0x70000020)
	#define APB1_WWDG									((u32)0x70000800)
	#define APB1_SPI2									((u32)0x70004000)
	#define APB1_SPI3									((u32)0x70008000)
	#define APB1_USART2								((u32)0x70020000)
	#define APB1_USART3								((u32)0x70040000)
		
	void MCU_INIT(u8 PLL);
	void JTAG_Set(u8 mode);
	void RCC_SW(u32 Peripheral,u8 ON_OFF);
	void iEXTI_Init(GPIO_TypeDef* PORT,u8 Line,u8 Trigger,u8 Cmd);
	void iNVIC_Init(u8 NVIC_IRQChannel,u8 NVIC_IRQChannelPreemptionPriority,u8 NVIC_IRQChannelSubPriority,u8 NVIC_IRQChannelCmd);
	void SysTick_Init(void);
#endif


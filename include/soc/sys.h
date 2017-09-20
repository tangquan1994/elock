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

	#define SYS_PORT			USART1			//ϵͳʹ�õĴ���
	#define DEBUG											//ʹ�õ���
	
	/******************��ʽ����*********************/
	#define On						1		//ʹ��
	#define Off						0		//ʧ��
	#define FTIR   				1  	//�½��ش���
	#define RTIR   				2  	//�����ش���
	#define Enable				1		//ʹ��
	#define Disable				0		//ʧ��

	
	/***************JTAGģʽ���ö���****************/
	#define JTAG_SWD_DISABLE   				0X02
	#define SWD_ENABLE         				0X01
	#define JTAG_SWD_ENABLE    				0X00	
	
	/*******************�ⲿ�ж�********************/
	#define ENABLE_INT(n)							EXTI->IMR |= (u32)(1<<n)						//ʹ��ĳһ���ⲿ�ж�(������)
	#define DISABLE_INT(n)						EXTI->IMR &= ~((u32)(1<<n))					//ʧ��ĳһ���ⲿ�ж�(������)	
	#define EXTI_SWIER(n)							{EXTI->SWIER |= (1<<n);EXTI->PR |= 1<<n;}				//���ⲿ�ж���n����һ������ж�
	//���������ĳһ�����ɱ���ϵĴ����н��жϹر�֮���ڴ������֮���ֽ��жϴ�,����ڲ��ɱ��������ʱ
	//����һ���ж�,��ʱ�жϻὫPR�Ĵ�����Ӧλ��λ,���ǲ�������ж���Ӧ,����һ�δ��ж�ʱ,
	//Ҳ�����ͷ������©���ж��ź�,�ʲ�������жϵķ�ʽ.����жϲο�STM32���Ĳο��ֲ�.
	#define SWIER_ON_PEND(n)					{EXTI->SWIER |= (EXTI->PR&(1<<n));EXTI->PR |= 1<<n;}			//����PR�Ĵ������ⲿ�ж���n����һ������ж�
	
	
	/*=============================================*/
	/*********************ʱ��**********************/	
	/*=============================================*/
	//����ָ��RCC�ľ�ָ̬�����
	static RCC_TypeDef* RP = RCC;
	//AHB���ߵ�����,�����λΪ5,��ʾ��RCC�ṹ���ڲ���ƫ��Ϊ5����(ÿ����4���ֽ�)
	#define AHB_DMA1									((u32)0x50000001)
	#define AHB_DMA2									((u32)0x50000002)
	#define AHB_SRAM									((u32)0x50000004)
	#define AHB_FLITF									((u32)0x50000010)
	#define AHB_CRC										((u32)0x50000040)
	#define AHB_FSMC									((u32)0x50000100)
	#define AHB_SDIO									((u32)0x50000400)
	//APB2���ߵ�����,�����λΪ6,��ʾ��RCC�ṹ���ڲ���ƫ��Ϊ6����(ÿ����4���ֽ�)
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
	//APB2���ߵ�����,�����λΪ7,��ʾ��RCC�ṹ���ڲ���ƫ��Ϊ7����(ÿ����4���ֽ�)
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


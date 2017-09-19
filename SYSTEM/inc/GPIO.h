#ifndef __GPIO_H__
#define __GPIO_H__
	#include "stm32f10x_type.h"
	#include "stm32f10x_conf.h"
	#include "stm32f10x_map.h"
	#include "stm32f10x_nvic.h"
	#include "SYS.h"
	
	//---------------------------------------
	//-------------��Ȩ ��Ȩ����-------------
	//---------------------------------------
	
	//��������IO�ڲ�����������ǧ��
	//λ������IO�ھ��и�����ٶ��Լ������ǿ�˳�����ֲ�ı�����,���Խ�51����ֱ����ֲ����
	//�����ؽ��ж����޸�,ֻ�趨�弸��IO�ڵĺ궨���Լ�IO�ڵĳ�ʼ������.
	//λ�������Ե���IO�ڲ���������,��Ϊλ������ֻ����һ�μ���һ�θ�ֵһ�α�������ָ��,
	//������λ�������Ļ�����Ҫ�����һ��������.���ǵ��Զ��IO�ڽ��в���ʱ,�Ĵ�����������
	//��������,��Ϊ���ܶԶ���IO�ڲ�����ִֻ��һ�μ��ء�һ�θ�ֵ��һ�λ��һ�α�������


	//********************************************************************
	//**************************λ����������IO��**************************
	//********************************************************************
	//�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
	//������궨�帳ֵ�����λ��0�Ļ�������Ӧλ�����������,��֮������λ��1�Ļ�������Ӧλ��һ
	//�������"PAout(0) = 0xFE",��Ȼ0xFE����0,��������0xFE�����λ��0,λ������Ȼ�ὫGPIOA_Pin_0
	//��������,ֻ�е���ֵ���λΪ1ʱ�ŻὫ��Ӧ������һ.
	//ע:������ú궨���������д��������������Զ�������Ĺ�ʽ�����������ҪCPU������ʱ����λ��ַת����ʽ
	//�������������øú�ʱ������Ǳ���,��ô���������Ŀɱ��Ա�����������ʵ�ּ���ý��,ֻ��������ʱ����
	#define BIT_ADDR(addr, n)			*((volatile unsigned long*)(0x42000000 + ((addr-0x40000000)<<5) + (n<<2)))	//�ֽڵ�ַתλ��ַ��ʽ
	//IO�ڵ�ַӳ��
	#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
	#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
	#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
	#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
	#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
	#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
	#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

	#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
	#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
	#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
	#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
	#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
	#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
	#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
	 
	#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
	#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

	#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
	#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

	#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
	#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

	#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
	#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

	#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
	#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

	#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
	#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

	#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
	#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����

	//********************************************************************
	//*************************�Ĵ�����������IO��*************************
	//********************************************************************
	//general purpose I/O OUTPUT
	#define 	GPO(PORT,PIN,VAL)					if((VAL)!= 0) PORT->BSRR |= PIN;else PORT->BRR |= PIN;	//����ٶ���,��Ϊ��һ���ж�,ִ��Ч�ʵ�
	#define 	SETB(PORT,PIN)						PORT->BSRR |= PIN		//�������һ��ִ��Ч�ʸ�
	#define 	CLRB(PORT,PIN)						PORT->BRR |= PIN
	//general purpose I/O INPUT
	#define 	GPI(PORT,PIN)							(u16)(PORT->IDR & PIN)
	
	//====================================================================
	//***************************GPIO���ź궨��***************************
	//====================================================================
	//ͬʱ���弸�����ſ�������:GPIO_Pin_0|GPIO_Pin_3|GPIO_Pin_7
	//ȫ��������ȥ��һ�������ſ�������:GPIO_Pin_All&(~GPIO_Pin_3)&(&GPIO_Pin_8)
	#define GPIO_Pin_0                 ((u16)0x0001)  /*!< Pin 0 selected */
	#define GPIO_Pin_1                 ((u16)0x0002)  /*!< Pin 1 selected */
	#define GPIO_Pin_2                 ((u16)0x0004)  /*!< Pin 2 selected */
	#define GPIO_Pin_3                 ((u16)0x0008)  /*!< Pin 3 selected */
	#define GPIO_Pin_4                 ((u16)0x0010)  /*!< Pin 4 selected */
	#define GPIO_Pin_5                 ((u16)0x0020)  /*!< Pin 5 selected */
	#define GPIO_Pin_6                 ((u16)0x0040)  /*!< Pin 6 selected */
	#define GPIO_Pin_7                 ((u16)0x0080)  /*!< Pin 7 selected */
	#define GPIO_Pin_8                 ((u16)0x0100)  /*!< Pin 8 selected */
	#define GPIO_Pin_9                 ((u16)0x0200)  /*!< Pin 9 selected */
	#define GPIO_Pin_10                ((u16)0x0400)  /*!< Pin 10 selected */
	#define GPIO_Pin_11                ((u16)0x0800)  /*!< Pin 11 selected */
	#define GPIO_Pin_12                ((u16)0x1000)  /*!< Pin 12 selected */
	#define GPIO_Pin_13                ((u16)0x2000)  /*!< Pin 13 selected */
	#define GPIO_Pin_14                ((u16)0x4000)  /*!< Pin 14 selected */
	#define GPIO_Pin_15                ((u16)0x8000)  /*!< Pin 15 selected */
	#define GPIO_Pin_HIGH          	   ((u16)0xFF00)  /*!< All pins selected */
	#define GPIO_Pin_LOW	           	 ((u16)0x00FF)  /*!< All pins selected */
	#define GPIO_Pin_All               ((u16)0xFFFF)  /*!< All pins selected */

	#define Pin_0                 ((u16)0x0001)  /*!< Pin 0 selected */
	#define Pin_1                 ((u16)0x0002)  /*!< Pin 1 selected */
	#define Pin_2                 ((u16)0x0004)  /*!< Pin 2 selected */
	#define Pin_3                 ((u16)0x0008)  /*!< Pin 3 selected */
	#define Pin_4                 ((u16)0x0010)  /*!< Pin 4 selected */
	#define Pin_5                 ((u16)0x0020)  /*!< Pin 5 selected */
	#define Pin_6                 ((u16)0x0040)  /*!< Pin 6 selected */
	#define Pin_7                 ((u16)0x0080)  /*!< Pin 7 selected */
	#define Pin_8                 ((u16)0x0100)  /*!< Pin 8 selected */
	#define Pin_9                 ((u16)0x0200)  /*!< Pin 9 selected */
	#define Pin_10                ((u16)0x0400)  /*!< Pin 10 selected */
	#define Pin_11                ((u16)0x0800)  /*!< Pin 11 selected */
	#define Pin_12                ((u16)0x1000)  /*!< Pin 12 selected */
	#define Pin_13                ((u16)0x2000)  /*!< Pin 13 selected */
	#define Pin_14                ((u16)0x4000)  /*!< Pin 14 selected */
	#define Pin_15                ((u16)0x8000)  /*!< Pin 15 selected */
	#define Pin_HIGH          	 	((u16)0xFF00)  /*!< All pins selected */
	#define Pin_LOW	           	 	((u16)0x00FF)  /*!< All pins selected */
	#define Pin_All               ((u16)0xFFFF)  /*!< All pins selected */

	//====================================================================
	//***********************GPIO����������ú궨��***********************
	//====================================================================
	//GPIO����������ú궨��
	#define iGPIO_Mode_AIN							0x00
	#define iGPIO_Mode_IN_FLOATING 			0x04
	#define iGPIO_Mode_IPD 							0x08
	#define iGPIO_Mode_IPU 							0x08
	
	#define iGPIO_Mode_Out_PP						0x00
	#define iGPIO_Mode_Out_OD						0x04
	#define iGPIO_Mode_AF_PP						0x08
	#define iGPIO_Mode_AF_OD						0x0C	
	
	//GPIO����ٶȿ���
	#define iGPIO_Input									0x00
  #define iGPIO_Speed_10MHz						0x01
  #define iGPIO_Speed_2MHz						0x02
  #define iGPIO_Speed_50MHz						0x03
	
	void iGPIO_Init(GPIO_TypeDef* PORT,u16 PINS,u8 MODE,u8 Frequency);
	
#endif








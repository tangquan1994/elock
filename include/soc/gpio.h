#ifndef __GPIO_H__
#define __GPIO_H__
	#include "stm32f10x_type.h"
	#include "stm32f10x_conf.h"
	#include "stm32f10x_map.h"
	#include "stm32f10x_nvic.h"
	#include "SYS.h"
	
	//---------------------------------------
	//-------------汤权 版权所有-------------
	//---------------------------------------
	
	//下面两个IO口操作函数各有千秋
	//位带操作IO口具有更快的速度以及大大增强了程序移植的便利性,可以将51程序直接移植过来
	//而不必进行额外修改,只需定义几个IO口的宏定义以及IO口的初始化即可.
	//位带操作对单个IO口操作有优势,因为位带操作只进行一次加载一次赋值一次保存运算指令,
	//而不用位带操作的话则需要额外的一个或运算.但是当对多个IO口进行操作时,寄存器法就有明
	//显优势了,因为不管对多少IO口操作都只执行一次加载、一次赋值、一次或和一次保存运算


	//********************************************************************
	//**************************位带操作控制IO口**************************
	//********************************************************************
	//参考<<CM3权威指南>>第五章(87页~92页).
	//如果给宏定义赋值的最低位是0的话将对相应位进行清零操作,反之如果最低位是1的话将对相应位置一
	//例如操作"PAout(0) = 0xFE",虽然0xFE大于0,但是由于0xFE的最低位是0,位操作仍然会将GPIOA_Pin_0
	//引脚清零,只有当赋值最低位为1时才会将相应引脚置一.
	//注:如果是用宏定义或者向宏中传常量编译器会自动将下面的公式算出来而不需要CPU在运行时计算位地址转换公式
	//但是如果程序调用该宏时传入的是变量,那么忧郁变量的可变性编译器不可以实现计算好结果,只能在运行时计算
	#define BIT_ADDR(addr, n)			*((volatile unsigned long*)(0x42000000 + ((addr-0x40000000)<<5) + (n<<2)))	//字节地址转位地址公式
	//IO口地址映射
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
	 
	#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
	#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

	#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
	#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

	#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
	#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

	#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
	#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

	#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
	#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

	#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
	#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

	#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
	#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

	//********************************************************************
	//*************************寄存器操作控制IO口*************************
	//********************************************************************
	//general purpose I/O OUTPUT
	#define 	GPO(PORT,PIN,VAL)					if((VAL)!= 0) PORT->BSRR |= PIN;else PORT->BRR |= PIN;	//这个速度慢,因为有一个判断,执行效率低
	#define 	SETB(PORT,PIN)						PORT->BSRR |= PIN		//这个下面一个执行效率高
	#define 	CLRB(PORT,PIN)						PORT->BRR |= PIN
	//general purpose I/O INPUT
	#define 	GPI(PORT,PIN)							(u16)(PORT->IDR & PIN)
	
	//====================================================================
	//***************************GPIO引脚宏定义***************************
	//====================================================================
	//同时定义几个引脚可以这样:GPIO_Pin_0|GPIO_Pin_3|GPIO_Pin_7
	//全部引脚中去掉一两个引脚可以这样:GPIO_Pin_All&(~GPIO_Pin_3)&(&GPIO_Pin_8)
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
	//***********************GPIO输入输出配置宏定义***********************
	//====================================================================
	//GPIO输入输出配置宏定义
	#define iGPIO_Mode_AIN							0x00
	#define iGPIO_Mode_IN_FLOATING 			0x04
	#define iGPIO_Mode_IPD 							0x08
	#define iGPIO_Mode_IPU 							0x08
	
	#define iGPIO_Mode_Out_PP						0x00
	#define iGPIO_Mode_Out_OD						0x04
	#define iGPIO_Mode_AF_PP						0x08
	#define iGPIO_Mode_AF_OD						0x0C	
	
	//GPIO输出速度控制
	#define iGPIO_Input									0x00
  #define iGPIO_Speed_10MHz						0x01
  #define iGPIO_Speed_2MHz						0x02
  #define iGPIO_Speed_50MHz						0x03
	
	void iGPIO_Init(GPIO_TypeDef* PORT,u16 PINS,u8 MODE,u8 Frequency);
	
#endif








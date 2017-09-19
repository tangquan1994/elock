#ifndef ___IIC_H
#define ___IIC_H
	#include "stm32f10x_type.h"
	#include "stm32f10x_conf.h"
	#include "stm32f10x_map.h"
	#include "stm32f10x_nvic.h"
	#include "GPIO.h"
	#include "delay.h"

	//SDA --> PB7  SCL --> PB6
	#define SDA_Out()     	{GPIOB->CRL&=0x0fffffff;GPIOB->CRL|=0x30000000;}
	#define SDA_In()      	{GPIOB->CRL&=0x0fffffff;GPIOB->CRL|=0x80000000;GPIOB->ODR|=(unsigned int)1<<7;}
	#define SDA_STATUS     	GPI(GPIOB,GPIO_Pin_7)
	#define IIC_SCL(n)     	GPO(GPIOB,GPIO_Pin_6,n)
	#define IIC_SDA(n)     	GPO(GPIOB,GPIO_Pin_7,n)

	/*******************************
	** 常用IIC器件地址宏定义 
	*******************************/
	#define	MPU6050				0xD0
	#define HCM5883				0x3C	  
	#define BMP085				0xee
	#define AT24C02 			0xa0

	void IIC_Init(void);
	void IIC_Send(u8 device,u8 addr,u8 dat);
	u8 	 IIC_Read(u8 device,u8 addr);
	void IIC_ReadBUF(u8 device,u8 addr,u8* BUF,u8 lenth);
	short Multiple_read(u8 device,u8 addr);
	long BMP085ReadTemp(void);
	long BMP085ReadPressure(void);
#endif

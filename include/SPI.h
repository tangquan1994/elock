#ifndef __SPI_H
#define __SPI_H
	#include "stm32f10x_type.h"
	#include "stm32f10x_conf.h"
	#include "stm32f10x_map.h"
	#include "stm32f10x_nvic.h"
	#include "sys.h"
				    
	// SPI�����ٶ�����
	#define SPI_SPEED_2   		0
	#define SPI_SPEED_4   		1
	#define SPI_SPEED_8   		2
	#define SPI_SPEED_16  		3
	#define SPI_SPEED_32 			4
	#define SPI_SPEED_64 			5
	#define SPI_SPEED_128 		6
	#define SPI_SPEED_256 		7
						  	    													  
	void 	iSPI_Init(SPI_TypeDef* SPI);			 //��ʼ��SPI��
	void  SPI_SetSpeed(SPI_TypeDef* SPI,u8 SpeedSet); //����SPI�ٶ�   
	u8 		SPI_RW(SPI_TypeDef* SPI,u8 TxData);//SPI���߶�дһ���ֽ�

#endif
		  

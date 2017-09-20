#include "SPI.h"

//������SPIģ��ĳ�ʼ������,��MSP430����ͨ��,��Ϊ�ӻ�����MPU6050�����ĽǶ�����			  
//SPI��ΪSLAVEģʽ,DMAģʽ,����25���ֽڵ����ݺ�����ж�
//һ��float��������ռ��4���ֽ�,ʣ��һ����Ϊ������Ϣ.
void iSPI_Init(SPI_TypeDef* SPI)
{
	RCC_SW(APB2_SPI1|APB2_GPIOA|APB2_GPIOB,On);
	RCC_SW(APB2_GPIOB,On);
	
	RCC_SW(APB1_SPI2,On);

	//SPI1�˿�
	iGPIO_Init(GPIOA,Pin_5|Pin_6|Pin_7,iGPIO_Mode_AF_PP,iGPIO_Speed_50MHz);

	//SPI2�˿�
	iGPIO_Init(GPIOB,Pin_13|Pin_14|Pin_15,iGPIO_Mode_AF_PP,iGPIO_Speed_50MHz);

	//SPI3�˿�
// 	GPIO_Init(GPIOB,Pin_3|Pin_4|Pin_5,GPIO_Mode_AF_PP,GPIO_Speed_50MHz);

	SPI->CR1  |= 0<<10;				//ȫ˫��ģʽ
	SPI->CR1  |=1<<9; 				//���nss����
	SPI->CR1  |=1<<8;  

	SPI->CR1 |= 1<<2; 				//SPI����
	SPI->CR1 |= 0<<11;				//8bit���ݸ�ʽ
	
	//SI4432��SPI����ģʽ����������********************
	SPI->CR1 |= 1<<1;			//����ģʽ��SCKΪ1 CPOL=1
	SPI->CR1 |= 1<<0;			//���ݲ����ӵڶ���ʱ����ؿ�ʼ,CPHA=1  
	
	//��SPI����APB1������.ʱ��Ƶ�����Ϊ36M.
	SPI->CR1 |= 3<<3; 				//Fsck=Fpclk1/16
	SPI->CR1 |= 0<<7; 				//MSBfirst�ȴ��͸�λ  
	SPI->CR1 |= 1<<6; 				//SPI�豸ʹ��
	
	//SPI����DMAģʽ
	SPI->CR2 &= ~(1<<6);			//��ֹ�����ж�
// 	SPI->CR2 |= 0x0001;				//��������DMA
	
	SPI_RW(SPI,0xff);	//��������		 
}   
//SPI�ٶ����ú���
//SpeedSet:0~7
//SPI�ٶ�=fAPB1/2^(SpeedSet+1)
//APB1ʱ��һ��Ϊ36Mhz
void SPI_SetSpeed(SPI_TypeDef* SPI,u8 SpeedSet)
{
	SpeedSet&=0X07;						//���Ʒ�Χ
	SPI->CR1&=0XFFC7;
	SPI->CR1|=SpeedSet<<3;		//����SPI�ٶ�  
	SPI->CR1|=1<<6; 					//SPI�豸ʹ��
} 


//SPI ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI_RW(SPI_TypeDef* SPI,u8 TxData)
{
	u16 retry=0;
	while((SPI->SR&0x0002)==0)		//�ȴ���������			  
	{
		retry++;
		if(retry>=200)return 0;			//��ʱ�˳�
	}
	SPI->DR=TxData;	 	  					//����һ��byte 
	retry = 0;
	while((SPI->SR&0x0001)==0) 		//�ȴ�������һ��byte  
	{
		retry++;
		if(retry>=200)return 0;			//��ʱ�˳�
	}
	return SPI->DR;          			//�����յ�������				    
}

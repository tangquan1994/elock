#include "SPI.h"

//以下是SPI模块的初始化代码,和MSP430进行通信,作为从机接收MPU6050发来的角度数据			  
//SPI作为SLAVE模式,DMA模式,接收25个字节的数据后产生中断
//一个float类型数据占用4个字节,剩下一个作为控制信息.
void iSPI_Init(SPI_TypeDef* SPI)
{
	RCC_SW(APB2_SPI1|APB2_GPIOA|APB2_GPIOB,On);
	RCC_SW(APB2_GPIOB,On);
	
	RCC_SW(APB1_SPI2,On);

	//SPI1端口
	iGPIO_Init(GPIOA,Pin_5|Pin_6|Pin_7,iGPIO_Mode_AF_PP,iGPIO_Speed_50MHz);

	//SPI2端口
	iGPIO_Init(GPIOB,Pin_13|Pin_14|Pin_15,iGPIO_Mode_AF_PP,iGPIO_Speed_50MHz);

	//SPI3端口
// 	GPIO_Init(GPIOB,Pin_3|Pin_4|Pin_5,GPIO_Mode_AF_PP,GPIO_Speed_50MHz);

	SPI->CR1  |= 0<<10;				//全双工模式
	SPI->CR1  |=1<<9; 				//软件nss管理
	SPI->CR1  |=1<<8;  

	SPI->CR1 |= 1<<2; 				//SPI主机
	SPI->CR1 |= 0<<11;				//8bit数据格式
	
	//SI4432的SPI工作模式不是这样的********************
	SPI->CR1 |= 1<<1;			//空闲模式下SCK为1 CPOL=1
	SPI->CR1 |= 1<<0;			//数据采样从第二个时间边沿开始,CPHA=1  
	
	//对SPI属于APB1的外设.时钟频率最大为36M.
	SPI->CR1 |= 3<<3; 				//Fsck=Fpclk1/16
	SPI->CR1 |= 0<<7; 				//MSBfirst先传送高位  
	SPI->CR1 |= 1<<6; 				//SPI设备使能
	
	//SPI开启DMA模式
	SPI->CR2 &= ~(1<<6);			//禁止接收中断
// 	SPI->CR2 |= 0x0001;				//开启接收DMA
	
	SPI_RW(SPI,0xff);	//启动传输		 
}   
//SPI速度设置函数
//SpeedSet:0~7
//SPI速度=fAPB1/2^(SpeedSet+1)
//APB1时钟一般为36Mhz
void SPI_SetSpeed(SPI_TypeDef* SPI,u8 SpeedSet)
{
	SpeedSet&=0X07;						//限制范围
	SPI->CR1&=0XFFC7;
	SPI->CR1|=SpeedSet<<3;		//设置SPI速度  
	SPI->CR1|=1<<6; 					//SPI设备使能
} 


//SPI 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI_RW(SPI_TypeDef* SPI,u8 TxData)
{
	u16 retry=0;
	while((SPI->SR&0x0002)==0)		//等待发送区空			  
	{
		retry++;
		if(retry>=200)return 0;			//超时退出
	}
	SPI->DR=TxData;	 	  					//发送一个byte 
	retry = 0;
	while((SPI->SR&0x0001)==0) 		//等待接收完一个byte  
	{
		retry++;
		if(retry>=200)return 0;			//超时退出
	}
	return SPI->DR;          			//返回收到的数据				    
}

#include "IIC.h"

/*******************************
 ** IIC器件的选择地址
*******************************/

void IIC_delay(u16 i)
{
	while(i--); 
}

/*******************************
 ** IIC总线初始化 
 ** PB6为SCL
 ** PB7为SDA
*******************************/

void IIC_Init(void)
{
  RCC->APB2ENR |= 1<<3;						//??GPIOB??
	GPIO_Init(GPIOB,GPIO_Pin_6|GPIO_Pin_7,GPIO_Mode_Out_PP,GPIO_Speed_2MHz);
}

/*******************************
 ** start signal
 ** 
*******************************/
void IIC_Start()
{
	SDA_Out();
	IIC_SDA(1);
	IIC_SCL(1);
	IIC_delay(5);
	IIC_SDA(0);
	IIC_delay(5);
	IIC_SCL(0);
}
void IIC_Stop()
{
	SDA_Out();
	IIC_SCL(0);
	IIC_SDA(0);
	IIC_delay(5);
	IIC_SCL(1);
	IIC_delay(5);
	IIC_SDA(1);
	IIC_delay(5);
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_In();      //SDA设置为输入  
	IIC_SDA(1);
	IIC_SCL(1);
	IIC_delay(5);	 
	while(SDA_STATUS)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL(0);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL(0);
	SDA_Out();
	IIC_SDA(0);
	IIC_delay(5);
	IIC_SCL(1);
	IIC_delay(5);
	IIC_SCL(0);
}

//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL(0);
	SDA_Out();
	IIC_SDA(1);
	IIC_delay(5);
	IIC_SCL(1);
	IIC_delay(5);
	IIC_SCL(0);
}

void IIC_SendByte(u8 dat)
{
	u8 i;
	SDA_Out();
	IIC_SCL(0);
	IIC_SDA(0);
	for(i=0;i<8;i++)
	{
		if(dat & 0x80)
		{
			IIC_SDA(1);
		}
		else
		{
			IIC_SDA(0);
		}      
		IIC_delay(5);
		IIC_SCL(1);
		IIC_delay(5);
		IIC_SCL(0);
		IIC_delay(5);
		dat <<= 1;
	}	
}

void IIC_Send(u8 device,u8 addr,u8 dat)
{
	IIC_Start();
	IIC_SendByte(device);
	IIC_Wait_Ack();
	IIC_SendByte(addr);
	IIC_Wait_Ack();
	IIC_SendByte(dat);
	IIC_Wait_Ack();
	IIC_Stop();
// 	delay_ms(5);	//in consideration of the low speed of IIC BUS we take a short break here 
}

u8 IIC_ReadByte()
{
	u8 temp=0,i;
	IIC_SDA(1);
	SDA_In();
	for(i=0;i<8;i++)
	{
		IIC_SCL(0);
		IIC_delay(5);
		IIC_SCL(1);
		IIC_delay(5);
		temp <<= 1;
		if(SDA_STATUS) //不能用temp |= SDA_STATUS； 这样不好！！  类型一定要严谨！！
			temp |= 1; 
		IIC_delay(5);
	}
	return temp;
}

u8 IIC_Read(u8 device,u8 addr)
{
	u8 temp;
	IIC_Start();
	IIC_SendByte(device);
	IIC_Wait_Ack();
	IIC_SendByte(addr);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_SendByte(device+1);
	IIC_Wait_Ack();
	temp = IIC_ReadByte();
	IIC_NAck();//发送nACK
	IIC_Stop();
// 	delay_ms(5);
	return temp;
}

//连续读多个数据,专门为HCM5883设计
void IIC_ReadBUF(u8 device,u8 addr,u8* BUF,u8 lenth)
{
	u8 i;
	IIC_Start();
	IIC_SendByte(device);
	IIC_Wait_Ack();
	IIC_SendByte(addr);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_SendByte(device+1);
	IIC_Wait_Ack();
	//连续读取lenth个数据
	for(i=0;i<lenth;i++)
	{
		BUF[i] = IIC_ReadByte();
		if(i == 5)
			IIC_NAck();	//发送nACK
		else
			IIC_Ack();	//发送ACK
	}
	IIC_Stop();
// 	delay_ms(5);
}

//读一个16位的数据,,专门为BMP085设计
short Multiple_read(u8 device,u8 addr)
{
	u8 msb,lsb;
	IIC_Start();
	IIC_SendByte(device);
	IIC_Wait_Ack();
	IIC_SendByte(addr);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_SendByte(device+1);
	IIC_NAck();	//发送nACK
	msb = IIC_ReadByte();
	IIC_Ack();
	lsb = IIC_ReadByte();
	IIC_Ack();	//发送ACK}
	IIC_Stop();
	return (msb<<8)|lsb;
}

long BMP085ReadTemp(void)
{
	IIC_Start();
	IIC_SendByte(BMP085);
	IIC_Wait_Ack();
	IIC_SendByte(0xF4);
	IIC_Wait_Ack();
	IIC_SendByte(0x2E);
	IIC_Wait_Ack();
	IIC_Stop();
	delay_ms(5);	// max time is 4.5ms	
	return (long)Multiple_read(BMP085,0xF6);
}

long BMP085ReadPressure(void)
{
	long pressure;
	IIC_SendByte(BMP085);
	IIC_Wait_Ack();
	IIC_SendByte(0xF4);
	IIC_Wait_Ack();
	IIC_SendByte(0x34);
	IIC_Wait_Ack();
	IIC_Stop();
	delay_ms(5);	// max time is 4.5ms	
	pressure = (long)Multiple_read(BMP085,0xF6);
	pressure &= 0x0000ffff;
	return (long)Multiple_read(BMP085,0xF6);
}













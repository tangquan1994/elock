#include  "GPIO.h"


void iGPIO_Init(GPIO_TypeDef* PORT,u16 PINS,u8 MODE,u8 Frequency)
{
	unsigned char i,temp;
	//不管模式配置成什么,都上拉
	GPO(PORT,PINS,1);					//默认输出高电平
	//整合.如果是输入模式的话因为低两位为00,故将不会影响MODE的值
	//如果是输出模式的话低两位默认为11,相与之后低两位变成想要的.
	MODE |= Frequency;
	for(i=0;i<=15;i++)
	{
		if((PINS&((u16)1<<i)) != 0)
		{
			PINS &= 	~((u16)1<<i);
			temp = 	 	i%8;			//对8取模
			temp <<= 	2;				//temp乘以4,因为一个引脚有四个位控制,用左移比用乘法效率高
			if(i >= 8)
			{
				PORT->CRH &= ~((u32)0xf << temp);
				PORT->CRH |= (u32)MODE << temp;
			}
			else
			{
				PORT->CRL &= ~((u32)0xf << temp);
				PORT->CRL |= (u32)MODE << temp;
			}
			if(PINS == (u16)0)
				break;
		}
	}
}








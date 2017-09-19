#include  "GPIO.h"


void iGPIO_Init(GPIO_TypeDef* PORT,u16 PINS,u8 MODE,u8 Frequency)
{
	unsigned char i,temp;
	//����ģʽ���ó�ʲô,������
	GPO(PORT,PINS,1);					//Ĭ������ߵ�ƽ
	//����.���������ģʽ�Ļ���Ϊ����λΪ00,�ʽ�����Ӱ��MODE��ֵ
	//��������ģʽ�Ļ�����λĬ��Ϊ11,����֮�����λ�����Ҫ��.
	MODE |= Frequency;
	for(i=0;i<=15;i++)
	{
		if((PINS&((u16)1<<i)) != 0)
		{
			PINS &= 	~((u16)1<<i);
			temp = 	 	i%8;			//��8ȡģ
			temp <<= 	2;				//temp����4,��Ϊһ���������ĸ�λ����,�����Ʊ��ó˷�Ч�ʸ�
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








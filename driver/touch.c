#include "TOUCH.h"

float A,B,C,D;					//У׼ϵ��
u16 ADJ_DATA[4][2];			//��������������,�з�����
// const u16 ADJ_POINT[5][2] = {{20,20},{300,20},{20,220},{300,220},{160,120}};		//���Ͻ� ���Ͻ� ���½� ���½� �е�
const u16 ADJ_POINT[5][2] = {{20,20},{220,20},{20,300},{220,300},{120,160}};		//���Ͻ� ���Ͻ� ���½� ���½� �е�
u16 TOUCH_[2]	= {0,0};	//���ζ���
bool IS_CORRECTED = FALSE;

void TOUCH_Init(void)
{
	iSPI_Init(SPI2);
	RCC_SW(APB2_GPIOG,On);
	iGPIO_Init(GPIOG,Pin_7,iGPIO_Mode_IPU,iGPIO_Input);
	iGPIO_Init(GPIOB,Pin_12,iGPIO_Mode_Out_PP,iGPIO_Speed_50MHz);
	
	//�������жϳ�ʼ��
	iNVIC_Init(EXTI9_5_IRQChannel,2,2,Enable);
	iEXTI_Init(GPIOG,7,FTIR,Enable);
	
	//��֪��Ϊʲô �����һ�β�����int�������ź����
	TOUCH_READ();
	//������һ�ε�����
	A = 0.065402;
	B = -15.513409;
	C = -0.074967;
	D = 328.262390;
// 	TOUCH_ADJ();					//��ʹ�ò���������  ������У׼
	IS_CORRECTED = TRUE;		//����Ϊ��У׼
}

u16 TOUCH_GetData(u8 channel)
{
	//�������ʱ,��Ȼ�����������ݲ��ȶ��Ҿ��ȵ��Ҳ�����
	u16 x=0;
	TOUCH_CS = 0;
	//delay_us(10);	��֪��Ϊʲô������ʱ����������,��Ŀ���,�ѵ��������ʱ���̫��ô?????
	SPI_RW(SPI2,channel);
	//delay_us(10);
	x=SPI_RW(SPI2,0x00);
	x <<= 8;
	x += SPI_RW(SPI2,0x00);
	//delay_us(10);
	x = x>>3;
	TOUCH_CS = 1;
	return (x);
}

extern 	void OS_Dly(u32 ticks);
bool TOUCH_PRESSED(int *x,int *y)
{
	if(!TOUCH_INT)
	{
		OS_Dly(10);	//�����ʱ���Ǹ�Сһ��,��Ϊϵͳ���ȵĿ���,̫��Ļ����׵��°���ʱ����̵Ļ���ⲻ������
		if(!TOUCH_INT)
		{
			TOUCH_READ();
			*x = TOUCH_[0];
			*y = TOUCH_[1];
			return TRUE;
		}
	}
	return FALSE;
}

void TOUCH_READ(void)
{
	#define SAMPLE_CNT	10
	u8 i;
	u16 MAX = 0,MIN = 65535,temp;
	u32 SUM_X=0,SUM_Y=0;
	for(i=0;i<SAMPLE_CNT;i++)
	{
		temp = TOUCH_GetData(X_CHANNEL);
		MAX = (temp > MAX)?temp:MAX;		//��ȡ���ֵ
		MIN = (temp > MIN)?MIN:temp;		//��ȡ��Сֵ
		SUM_X += temp;
	}
	SUM_X -= (MAX + MIN);		//��ȥ���ֵ����Сֵ
	MAX = MIN = 0;
	for(i=0;i<SAMPLE_CNT;i++)
	{
		temp = TOUCH_GetData(Y_CHANNEL);
		MAX = (temp > MAX)?temp:MAX;		//��ȡ���ֵ
		MIN = (temp > MIN)?MIN:temp;		//��ȡ��Сֵ
		SUM_Y += temp;
	}
	SUM_Y -= (MAX + MIN);		//��ȥ���ֵ
	
	TOUCH_[X_AXIS] = SUM_X / (SAMPLE_CNT-2);
	TOUCH_[Y_AXIS] = SUM_Y / (SAMPLE_CNT-2);

	if(IS_CORRECTED == TRUE)
	{
		TOUCH_[X_AXIS] = A*TOUCH_[X_AXIS] + B;
		TOUCH_[Y_AXIS] = C*TOUCH_[Y_AXIS] + D;
	}
// 	printf("%d %d\n\r\n",TOUCH_[0],TOUCH_[1]);
}

void TOUCH_ADJ(void)
{
	u8 i;
	bool OK = FALSE;
	int error;
	LCD_Clear(Black);
	while(!OK)
	{
		GUI_DRAW_CROSS(ADJ_POINT[4][0],ADJ_POINT[4][1],Black);
		for(i=0;i<4;i++)
		{
			if(i > 0)	//�����һ�ε�ʮ�ּ�
				GUI_DRAW_CROSS(ADJ_POINT[i-1][0],ADJ_POINT[i-1][1],Black);
			GUI_DRAW_CROSS(ADJ_POINT[i][0],ADJ_POINT[i][1],White);
			while(TOUCH_INT)	//ȥ��,����!!
			{
				while(TOUCH_INT);
				delay_ms(10);
			}
			while(!TOUCH_INT)	//ȥ��,����!!
			{
				while(!TOUCH_INT)
				{
					TOUCH_READ();
					delay_ms(10);
				}
			}
			ADJ_DATA[i][0] = TOUCH_[0];
			ADJ_DATA[i][1] = TOUCH_[1];
		}
		//���������ƫ��ϵ��
		{
			A = (((float)ADJ_POINT[0][0]-(float)ADJ_POINT[3][0])/ \
					((float)ADJ_DATA[0][0] - (float)ADJ_DATA[3][0]));
					
			B = (float)ADJ_POINT[0][0] - A * ADJ_DATA[0][0];
			
			C = (((float)ADJ_POINT[0][1]-(float)ADJ_POINT[3][1])/ \
					((float)ADJ_DATA[0][1] - (float)ADJ_DATA[3][1]));
					
			D = (float)ADJ_POINT[0][1] - C * ADJ_DATA[0][1];

			printf("%d %d\n\r\n",ADJ_DATA[0][0],ADJ_DATA[0][1]);
			printf("%d %d\n\r\n",ADJ_DATA[1][0],ADJ_DATA[1][1]);
			printf("%d %d\n\r\n",ADJ_DATA[2][0],ADJ_DATA[2][1]);
			printf("%d %d\n\r\n",ADJ_DATA[3][0],ADJ_DATA[3][1]);
			printf("%f %f %f %f\n\r\n",A,B,C,D);
		}
		IS_CORRECTED = TRUE;
		
		//��֤ϵ����׼ȷ��
		GUI_DRAW_CROSS(ADJ_POINT[i-1][0],ADJ_POINT[i-1][1],Black);
		GUI_DRAW_CROSS(ADJ_POINT[i][0],ADJ_POINT[i][1],White);
		while(TOUCH_INT)	//ȥ��,����!!
		{
			while(TOUCH_INT);
			delay_ms(10);
		}
		while(!TOUCH_INT)	//ȥ��,����!!
		{
			while(!TOUCH_INT)
			{
				TOUCH_READ();
				delay_ms(10);
			}
		}
		printf("%d %d\n\r\n",TOUCH_[0],TOUCH_[1]);
		for(i=0;i<2;i++)
		{
			error = (int)TOUCH_[i]-(int)ADJ_POINT[4][i];
			if(error > 5 || error < -5)
			{
				OK = FALSE;
				IS_CORRECTED = FALSE;
			}
			else
			{
				OK = TRUE;
				IS_CORRECTED = TRUE;
			}
		}
	}
	GUI_DRAW_CROSS(ADJ_POINT[4][0],ADJ_POINT[4][1],Black);
}




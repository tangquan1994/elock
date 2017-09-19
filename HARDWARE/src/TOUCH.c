#include "TOUCH.h"

float A,B,C,D;					//校准系数
u16 ADJ_DATA[4][2];			//触摸屏矫正数据,有符号数
// const u16 ADJ_POINT[5][2] = {{20,20},{300,20},{20,220},{300,220},{160,120}};		//左上角 右上角 左下角 右下角 中点
const u16 ADJ_POINT[5][2] = {{20,20},{220,20},{20,300},{220,300},{120,160}};		//左上角 右上角 左下角 右下角 中点
u16 TOUCH_[2]	= {0,0};	//本次读数
bool IS_CORRECTED = FALSE;

void TOUCH_Init(void)
{
	iSPI_Init(SPI2);
	RCC_SW(APB2_GPIOG,On);
	iGPIO_Init(GPIOG,Pin_7,iGPIO_Mode_IPU,iGPIO_Input);
	iGPIO_Init(GPIOB,Pin_12,iGPIO_Mode_Out_PP,iGPIO_Speed_50MHz);
	
	//触屏脚中断初始化
	iNVIC_Init(EXTI9_5_IRQChannel,2,2,Enable);
	iEXTI_Init(GPIOG,7,FTIR,Enable);
	
	//不知道为什么 必须读一次才能让int引脚有信号输出
	TOUCH_READ();
	//测量过一次的数据
	A = 0.065402;
	B = -15.513409;
	C = -0.074967;
	D = 328.262390;
// 	TOUCH_ADJ();					//已使用测量的数据  无需再校准
	IS_CORRECTED = TRUE;		//设置为已校准
}

u16 TOUCH_GetData(u8 channel)
{
	//必须加延时,不然读出来的数据不稳定且精度低且波动大
	u16 x=0;
	TOUCH_CS = 0;
	//delay_us(10);	不知道为什么加上延时反而不好了,真的坑人,难道是软件延时误差太大么?????
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
		OS_Dly(10);	//这个延时还是改小一点,因为系统调度的开销,太大的话容易导致按下时间过短的话检测不到按下
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
		MAX = (temp > MAX)?temp:MAX;		//获取最大值
		MIN = (temp > MIN)?MIN:temp;		//获取最小值
		SUM_X += temp;
	}
	SUM_X -= (MAX + MIN);		//减去最大值和最小值
	MAX = MIN = 0;
	for(i=0;i<SAMPLE_CNT;i++)
	{
		temp = TOUCH_GetData(Y_CHANNEL);
		MAX = (temp > MAX)?temp:MAX;		//获取最大值
		MIN = (temp > MIN)?MIN:temp;		//获取最小值
		SUM_Y += temp;
	}
	SUM_Y -= (MAX + MIN);		//减去最大值
	
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
			if(i > 0)	//清除上一次的十字架
				GUI_DRAW_CROSS(ADJ_POINT[i-1][0],ADJ_POINT[i-1][1],Black);
			GUI_DRAW_CROSS(ADJ_POINT[i][0],ADJ_POINT[i][1],White);
			while(TOUCH_INT)	//去抖,技巧!!
			{
				while(TOUCH_INT);
				delay_ms(10);
			}
			while(!TOUCH_INT)	//去抖,技巧!!
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
		//计算比例及偏移系数
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
		
		//验证系数的准确性
		GUI_DRAW_CROSS(ADJ_POINT[i-1][0],ADJ_POINT[i-1][1],Black);
		GUI_DRAW_CROSS(ADJ_POINT[i][0],ADJ_POINT[i][1],White);
		while(TOUCH_INT)	//去抖,技巧!!
		{
			while(TOUCH_INT);
			delay_ms(10);
		}
		while(!TOUCH_INT)	//去抖,技巧!!
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




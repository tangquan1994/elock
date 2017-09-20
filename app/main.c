#include "includes.h"
#include "ws2812.h"
#include "libqueue.h"
#include "at.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_dbgmcu.h"

extern Queue USART2ReciQueue;
#define TEST	PCout(13)
#define RF_CS	PAout(4)
#define RF_SET	PAout(5)

#define RECEIVE_TIMEOUT		(60)	//等待接收超时ms
#define WORK_CYCLE			(3)	//休眠工作周期ms


#define DEVICE_ID	0

typedef enum
{
	COM_NONE = 0,
	COM_OPENDOOR = 1,
}Command;

int ReciState = 0;
int device_id = 0;
Command command_id = COM_NONE;



void SystemInit()
{
	MCU_INIT(2);					//系统时钟72MHz
	Uart_Init(USART1,115200,8);	//调试串口
	Uart_Init(USART2,9600,8);		//
	Delay_Init();
	QueueInit(&USART2ReciQueue);	//串口接收队列初始化

	printf("========================================================\r\n");
	printf("**********************system start**********************\r\n");
	printf("========================================================\r\n");
}

//设置无线模块的参数数组
u8 ParaSetBuff[] =
{
	0xAA,	//0xaa（命令字节）
	0x5A,	//0x5a（命令字节）
	0x22,	//Self ID 2Byte(模块 ID)
	0x33,	//
	0x11,	//Net ID 2Byte(组网 ID)
	0x22,	//
	0x00,	//Nc
	0x00,	//RF Power 1Byte(0~14)
	0x00,	//Nc
	0x04,	//Baud 1Byte(0~6)波特率
	0x00,	//Nc
	0x64,	//RF CHN 1Byte(0~127)
	0x00,	//Nc
	0x00,	//Nc
	0x00,	//Nc
	0x12,	//Lenght 1Byte(0x12)
	0x00,	//Nc
	0x06,	//CheckSum
};

void Set_RF_Para(	unsigned short self_id,
					unsigned short net_id,
					unsigned char rf_pwr,
					unsigned char baudrate,
					unsigned char rf_ch)
{
	int i;
	RF_CS = 0;
	RF_SET = 0;
	delay_ms(55);	//至少需要延时55ms等待无线模块就绪，测试出来的值

	ParaSetBuff[2] = (unsigned char)self_id;
	ParaSetBuff[3] = (unsigned char)(self_id >> 8);

	ParaSetBuff[4] = (unsigned char)net_id;
	ParaSetBuff[5] = (unsigned char)(net_id >> 8);

	ParaSetBuff[7] = (unsigned char)rf_pwr;

	ParaSetBuff[9] = (unsigned char)baudrate;

	ParaSetBuff[11] = (unsigned char)rf_ch;

	ParaSetBuff[17] = 0;

	for(i=0;i<17;i++)
		ParaSetBuff[17] += ParaSetBuff[i];

// 	for(i=0;i<18;i++)
// 		printf("%.2X ",ParaSetBuff[i]);

	UartSendBuff(USART2,ParaSetBuff,sizeof(ParaSetBuff) / sizeof(ParaSetBuff[0]));

	for(i=0;i<18;i++)
		printf("%.2X ",UartGetByte(USART2));

	RF_SET = 1;	//等待收到回应才能把SET引脚置位1,不然会收不到回应

	printf("parameter set finished\r\n");
}

int main(void)
{
	int i,count=0;
	u8 data;
	SystemInit();

	RCC_SW(APB2_GPIOA,On);
	iGPIO_Init(GPIOA,Pin_4,iGPIO_Mode_Out_PP,iGPIO_Speed_2MHz);		//初始化CS引脚
	iGPIO_Init(GPIOA,Pin_5,iGPIO_Mode_Out_PP,iGPIO_Speed_2MHz);		//初始化SET引脚

// 	while(1)
// 	{
// 		printf("receive timeout\r\n");
// 		delay_ms(1000);
// 	}

	RF_CS = 0;
	RF_SET = 1;
// 	delay_ms(55);	//至少需要延时55ms等待无线模块就绪，测试出来的值

// 	Set_RF_Para(0x0001,0x0001,0,4,0);
// 	UartSendStr(USART2,"tangquan");

	USART2->SR &= ~(1<<5); 	//清除RXNE标志位,打开无线模块的时候由于引脚的电平原因会收到一个0x00字节，这个字节是没用的数据

// 	while(UartGetByte3(USART2,&data) == 0)
// 	{
// 		count++;
// 		if(count == RECEIVE_TIMEOUT)
// 			break;
// 		delay_ms(1);
// 	}
// 	if(count == RECEIVE_TIMEOUT)
// 	{
// 		printf("receive timeout\r\n");
// 	}
// 	else
// 	{
// 		printf("%.2X ",data);
// 	}

	while(1)
	{
		if(ReciState == 0)
		{
			if(UartGetByte3(USART2,&data) == 0)
			{
				count++;
				if(count == RECEIVE_TIMEOUT)
					ReciState = 1;
			}
			else
			{
				USART2->SR &= ~(1<<5);	//清除标志位
				count = 0;	//计数清零
				ReciState = 2;
			}
		}
		else if(ReciState == 1)
		{
			printf("receive timeout\r\n");
			break;
		}
		else if(ReciState == 2)
		{
			device_id = data >> 4;
			command_id = data & 0x0F;
			printf("device_id:%d , command_id:%d\r\n",device_id,command_id);
			if(device_id == DEVICE_ID)
			{
				printf("device id match\r\n");
				if(command_id == COM_OPENDOOR)
				{
					printf("command:%s\r\n","COM_OPENDOOR");

				}
			}
			else
			{
				printf("device id unmatch\r\n");
			}
			break;
		}
		delay_ms(1);
	}


// 	RF_CS = 1;	//关闭无线模块

	delay_ms(1);	//延时一会等待串口数据发送完了

	//将串口引脚设置为高阻态并关闭GPIOA的时钟，降低功耗，数据手册上说的是进入待机模式之后
	//引脚会自动进入高阻态，但是测量的时候发现PA3引脚为0V，PA2为3.3V，不知道为什么，而无线
	//模块进入休眠的时候TXD和RXD引脚都为3.3V，PA3引脚为0V可能是高阻态也可能是真的低电平,
	//测量之后发现PA3的低电平应该是连到了GND，不是高阻态，这样会导致PA3因脚上有电流，进入
	//待机之前手动将引脚设置为高阻态后测量发现正常了，引脚没有电流了。
	iGPIO_Init(GPIOA,Pin_2,iGPIO_Mode_IN_FLOATING,iGPIO_Input);
	iGPIO_Init(GPIOA,Pin_3,iGPIO_Mode_IN_FLOATING,iGPIO_Input);

	iGPIO_Init(GPIOA,Pin_9,iGPIO_Mode_IN_FLOATING,iGPIO_Input);
	iGPIO_Init(GPIOA,Pin_10,iGPIO_Mode_IN_FLOATING,iGPIO_Input);

	iGPIO_Init(GPIOA,Pin_13,iGPIO_Mode_IN_FLOATING,iGPIO_Input);
	iGPIO_Init(GPIOA,Pin_14,iGPIO_Mode_IN_FLOATING,iGPIO_Input);
	iGPIO_Init(GPIOA,Pin_15,iGPIO_Mode_IN_FLOATING,iGPIO_Input);

	RCC_SW(APB2_GPIOA,Off);

	iGPIO_Init(GPIOB,Pin_3,iGPIO_Mode_IN_FLOATING,iGPIO_Input);
	iGPIO_Init(GPIOB,Pin_4,iGPIO_Mode_IN_FLOATING,iGPIO_Input);
	RCC_SW(APB2_GPIOB,Off);

	Timer_Switch(DELAY_TIM,Off);	//关闭定时器

	//所有引脚设置成模拟输入模式，可以明显引脚的漏电流。
	GPIOA->CRL = 0x00000000;
	GPIOA->CRH = 0x00000000;
	GPIOB->CRL = 0x00000000;
	GPIOB->CRH = 0x00000000;
	GPIOC->CRL = 0x00000000;
	GPIOC->CRH = 0x00000000;


	//---------------------------------------问题---------------------------------------
	//1、使用JTAG下载完程序之后工作电流会比较大，不知道为什么，断电上一次电之后功耗变正常
	//复位不行，只能断电上电功耗才能变得正常

	//2、第一次上电的时候可定会receive timeout，也不知道为什么，估计是因为刚上电的时候无线
	//模块的初始化时间较长，而CS从失能变成使能的时候初始化时间较短一些。


// 	PWR->CSR |= 1 << 8;	//使能WKUP引脚，WKUP上升沿的时候从待机模式恢复
	PWR_WakeUpPinCmd(ENABLE);


// 	IWDG_Enable();
// 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
// 	IWDG_SetPrescaler(IWDG_Prescaler_256);
// 	IWDG_SetReload(156 * WORK_CYCLE);
// 	IWDG_ReloadCounter();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);

	AFIO->MAPR &= ~(7 << 24);
	AFIO->MAPR |= (2 << 24);	//010：关闭JTAG-DP，启用SW-DP；
// 	DBGMCU_Config(DBGMCU_STANDBY,Enable);	//Keep debugger connection during STANDBY mode

// 	printf("\r\nBefore PWR_EnterSTANDBYMode\r\n");
	PWR_EnterSTANDBYMode();	//进入standby低功耗模式，进入standby模式之后SWD不能直接下载，需要复位之后才能下载
// 	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);	//进入stop低功耗模式
// 	printf("After PWR_EnterSTANDBYMode\r\n");

	while(1)
	{
	}
}






















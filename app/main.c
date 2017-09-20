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

#define RECEIVE_TIMEOUT		(60)	//�ȴ����ճ�ʱms
#define WORK_CYCLE			(3)	//���߹�������ms


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
	MCU_INIT(2);					//ϵͳʱ��72MHz
	Uart_Init(USART1,115200,8);	//���Դ���
	Uart_Init(USART2,9600,8);		//
	Delay_Init();
	QueueInit(&USART2ReciQueue);	//���ڽ��ն��г�ʼ��

	printf("========================================================\r\n");
	printf("**********************system start**********************\r\n");
	printf("========================================================\r\n");
}

//��������ģ��Ĳ�������
u8 ParaSetBuff[] =
{
	0xAA,	//0xaa�������ֽڣ�
	0x5A,	//0x5a�������ֽڣ�
	0x22,	//Self ID 2Byte(ģ�� ID)
	0x33,	//
	0x11,	//Net ID 2Byte(���� ID)
	0x22,	//
	0x00,	//Nc
	0x00,	//RF Power 1Byte(0~14)
	0x00,	//Nc
	0x04,	//Baud 1Byte(0~6)������
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
	delay_ms(55);	//������Ҫ��ʱ55ms�ȴ�����ģ����������Գ�����ֵ

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

	RF_SET = 1;	//�ȴ��յ���Ӧ���ܰ�SET������λ1,��Ȼ���ղ�����Ӧ

	printf("parameter set finished\r\n");
}

int main(void)
{
	int i,count=0;
	u8 data;
	SystemInit();

	RCC_SW(APB2_GPIOA,On);
	iGPIO_Init(GPIOA,Pin_4,iGPIO_Mode_Out_PP,iGPIO_Speed_2MHz);		//��ʼ��CS����
	iGPIO_Init(GPIOA,Pin_5,iGPIO_Mode_Out_PP,iGPIO_Speed_2MHz);		//��ʼ��SET����

// 	while(1)
// 	{
// 		printf("receive timeout\r\n");
// 		delay_ms(1000);
// 	}

	RF_CS = 0;
	RF_SET = 1;
// 	delay_ms(55);	//������Ҫ��ʱ55ms�ȴ�����ģ����������Գ�����ֵ

// 	Set_RF_Para(0x0001,0x0001,0,4,0);
// 	UartSendStr(USART2,"tangquan");

	USART2->SR &= ~(1<<5); 	//���RXNE��־λ,������ģ���ʱ���������ŵĵ�ƽԭ����յ�һ��0x00�ֽڣ�����ֽ���û�õ�����

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
				USART2->SR &= ~(1<<5);	//�����־λ
				count = 0;	//��������
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


// 	RF_CS = 1;	//�ر�����ģ��

	delay_ms(1);	//��ʱһ��ȴ��������ݷ�������

	//��������������Ϊ����̬���ر�GPIOA��ʱ�ӣ����͹��ģ������ֲ���˵���ǽ������ģʽ֮��
	//���Ż��Զ��������̬�����ǲ�����ʱ����PA3����Ϊ0V��PA2Ϊ3.3V����֪��Ϊʲô��������
	//ģ��������ߵ�ʱ��TXD��RXD���Ŷ�Ϊ3.3V��PA3����Ϊ0V�����Ǹ���̬Ҳ��������ĵ͵�ƽ,
	//����֮����PA3�ĵ͵�ƽӦ����������GND�����Ǹ���̬�������ᵼ��PA3������е���������
	//����֮ǰ�ֶ�����������Ϊ����̬��������������ˣ�����û�е����ˡ�
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

	Timer_Switch(DELAY_TIM,Off);	//�رն�ʱ��

	//�����������ó�ģ������ģʽ�������������ŵ�©������
	GPIOA->CRL = 0x00000000;
	GPIOA->CRH = 0x00000000;
	GPIOB->CRL = 0x00000000;
	GPIOB->CRH = 0x00000000;
	GPIOC->CRL = 0x00000000;
	GPIOC->CRH = 0x00000000;


	//---------------------------------------����---------------------------------------
	//1��ʹ��JTAG���������֮����������Ƚϴ󣬲�֪��Ϊʲô���ϵ���һ�ε�֮�󹦺ı�����
	//��λ���У�ֻ�ܶϵ��ϵ繦�Ĳ��ܱ������

	//2����һ���ϵ��ʱ��ɶ���receive timeout��Ҳ��֪��Ϊʲô����������Ϊ���ϵ��ʱ������
	//ģ��ĳ�ʼ��ʱ��ϳ�����CS��ʧ�ܱ��ʹ�ܵ�ʱ���ʼ��ʱ��϶�һЩ��


// 	PWR->CSR |= 1 << 8;	//ʹ��WKUP���ţ�WKUP�����ص�ʱ��Ӵ���ģʽ�ָ�
	PWR_WakeUpPinCmd(ENABLE);


// 	IWDG_Enable();
// 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
// 	IWDG_SetPrescaler(IWDG_Prescaler_256);
// 	IWDG_SetReload(156 * WORK_CYCLE);
// 	IWDG_ReloadCounter();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);

	AFIO->MAPR &= ~(7 << 24);
	AFIO->MAPR |= (2 << 24);	//010���ر�JTAG-DP������SW-DP��
// 	DBGMCU_Config(DBGMCU_STANDBY,Enable);	//Keep debugger connection during STANDBY mode

// 	printf("\r\nBefore PWR_EnterSTANDBYMode\r\n");
	PWR_EnterSTANDBYMode();	//����standby�͹���ģʽ������standbyģʽ֮��SWD����ֱ�����أ���Ҫ��λ֮���������
// 	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);	//����stop�͹���ģʽ
// 	printf("After PWR_EnterSTANDBYMode\r\n");

	while(1)
	{
	}
}






















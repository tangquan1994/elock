#include  "sys.h"
#include "stm32f10x_rcc.h"

#if 1
	#pragma import(__use_no_semihosting)             
	//��׼����Ҫ��֧�ֺ���                 
	struct __FILE 
	{ 
		int handle; 
		/* Whatever you require here. If the only file you are using is */ 
		/* standard output using printf() for debugging, no file handling */ 
		/* is required. */ 
	}; 
	/* FILE is typedef�� d in stdio.h. */ 
	FILE __stdout;
	FILE __stdin;
	//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
	_sys_exit(int x) 
	{ 
		x = x; 
	} 
	//�ض���fputc���� 
	int fputc(int ch, FILE *f)
	{
		UartSendByte(SYS_PORT,(u8)ch);
		return ch;
	}
	//�ض���fgetc���� 
	int fgetc(FILE *f)
	{
		return UartGetByte(SYS_PORT);
	}
#endif  

#if defined( DEBUG )
	const char ASSERT_FAILED_STR[] = "Error in %s at line %d\r\n";
	void assert_failed(char *file, int line)
	{
		while (1)
		{
			printf(ASSERT_FAILED_STR, file, line); 
			delay_ms(1000);
		}
	}
#endif

void iRCC_DeInit(void)
{	
	RCC->APB1RSTR =  0x00000000;	//��λ����			 
	RCC->APB2RSTR =  0x00000000; 
	  
	RCC->AHBENR 	=  0x00000014;  //˯��ģʽ�����SRAMʱ��ʹ��.�����ر�.	  
	RCC->APB2ENR 	=  0x00000000; 	//����ʱ�ӹر�.			   
	RCC->APB1ENR 	=	 0x00000000;   
	RCC->CR 			|= 0x00000001; 	//ʹ���ڲ�����ʱ��HSION	 															 
	RCC->CFGR 		&= 0xF8FF0000; 	//��λSW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]					 
	RCC->CR 			&= 0xFEF6FFFF; 	//��λHSEON,CSSON,PLLON
	RCC->CR 			&= 0xFFFBFFFF; 	//��λHSEBYP	   	  
	RCC->CFGR 		&= 0xFF80FFFF; 	//��λPLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE 
	RCC->CIR 			=	 0x00000000;	//�ر������ж�		 
	#ifdef  VECT_TAB_RAM
		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
	#else   
		NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x0);
	#endif
}

//JTAGģʽ����,��������JTAG��ģʽ
//mode:jtag,swdģʽ����;00,ȫʹ��;01,ʹ��SWD;10,ȫ�ر�;
//CHECK OK	
//100818		  
void JTAG_Set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //��������ʱ��	   
	AFIO->MAPR&=0XF8FFFFFF; //���MAPR��[26:24]
	AFIO->MAPR|=temp;       //����jtagģʽ
} 

//ϵͳʱ�ӳ�ʼ������
//pll:��Ƶϵ��,2~16,15��16�ı�Ƶϵ����һ����Ϊ16��Ƶ		 
void MCU_INIT(u8 PLL)
{
	RCC_DeInit();
	
	RCC_HSEConfig(RCC_HSE_OFF);
	RCC_LSEConfig(RCC_LSE_OFF);
	RCC_HSICmd(ENABLE);
// 	RCC_PLLCmd(DISABLE);
// 	RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_2);
// 	RCC_PLLCmd(ENABLE);
	
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != SET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	
	RCC_LSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);
	
	
	

// 	RCC->CR 		|= (u32)1<<16;								//�ⲿ����ʱ��ʱ��
// 	RCC->CR			&= ~((u32)1<<24);							//PLL��ʱ�ر�
// 	while(!(RCC->CR&((u32)1<<17)));						//�ȴ��ⲿ����ʱ��HSE����
// 	RCC->CFGR		&= ~((u32)0x3FF<<4);					//Ԥ��Ƶλ����							
// 	RCC->CFGR		|= (u32)0<<11;								//APB2ΪAHB����ʱ��,����Ƶ
// 	RCC->CFGR		|= (u32)4<<8;									//APB1ΪAHB����ʱ�Ӷ���Ƶ
// 	RCC->CFGR		|= (u32)0<<4;									//AHBΪSYSCLK����ʱ��,����Ƶ
// 	PLL 				-= 2;													//����2����λ,��Ϊ0�ͱ�ʾ2��Ƶ,��pll��ʾpll-2��Ƶ
// 	if(PLL > 0x0f) PLL = 0x0f;								//PLL��Χ���ƣ�����Ӱ������λ
// 	RCC->CFGR		|= (u32)PLL<<18; 		  				//����PLLֵ2~16��Ƶ���
// 	RCC->CFGR		|= (u32)1<<16;	   	 					//HSEʱ����ΪPLL����ʱ��
// 	FLASH->ACR	|= 0x32;	    								//FLASH 2����ʱ����
// 	RCC->CR			|= (u32)1<<24;								//PLL��
// 	while(!(RCC->CR&((u32)1<<25)));						//�ȴ�PLLʱ�Ӿ���
// 	RCC->CFGR		&= ~((u32)3<<0);							//ϵͳʱ���л�λ���� 
// 	RCC->CFGR		|= (u32)2<<0;									//����PLL��Ϊϵͳʱ��
// 	while((RCC->CFGR&((u32)3<<2)) != 0x08);		//�ȴ�PLL��Ϊϵͳʱ�����óɹ�

	//�������JTAG����Ϊʧ�ܵĻ���������ŵĵ�ƽ״̬��ODR�Ĵ����ĸ�ֵ����ͬ
	//��Ϊ�ϵ縴λ��ʼ��Ĭ����JTAGģʽ,������ű����ó�JTAG����
	JTAG_Set(JTAG_SWD_ENABLE);
// 	JTAG_Set(JTAG_SWD_DISABLE);
	
	//set group
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//2 bits for pre-emption priority,2 bits for subpriority
}

//void NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)
//{
//	//���ڱ�ʶ����������CODE��������RAM��
//	SCB->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);//����NVIC��������ƫ�ƼĴ���
//}

//NVIC_SubPriority��NVIC_PreemptionPriority��ԭ����,��ֵԽС,Խ����	   
//void NVIC_PriorityGroupConfig(u32 NVIC_PriorityGroup)
//{
//	if(IS_NVIC_PRIORITY_GROUP(NVIC_PriorityGroup))
//	{
//		SCB->AIRCR &= ~((u32)7<<8);						//bit[10:8]������
//		SCB->AIRCR |= NVIC_PriorityGroup;			//�����鸳��bit[10:8]
//	}
//}

void iNVIC_Init(u8 NVIC_IRQChannel,u8 NVIC_IRQChannelPreemptionPriority,u8 NVIC_IRQChannelSubPriority,u8 NVIC_IRQChannelCmd)
{
	u32 Priority = 0;
	if(NVIC_IRQChannelCmd == Enable)
	{
		//enable interrupt
		NVIC->ISER[NVIC_IRQChannel/32] |= (u32)0x01<<(NVIC_IRQChannel%32);
		//compound priority
		Priority = (u32)((NVIC_IRQChannelPreemptionPriority)<<2 | \
										  NVIC_IRQChannelSubPriority);
		NVIC->IPR[NVIC_IRQChannel/4] 	|= Priority<<(((u32)NVIC_IRQChannel%4)*8+4);		//ӦΪ�õ��Ǹ���λ,���Ի�Ҫ����4����
	}
	else
	{
		//disenable interrupt
		NVIC->ISER[NVIC_IRQChannel/32] &= ~((u32)0x01<<(NVIC_IRQChannel%32));
	}
}

// FTIR	�½��ش���
// RTIR	�����ش���
// Enable ʹ��,Disable ʧ��
void iEXTI_Init(GPIO_TypeDef* PORT,u8 Line,u8 Trigger,u8 Cmd)
{
	u8 temp;
	//��GPIO�˿��м�����ж϶˿ں�,����ָ����AFIO_EXTICRx�Ĵ�����
	temp = (u8)((u32)((u8*)PORT-(u8*)GPIOA_BASE)>>10);
	RCC->APB2ENR |= (u32)0x01;		//��AFIOʱ��
	AFIO->EXTICR[Line/4] &= ~((u16)0x0F<<(Line%4*4));		//��λ
	AFIO->EXTICR[Line/4] |= (u16)temp<<(Line%4*4);			//�˿��ж�
	EXTI->IMR 	&= 	~((u32)1<<Line);										//�ⲿ�ж�����λ���
	EXTI->IMR 	|= 	(u32)((u8)Cmd)<<Line;								//�ⲿ�ж����ν�ֹ
	if(Trigger == RTIR)
		EXTI->RTSR 	|= 	(u32)1<<Line;			//�����ش���
	else
		EXTI->FTSR 	|= 	(u32)1<<Line;			//�½��ش���
}

//��ͬ�����ϵ����費��ͬʱ��ʼ��
void RCC_SW(u32 Peripheral,u8 ON_OFF)
{
	u8 Skewing = (Peripheral&0xF0000000)>>28;		//��ȡ��RCC�ṹ���ڲ�ƫ����,���������ߵ�4λ��
	if(ON_OFF!=0)
		*((u32*)RP+Skewing)	|= Peripheral;
	else
		*((u32*)RP+Skewing)	&= ~Peripheral;
}

//ϵͳ�δ�ʱ��SysTick��ʼ��
void SysTick_Init(void)
{
	//SysTick��ʱ�ӻ���Ҫ��һ��,��Ϊ��ʱ�������жԼ���ֵ���ж�,
	//̫��Ļ������Բ�׽����ֵ,������ʱ�����.
 	SysTick->CTRL	&= ~(1<<2);		//0=HCLK/8;1=HCLK HCLK:72MHz
 	SysTick->CTRL	|= (1<<1);		//SYSTICK�ж�ʹ��
	SysTick->LOAD	= 9000;
	SysTick->VAL	= 0;
}












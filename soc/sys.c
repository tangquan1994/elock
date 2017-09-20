#include  "sys.h"
#include "stm32f10x_rcc.h"

#if 1
	#pragma import(__use_no_semihosting)             
	//标准库需要的支持函数                 
	struct __FILE 
	{ 
		int handle; 
		/* Whatever you require here. If the only file you are using is */ 
		/* standard output using printf() for debugging, no file handling */ 
		/* is required. */ 
	}; 
	/* FILE is typedef’ d in stdio.h. */ 
	FILE __stdout;
	FILE __stdin;
	//定义_sys_exit()以避免使用半主机模式    
	_sys_exit(int x) 
	{ 
		x = x; 
	} 
	//重定义fputc函数 
	int fputc(int ch, FILE *f)
	{
		UartSendByte(SYS_PORT,(u8)ch);
		return ch;
	}
	//重定义fgetc函数 
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
	RCC->APB1RSTR =  0x00000000;	//复位结束			 
	RCC->APB2RSTR =  0x00000000; 
	  
	RCC->AHBENR 	=  0x00000014;  //睡眠模式闪存和SRAM时钟使能.其他关闭.	  
	RCC->APB2ENR 	=  0x00000000; 	//外设时钟关闭.			   
	RCC->APB1ENR 	=	 0x00000000;   
	RCC->CR 			|= 0x00000001; 	//使能内部高速时钟HSION	 															 
	RCC->CFGR 		&= 0xF8FF0000; 	//复位SW[1:0],HPRE[3:0],PPRE1[2:0],PPRE2[2:0],ADCPRE[1:0],MCO[2:0]					 
	RCC->CR 			&= 0xFEF6FFFF; 	//复位HSEON,CSSON,PLLON
	RCC->CR 			&= 0xFFFBFFFF; 	//复位HSEBYP	   	  
	RCC->CFGR 		&= 0xFF80FFFF; 	//复位PLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE 
	RCC->CIR 			=	 0x00000000;	//关闭所有中断		 
	#ifdef  VECT_TAB_RAM
		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
	#else   
		NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x0);
	#endif
}

//JTAG模式设置,用于设置JTAG的模式
//mode:jtag,swd模式设置;00,全使能;01,使能SWD;10,全关闭;
//CHECK OK	
//100818		  
void JTAG_Set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
	AFIO->MAPR&=0XF8FFFFFF; //清除MAPR的[26:24]
	AFIO->MAPR|=temp;       //设置jtag模式
} 

//系统时钟初始化函数
//pll:倍频系数,2~16,15和16的倍频系数是一样的为16倍频		 
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
	
	
	

// 	RCC->CR 		|= (u32)1<<16;								//外部高速时钟时能
// 	RCC->CR			&= ~((u32)1<<24);							//PLL暂时关闭
// 	while(!(RCC->CR&((u32)1<<17)));						//等待外部高速时钟HSE就绪
// 	RCC->CFGR		&= ~((u32)0x3FF<<4);					//预分频位清零							
// 	RCC->CFGR		|= (u32)0<<11;								//APB2为AHB总线时钟,不分频
// 	RCC->CFGR		|= (u32)4<<8;									//APB1为AHB总线时钟二分频
// 	RCC->CFGR		|= (u32)0<<4;									//AHB为SYSCLK总线时钟,不分频
// 	PLL 				-= 2;													//抵消2个单位,因为0就表示2倍频,故pll表示pll-2倍频
// 	if(PLL > 0x0f) PLL = 0x0f;								//PLL范围限制，不能影响其它位
// 	RCC->CFGR		|= (u32)PLL<<18; 		  				//设置PLL值2~16倍频输出
// 	RCC->CFGR		|= (u32)1<<16;	   	 					//HSE时钟作为PLL输入时钟
// 	FLASH->ACR	|= 0x32;	    								//FLASH 2个延时周期
// 	RCC->CR			|= (u32)1<<24;								//PLL打开
// 	while(!(RCC->CR&((u32)1<<25)));						//等待PLL时钟就绪
// 	RCC->CFGR		&= ~((u32)3<<0);							//系统时钟切换位清零 
// 	RCC->CFGR		|= (u32)2<<0;									//设置PLL作为系统时钟
// 	while((RCC->CFGR&((u32)3<<2)) != 0x08);		//等待PLL作为系统时钟设置成功

	//如果不将JTAG设置为失能的话有许多引脚的电平状态与ODR寄存器的赋值不相同
	//因为上电复位初始化默认是JTAG模式,许多引脚被复用成JTAG引脚
	JTAG_Set(JTAG_SWD_ENABLE);
// 	JTAG_Set(JTAG_SWD_DISABLE);
	
	//set group
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//2 bits for pre-emption priority,2 bits for subpriority
}

//void NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)
//{
//	//用于标识向量表是在CODE区还是在RAM区
//	SCB->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);//设置NVIC的向量表偏移寄存器
//}

//NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先	   
//void NVIC_PriorityGroupConfig(u32 NVIC_PriorityGroup)
//{
//	if(IS_NVIC_PRIORITY_GROUP(NVIC_PriorityGroup))
//	{
//		SCB->AIRCR &= ~((u32)7<<8);						//bit[10:8]先清零
//		SCB->AIRCR |= NVIC_PriorityGroup;			//将分组赋给bit[10:8]
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
		NVIC->IPR[NVIC_IRQChannel/4] 	|= Priority<<(((u32)NVIC_IRQChannel%4)*8+4);		//应为用的是高四位,所以还要加上4才行
	}
	else
	{
		//disenable interrupt
		NVIC->ISER[NVIC_IRQChannel/32] &= ~((u32)0x01<<(NVIC_IRQChannel%32));
	}
}

// FTIR	下降沿触发
// RTIR	上升沿触发
// Enable 使能,Disable 失能
void iEXTI_Init(GPIO_TypeDef* PORT,u8 Line,u8 Trigger,u8 Cmd)
{
	u8 temp;
	//从GPIO端口中计算出中断端口号,放入指定的AFIO_EXTICRx寄存器中
	temp = (u8)((u32)((u8*)PORT-(u8*)GPIOA_BASE)>>10);
	RCC->APB2ENR |= (u32)0x01;		//打开AFIO时钟
	AFIO->EXTICR[Line/4] &= ~((u16)0x0F<<(Line%4*4));		//复位
	AFIO->EXTICR[Line/4] |= (u16)temp<<(Line%4*4);			//端口中断
	EXTI->IMR 	&= 	~((u32)1<<Line);										//外部中断屏蔽位清除
	EXTI->IMR 	|= 	(u32)((u8)Cmd)<<Line;								//外部中断屏蔽禁止
	if(Trigger == RTIR)
		EXTI->RTSR 	|= 	(u32)1<<Line;			//上升沿触发
	else
		EXTI->FTSR 	|= 	(u32)1<<Line;			//下降沿触发
}

//不同总线上的外设不能同时初始化
void RCC_SW(u32 Peripheral,u8 ON_OFF)
{
	u8 Skewing = (Peripheral&0xF0000000)>>28;		//提取出RCC结构体内部偏移量,存放在在最高的4位中
	if(ON_OFF!=0)
		*((u32*)RP+Skewing)	|= Peripheral;
	else
		*((u32*)RP+Skewing)	&= ~Peripheral;
}

//系统滴答定时器SysTick初始化
void SysTick_Init(void)
{
	//SysTick的时钟还是要慢一点,因为延时函数中有对计数值的判断,
	//太快的话会难以捕捉计数值,导致延时误差变大.
 	SysTick->CTRL	&= ~(1<<2);		//0=HCLK/8;1=HCLK HCLK:72MHz
 	SysTick->CTRL	|= (1<<1);		//SYSTICK中断使能
	SysTick->LOAD	= 9000;
	SysTick->VAL	= 0;
}












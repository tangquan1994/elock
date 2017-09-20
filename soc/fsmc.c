#include "FSMC.h"

//BANK1的区域4作为LCD的寻址空间
void FSMC_LCD_Init(void)
{
	RCC_SW(AHB_FSMC,On);
	RCC_SW(APB2_AFIO|APB2_GPIOD|APB2_GPIOE|APB2_GPIOF|APB2_GPIOG,On);
	
  /* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
     PD.10(D15), PD.14(D0), PD.15(D1) as alternate 
     function push pull */
	iGPIO_Init(GPIOD,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
									GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
									GPIO_Pin_15,iGPIO_Mode_AF_PP,iGPIO_Speed_50MHz);
  /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
	iGPIO_Init(GPIOE,GPIO_Pin_7 | GPIO_Pin_HIGH,iGPIO_Mode_AF_PP,iGPIO_Speed_50MHz);
	
  /* Set PF.00(A0 (RS)) as alternate function push pull */
	iGPIO_Init(GPIOF,GPIO_Pin_0,iGPIO_Mode_AF_PP,iGPIO_Speed_50MHz);
	
  /* Set PG.12(NE4 (LCD/CS)) as alternate function push pull - CE3(LCD /CS) */
	iGPIO_Init(GPIOG,GPIO_Pin_12,iGPIO_Mode_AF_PP,iGPIO_Speed_50MHz);
	
	//寄存器清零
	//bank1有NE1~4,每一个有一个BCR+TCR,所以总共八个寄存器。
	//这里我使用NE4,也就对应BTCR[6],[7]。				    
	FSMC_Bank1->BTCR[6]=0X00000000;
	FSMC_Bank1->BTCR[7]=0X00000000;
	FSMC_Bank1E->BWTR[6]=0X00000000;
	//操作BCR寄存器	使用异步模式
	FSMC_Bank1->BTCR[6]|=1<<12;		//存储器写使能
	FSMC_Bank1->BTCR[6]|=1<<14;		//读写使用不同的时序
	FSMC_Bank1->BTCR[6]|=1<<4; 		//存储器数据宽度为16bit 	    
	//操作BTR寄存器	
	//读时序控制寄存器 							    
	FSMC_Bank1->BTCR[7]|=0<<28;		//模式A 	 							  	 
	FSMC_Bank1->BTCR[7]|=1<<0; 		//地址建立时间（ADDSET）为2个HCLK 1/36M=27ns	 	 
	//因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
	FSMC_Bank1->BTCR[7]|=0XF<<8;  	//数据保存时间为16个HCLK	 	 
	//写时序控制寄存器  
	FSMC_Bank1E->BWTR[6]|=0<<28; 	//模式A 	 							    
	FSMC_Bank1E->BWTR[6]|=0<<0;		//地址建立时间（ADDSET）为1个HCLK 
 	//4个HCLK（HCLK=72M）因为液晶驱动IC的写信号脉宽，最少也得50ns。72M/4=24M=55ns  	 
	FSMC_Bank1E->BWTR[6]|=3<<8; 	//数据保存时间为4个HCLK	
	//使能BANK1,区域4
	FSMC_Bank1->BTCR[6]|=1<<0;		//使能BANK1,区域4
			 
}

//BANK1的区域3作为SRAM的寻址空间
void FSMC_SRAM_Init(void)
{
	RCC->AHBENR|=1<<8;       //使能FSMC时钟	  
	RCC->APB2ENR|=1<<5;      //使能PORTD时钟
	RCC->APB2ENR|=1<<6;      //使能PORTE时钟
 	RCC->APB2ENR|=1<<7;      //使能PORTF时钟
	RCC->APB2ENR|=1<<8;      //使能PORTG时钟	   

	//PORTD复用推挽输出 	
	GPIOD->CRH&=0X00000000;
	GPIOD->CRH|=0XBBBBBBBB; 
	GPIOD->CRL&=0XFF00FF00;
	GPIOD->CRL|=0X00BB00BB;   	 
	//PORTE复用推挽输出 	
	GPIOE->CRH&=0X00000000;
	GPIOE->CRH|=0XBBBBBBBB; 
	GPIOE->CRL&=0X0FFFFF00;
	GPIOE->CRL|=0XB00000BB; 
	//PORTF复用推挽输出
	GPIOF->CRH&=0X0000FFFF;
	GPIOF->CRH|=0XBBBB0000;  	    	 											 
	GPIOF->CRL&=0XFF000000;
	GPIOF->CRL|=0X00BBBBBB; 

	//PORTG复用推挽输出 PG10->NE3     	 											 
	GPIOG->CRH&=0XFFFFF0FF;
	GPIOG->CRH|=0X00000B00;  
 	GPIOG->CRL&=0XFF000000;
	GPIOG->CRL|=0X00BBBBBB;
		
	//寄存器清零
	//bank1有NE1~4,每一个有一个BCR+TCR，所以总共八个寄存器。
	//这里我们使用NE3 ，也就对应BTCR[4],[5]。
	FSMC_Bank1->BTCR[4]=0X00000000;
	FSMC_Bank1->BTCR[5]=0X00000000;
	FSMC_Bank1E->BWTR[4]=0X00000000;
	//操作BCR寄存器	使用异步模式,模式A(读写共用一个时序寄存器)
	//BTCR[偶数]:BCR寄存器;BTCR[奇数]:BTR寄存器
	FSMC_Bank1->BTCR[4]|=1<<12;//存储器写使能
	FSMC_Bank1->BTCR[4]|=1<<4; //存储器数据宽度为16bit 	    
	//操作BTR寄存器								    
	FSMC_Bank1->BTCR[5]|=3<<8; //数据保持时间（DATAST）为3个HCLK 4/72M=55ns(对EM的SRAM芯片)	 	 
	FSMC_Bank1->BTCR[5]|=0<<4; //地址保持时间（ADDHLD）未用到	  	 
	FSMC_Bank1->BTCR[5]|=0<<0; //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns	 	 
	//闪存写时序寄存器  
	FSMC_Bank1E->BWTR[4]=0x0FFFFFFF;//默认值
	//使能BANK1区域3
	FSMC_Bank1->BTCR[4]|=1<<0; 												
}


//整个BANK2都做为NAND Flash控制器的寻址空间,注意NAND Flash的寻址空间和系统的寻址空间不重合,
//也就是说NAND Flash中的地址和STM32内部地址是不一样的,而SRAM和NOR flash则是挂载在STM32寻址空间上的
void FSMC_NAND_Init(void)
{
	RCC->AHBENR|=1<<8;       //使能FSMC时钟	  
	RCC->APB2ENR|=1<<5;      //使能PORTD时钟
	RCC->APB2ENR|=1<<6;      //使能PORTE时钟
 	RCC->APB2ENR|=1<<7;      //使能PORTF时钟
	RCC->APB2ENR|=1<<8;      //使能PORTG时钟

    //PORTD复用推挽输出 	
	GPIOD->CRH&=0X00000000;
	GPIOD->CRH|=0XBBBBBBBB; 
	GPIOD->CRL&=0X0000FF00;
	GPIOD->CRL|=0XB4BB00BB;   	 
	//PORTE复用推挽输出 	
	GPIOE->CRH&=0X00000000;
	GPIOE->CRH|=0XBBBBBBBB; 
	GPIOE->CRL&=0X0FFFFF00;
	GPIOE->CRL|=0XB00000BB; 
	//PORTF复用推挽输出
	GPIOF->CRH&=0X0000FFFF;
	GPIOF->CRH|=0XBBBB0000;  	    	 											 
	GPIOF->CRL&=0XFF000000;
	GPIOF->CRL|=0X00BBBBBB; 

	//PORTG复用推挽输出 PG10->NE3     	 											 
	GPIOG->CRH&=0XFFFFF0FF;
	GPIOG->CRH|=0X00000B00;  
 	GPIOG->CRL&=0XFF000000;
	GPIOG->CRL|=0X00BBBBBB;  

	FSMC_Bank2->PCR2=0x0000000e;
	FSMC_Bank2->SR2=0x00000000;
	FSMC_Bank2->PMEM2=0x01010101;
	FSMC_Bank2->PATT2=0x01010101;
}

void FSMC_NAND_ReadID(NAND_IDTypeDef* NAND_ID)
{	
	NAND_CMD=0x90;
	NAND_ADR=0x00;
	delay_ms(10);
	NAND_ID->Maker_ID   = NAND_DAT;
	NAND_ID->Device_ID  = NAND_DAT;
	NAND_ID->Third_ID   = NAND_DAT;
	NAND_ID->Fourth_ID  = NAND_DAT;
}


//BANK1的区域2作为NOR Flash的寻址空间
void FSMC_NOR_Init(void)
{
	RCC->AHBENR|=1<<8;       //使能FSMC时钟	  
	RCC->APB2ENR|=1<<5;      //使能PORTD时钟
	RCC->APB2ENR|=1<<6;      //使能PORTE时钟
 	RCC->APB2ENR|=1<<7;      //使能PORTF时钟
	RCC->APB2ENR|=1<<8;      //使能PORTG时钟	   

	//PORTD复用推挽输出 	
	GPIOD->CRH&=0X00000000;
	GPIOD->CRH|=0XBBBBBBBB; 
	GPIOD->CRL&=0XFF00FF00;
	GPIOD->CRL|=0X00BB00BB;   	 
	//PORTE复用推挽输出 	
	GPIOE->CRH&=0X00000000;
	GPIOE->CRH|=0XBBBBBBBB; 
	GPIOE->CRL&=0X0FFFFF00;
	GPIOE->CRL|=0XB00000BB; 
	//PORTF复用推挽输出
	GPIOF->CRH&=0X0000FFFF;
	GPIOF->CRH|=0XBBBB0000;  	    	 											 
	GPIOF->CRL&=0XFF000000;
	GPIOF->CRL|=0X00BBBBBB; 

	//PORTG复用推挽输出 PG10->NE3     	 											 
	GPIOG->CRH&=0XFFFFF00F;
	GPIOG->CRH|=0X00000BB0;  
 	GPIOG->CRL&=0XFF000000;
	GPIOG->CRL|=0X00BBBBBB;  	 				  
  
	//寄存器清零
	//bank1有NE1~4,每一个有一个BCR+TCR，所以总共八个寄存器。
	//这里我们使用NE3 ，也就对应BTCR[4],[5]。				    
	FSMC_Bank1->BTCR[2]=0X00000000;
	FSMC_Bank1->BTCR[3]=0X00000000;
	FSMC_Bank1E->BWTR[1]=0X00000000;
	//操作BCR寄存器	使用异步模式,模式A(读写共用一个时序寄存器)
	//BTCR[偶数]:BCR寄存器;BTCR[奇数]:BTR寄存器
	FSMC_Bank1->BTCR[2]|=1<<14;//存储器写使能
	FSMC_Bank1->BTCR[2]|=1<<12;//存储器写使能
	FSMC_Bank1->BTCR[2]|=1<<6;
	FSMC_Bank1->BTCR[2]|=1<<4; //存储器数据宽度为16bit 
	FSMC_Bank1->BTCR[2]|=1<<3;	    
	//操作BTR寄存器								    
	FSMC_Bank1->BTCR[3]|=1<<28;
	FSMC_Bank1->BTCR[3]|=7<<8; //数据保持时间（DATAST）为3个HCLK 4/72M=55ns(对EM的SRAM芯片)	 	 
	FSMC_Bank1->BTCR[3]|=0<<4; //地址保持时间（ADDHLD）未用到	  	 
	FSMC_Bank1->BTCR[3]|=5<<0; //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns	 	 
	//闪存写时序寄存器  
	FSMC_Bank1E->BWTR[2]|=1<<28;
	FSMC_Bank1E->BWTR[2]|=7<<8;
	FSMC_Bank1E->BWTR[2]|=0<<4;
	FSMC_Bank1E->BWTR[2]|=5<<0;
	//使能BANK1区域3
	FSMC_Bank1->BTCR[2]|=1<<0; 												
}

void FSMC_NOR_ReadID(NOR_IDTypeDef* NOR_ID)
{
  NOR_WRITE(ADDR_SHIFT(0x05555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT(0x02AAA), 0x0055);
  NOR_WRITE(ADDR_SHIFT(0x05555), 0x0090);

  NOR_ID->Manufacturer_Code = *(vu16 *) ADDR_SHIFT(0x0000);
  NOR_ID->Device_Code1 = *(vu16 *) ADDR_SHIFT(0x0001);
  NOR_ID->Device_Code2 = *(vu16 *) ADDR_SHIFT(0x000E);
  NOR_ID->Device_Code3 = *(vu16 *) ADDR_SHIFT(0x000F);
}




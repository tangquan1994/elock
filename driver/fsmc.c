#include "FSMC.h"

//BANK1������4��ΪLCD��Ѱַ�ռ�
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
	
	//�Ĵ�������
	//bank1��NE1~4,ÿһ����һ��BCR+TCR,�����ܹ��˸��Ĵ�����
	//������ʹ��NE4,Ҳ�Ͷ�ӦBTCR[6],[7]��				    
	FSMC_Bank1->BTCR[6]=0X00000000;
	FSMC_Bank1->BTCR[7]=0X00000000;
	FSMC_Bank1E->BWTR[6]=0X00000000;
	//����BCR�Ĵ���	ʹ���첽ģʽ
	FSMC_Bank1->BTCR[6]|=1<<12;		//�洢��дʹ��
	FSMC_Bank1->BTCR[6]|=1<<14;		//��дʹ�ò�ͬ��ʱ��
	FSMC_Bank1->BTCR[6]|=1<<4; 		//�洢�����ݿ��Ϊ16bit 	    
	//����BTR�Ĵ���	
	//��ʱ����ƼĴ��� 							    
	FSMC_Bank1->BTCR[7]|=0<<28;		//ģʽA 	 							  	 
	FSMC_Bank1->BTCR[7]|=1<<0; 		//��ַ����ʱ�䣨ADDSET��Ϊ2��HCLK 1/36M=27ns	 	 
	//��ΪҺ������IC�Ķ����ݵ�ʱ���ٶȲ���̫�죬�����1289���IC��
	FSMC_Bank1->BTCR[7]|=0XF<<8;  	//���ݱ���ʱ��Ϊ16��HCLK	 	 
	//дʱ����ƼĴ���  
	FSMC_Bank1E->BWTR[6]|=0<<28; 	//ģʽA 	 							    
	FSMC_Bank1E->BWTR[6]|=0<<0;		//��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 
 	//4��HCLK��HCLK=72M����ΪҺ������IC��д�ź���������Ҳ��50ns��72M/4=24M=55ns  	 
	FSMC_Bank1E->BWTR[6]|=3<<8; 	//���ݱ���ʱ��Ϊ4��HCLK	
	//ʹ��BANK1,����4
	FSMC_Bank1->BTCR[6]|=1<<0;		//ʹ��BANK1,����4
			 
}

//BANK1������3��ΪSRAM��Ѱַ�ռ�
void FSMC_SRAM_Init(void)
{
	RCC->AHBENR|=1<<8;       //ʹ��FSMCʱ��	  
	RCC->APB2ENR|=1<<5;      //ʹ��PORTDʱ��
	RCC->APB2ENR|=1<<6;      //ʹ��PORTEʱ��
 	RCC->APB2ENR|=1<<7;      //ʹ��PORTFʱ��
	RCC->APB2ENR|=1<<8;      //ʹ��PORTGʱ��	   

	//PORTD����������� 	
	GPIOD->CRH&=0X00000000;
	GPIOD->CRH|=0XBBBBBBBB; 
	GPIOD->CRL&=0XFF00FF00;
	GPIOD->CRL|=0X00BB00BB;   	 
	//PORTE����������� 	
	GPIOE->CRH&=0X00000000;
	GPIOE->CRH|=0XBBBBBBBB; 
	GPIOE->CRL&=0X0FFFFF00;
	GPIOE->CRL|=0XB00000BB; 
	//PORTF�����������
	GPIOF->CRH&=0X0000FFFF;
	GPIOF->CRH|=0XBBBB0000;  	    	 											 
	GPIOF->CRL&=0XFF000000;
	GPIOF->CRL|=0X00BBBBBB; 

	//PORTG����������� PG10->NE3     	 											 
	GPIOG->CRH&=0XFFFFF0FF;
	GPIOG->CRH|=0X00000B00;  
 	GPIOG->CRL&=0XFF000000;
	GPIOG->CRL|=0X00BBBBBB;
		
	//�Ĵ�������
	//bank1��NE1~4,ÿһ����һ��BCR+TCR�������ܹ��˸��Ĵ�����
	//��������ʹ��NE3 ��Ҳ�Ͷ�ӦBTCR[4],[5]��
	FSMC_Bank1->BTCR[4]=0X00000000;
	FSMC_Bank1->BTCR[5]=0X00000000;
	FSMC_Bank1E->BWTR[4]=0X00000000;
	//����BCR�Ĵ���	ʹ���첽ģʽ,ģʽA(��д����һ��ʱ��Ĵ���)
	//BTCR[ż��]:BCR�Ĵ���;BTCR[����]:BTR�Ĵ���
	FSMC_Bank1->BTCR[4]|=1<<12;//�洢��дʹ��
	FSMC_Bank1->BTCR[4]|=1<<4; //�洢�����ݿ��Ϊ16bit 	    
	//����BTR�Ĵ���								    
	FSMC_Bank1->BTCR[5]|=3<<8; //���ݱ���ʱ�䣨DATAST��Ϊ3��HCLK 4/72M=55ns(��EM��SRAMоƬ)	 	 
	FSMC_Bank1->BTCR[5]|=0<<4; //��ַ����ʱ�䣨ADDHLD��δ�õ�	  	 
	FSMC_Bank1->BTCR[5]|=0<<0; //��ַ����ʱ�䣨ADDSET��Ϊ2��HCLK 1/36M=27ns	 	 
	//����дʱ��Ĵ���  
	FSMC_Bank1E->BWTR[4]=0x0FFFFFFF;//Ĭ��ֵ
	//ʹ��BANK1����3
	FSMC_Bank1->BTCR[4]|=1<<0; 												
}


//����BANK2����ΪNAND Flash��������Ѱַ�ռ�,ע��NAND Flash��Ѱַ�ռ��ϵͳ��Ѱַ�ռ䲻�غ�,
//Ҳ����˵NAND Flash�еĵ�ַ��STM32�ڲ���ַ�ǲ�һ����,��SRAM��NOR flash���ǹ�����STM32Ѱַ�ռ��ϵ�
void FSMC_NAND_Init(void)
{
	RCC->AHBENR|=1<<8;       //ʹ��FSMCʱ��	  
	RCC->APB2ENR|=1<<5;      //ʹ��PORTDʱ��
	RCC->APB2ENR|=1<<6;      //ʹ��PORTEʱ��
 	RCC->APB2ENR|=1<<7;      //ʹ��PORTFʱ��
	RCC->APB2ENR|=1<<8;      //ʹ��PORTGʱ��

    //PORTD����������� 	
	GPIOD->CRH&=0X00000000;
	GPIOD->CRH|=0XBBBBBBBB; 
	GPIOD->CRL&=0X0000FF00;
	GPIOD->CRL|=0XB4BB00BB;   	 
	//PORTE����������� 	
	GPIOE->CRH&=0X00000000;
	GPIOE->CRH|=0XBBBBBBBB; 
	GPIOE->CRL&=0X0FFFFF00;
	GPIOE->CRL|=0XB00000BB; 
	//PORTF�����������
	GPIOF->CRH&=0X0000FFFF;
	GPIOF->CRH|=0XBBBB0000;  	    	 											 
	GPIOF->CRL&=0XFF000000;
	GPIOF->CRL|=0X00BBBBBB; 

	//PORTG����������� PG10->NE3     	 											 
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


//BANK1������2��ΪNOR Flash��Ѱַ�ռ�
void FSMC_NOR_Init(void)
{
	RCC->AHBENR|=1<<8;       //ʹ��FSMCʱ��	  
	RCC->APB2ENR|=1<<5;      //ʹ��PORTDʱ��
	RCC->APB2ENR|=1<<6;      //ʹ��PORTEʱ��
 	RCC->APB2ENR|=1<<7;      //ʹ��PORTFʱ��
	RCC->APB2ENR|=1<<8;      //ʹ��PORTGʱ��	   

	//PORTD����������� 	
	GPIOD->CRH&=0X00000000;
	GPIOD->CRH|=0XBBBBBBBB; 
	GPIOD->CRL&=0XFF00FF00;
	GPIOD->CRL|=0X00BB00BB;   	 
	//PORTE����������� 	
	GPIOE->CRH&=0X00000000;
	GPIOE->CRH|=0XBBBBBBBB; 
	GPIOE->CRL&=0X0FFFFF00;
	GPIOE->CRL|=0XB00000BB; 
	//PORTF�����������
	GPIOF->CRH&=0X0000FFFF;
	GPIOF->CRH|=0XBBBB0000;  	    	 											 
	GPIOF->CRL&=0XFF000000;
	GPIOF->CRL|=0X00BBBBBB; 

	//PORTG����������� PG10->NE3     	 											 
	GPIOG->CRH&=0XFFFFF00F;
	GPIOG->CRH|=0X00000BB0;  
 	GPIOG->CRL&=0XFF000000;
	GPIOG->CRL|=0X00BBBBBB;  	 				  
  
	//�Ĵ�������
	//bank1��NE1~4,ÿһ����һ��BCR+TCR�������ܹ��˸��Ĵ�����
	//��������ʹ��NE3 ��Ҳ�Ͷ�ӦBTCR[4],[5]��				    
	FSMC_Bank1->BTCR[2]=0X00000000;
	FSMC_Bank1->BTCR[3]=0X00000000;
	FSMC_Bank1E->BWTR[1]=0X00000000;
	//����BCR�Ĵ���	ʹ���첽ģʽ,ģʽA(��д����һ��ʱ��Ĵ���)
	//BTCR[ż��]:BCR�Ĵ���;BTCR[����]:BTR�Ĵ���
	FSMC_Bank1->BTCR[2]|=1<<14;//�洢��дʹ��
	FSMC_Bank1->BTCR[2]|=1<<12;//�洢��дʹ��
	FSMC_Bank1->BTCR[2]|=1<<6;
	FSMC_Bank1->BTCR[2]|=1<<4; //�洢�����ݿ��Ϊ16bit 
	FSMC_Bank1->BTCR[2]|=1<<3;	    
	//����BTR�Ĵ���								    
	FSMC_Bank1->BTCR[3]|=1<<28;
	FSMC_Bank1->BTCR[3]|=7<<8; //���ݱ���ʱ�䣨DATAST��Ϊ3��HCLK 4/72M=55ns(��EM��SRAMоƬ)	 	 
	FSMC_Bank1->BTCR[3]|=0<<4; //��ַ����ʱ�䣨ADDHLD��δ�õ�	  	 
	FSMC_Bank1->BTCR[3]|=5<<0; //��ַ����ʱ�䣨ADDSET��Ϊ2��HCLK 1/36M=27ns	 	 
	//����дʱ��Ĵ���  
	FSMC_Bank1E->BWTR[2]|=1<<28;
	FSMC_Bank1E->BWTR[2]|=7<<8;
	FSMC_Bank1E->BWTR[2]|=0<<4;
	FSMC_Bank1E->BWTR[2]|=5<<0;
	//ʹ��BANK1����3
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




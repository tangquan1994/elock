#include "R61505U.h"

u16 DeviceCode;

void LCD_Init(void)
{
	u16 i;
	//背光控制脚A1
	RCC_SW(APB2_GPIOA,On);
// 	iGPIO_Init(GPIOA,Pin_1,iGPIO_Mode_Out_PP,iGPIO_Speed_50MHz);
	PWM_Init(TIM2,100,720);	//
	//LCD_FSMC初始化
	FSMC_LCD_Init();
	
	delay_ms(100); 	/* delay 100 ms */
	DeviceCode = LCD_ReadReg(0x0000);
	printf("DeviceCode:%d\n\r\n",DeviceCode);
	//LCD驱动型号   9325、9328、9320、9300、9331、9919、RU1505、8989、
	if(DeviceCode==0x1505)
	{
		//*********************************************Start intial Sequence	  	
		LCD_WriteReg(0x00e5,0x8000);
		LCD_WriteReg(0x0000,0x0001);
		LCD_WriteReg(0x0001,0x0100);
		LCD_WriteReg(0x0002,0x0700);
		LCD_WriteReg(0x0003,0x1030);
		LCD_WriteReg(0x0004,0x0000);
		LCD_WriteReg(0x0008,0x0202);
		LCD_WriteReg(0x0009,0x0000);
		LCD_WriteReg(0x000a,0x0000);	
		LCD_WriteReg(0x000c,0x0000);
		LCD_WriteReg(0x000d,0x0000);
		LCD_WriteReg(0x000f,0x0000);
		//*********************************************Power On
		LCD_WriteReg(0x0010,0x0000);
		LCD_WriteReg(0x0011,0x0000);
		LCD_WriteReg(0x0012,0x0000);
		LCD_WriteReg(0x0013,0x0000);
		for(i=50000;i>0;i--);
		LCD_WriteReg(0x0010,0x17b0);
		LCD_WriteReg(0x0011,0x0037);
		for(i=50000;i>0;i--);
		LCD_WriteReg(0x0012,0x0138);
		for(i=50000;i>0;i--);
		LCD_WriteReg(0x0013,0x1700);
		LCD_WriteReg(0x0029,0x000d);
		for(i=50000;i>0;i--);
		LCD_WriteReg(0x0020,0x0000);
		LCD_WriteReg(0x0021,0x0000);
		//*********************************************Set gamma
		LCD_WriteReg(0x0030,0x0001);
		LCD_WriteReg(0x0031,0x0606);
		LCD_WriteReg(0x0032,0x0304);
		LCD_WriteReg(0x0033,0x0202);
		LCD_WriteReg(0x0034,0x0202);
		LCD_WriteReg(0x0035,0x0103);
		LCD_WriteReg(0x0036,0x011d);
		LCD_WriteReg(0x0037,0x0404);
		LCD_WriteReg(0x0038,0x0404);
		LCD_WriteReg(0x0039,0x0404);
		LCD_WriteReg(0x003c,0x0700);
		LCD_WriteReg(0x003d,0x0a1f);
		//**********************************************Set Gram aera
		LCD_WriteReg(0x0050,0x0000);
		LCD_WriteReg(0x0051,0x00ef);
		LCD_WriteReg(0x0052,0x0000);
		LCD_WriteReg(0x0053,0x013f);
		LCD_WriteReg(0x0060,0x2700);
		LCD_WriteReg(0x0061,0x0001);
		LCD_WriteReg(0x006a,0x0000);
		//*********************************************Paratial display
		LCD_WriteReg(0x0090,0x0010);
		LCD_WriteReg(0x0092,0x0000);
		LCD_WriteReg(0x0093,0x0003);
		LCD_WriteReg(0x0095,0x0101);
		LCD_WriteReg(0x0097,0x0000);
		LCD_WriteReg(0x0098,0x0000);
		//******************************************** Plan Control
		LCD_WriteReg(0x0007,0x0021);
		for(i=50000;i>0;i--);
		LCD_WriteReg(0x0007,0x0031);
		for(i=50000;i>0;i--);
		LCD_WriteReg(0x0007,0x0173);
		for(i=50000;i>0;i--);
	}
// 	BACK_LIGHT = 1;
	Channel_X(TIM2,1,100);
	delay_ms(50);
	LCD_Clear(White);
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteReg(u8 LCD_Reg,u16 LCD_RegValue)
{
  /* Write 16-bit Index, then Write Reg */
  LCD->LCD_REG = LCD_Reg;
  /* Write 16-bit Reg */
  LCD->LCD_RAM = LCD_RegValue;
}

/*******************************************************************************
* Function Name  : LCD_ReadReg
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
*******************************************************************************/
u16 LCD_ReadReg(u8 LCD_Reg)
{
  /* Write 16-bit Index (then Read Reg) */
  LCD->LCD_REG = LCD_Reg;
  /* Read 16-bit Reg */
  return (LCD->LCD_RAM);
}

/*******************************************************************************
* Function Name  : LCD_ReadRAM
* Description    : Reads the LCD RAM.
* Input          : None
* Output         : None
* Return         : LCD RAM Value.
*******************************************************************************/
u16 LCD_ReadRAM(void)
{
  /* Write 16-bit Index (then Read Reg) */
  LCD->LCD_REG = R34; /* Select GRAM Reg */
  /* Read 16-bit Reg */
  return (u16)LCD->LCD_RAM; 
}

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetCursor(u16 x,u16 y)
{
// 	if(DeviceCode==0x1505){}
	LCD_WriteReg(0x0020,x);        //行
// 	LCD_WriteReg(0x0021,0x13f-x);  //列
	LCD_WriteReg(0x0021,y);  //列
}			 

/****************************************************************************
* 名    称：void ili9320_SetWindows(u16 StartX,u16 StartY,u16 EndX,u16 EndY)
* 功    能：设置窗口区域
* 入口参数：StartX     行起始座标
*           StartY     列起始座标
*           EndX       行结束座标
*           EndY       列结束座标
* 出口参数：无
* 说    明：
* 调用方法：ili9320_SetWindows(0,0,100,100)；
****************************************************************************/
void LCD_SetWindow(u16 StartX,u16 StartY,u16 EndX,u16 EndY)
{
  LCD_SetCursor(StartX,StartY);
  LCD_WriteReg(0x0050, StartX);
  LCD_WriteReg(0x0052, StartY);
  LCD_WriteReg(0x0051, EndX);
  LCD_WriteReg(0x0053, EndY);
}

/****************************************************************************
* 名    称：void ili9320_Clear(u16 dat)
* 功    能：将屏幕填充成指定的颜色，如清屏，则填充 0xffff
* 入口参数：dat      填充值
* 出口参数：无
* 说    明：
* 调用方法：ili9320_Clear(0xffff);
****************************************************************************/
void LCD_Clear(u16 Color)
{
  u32 index=0;
  LCD_SetCursor(0,0); 
  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
  for(index=0;index<76800;index++)
	{
		LCD->LCD_RAM=Color;
	}
}

/****************************************************************************
* 名    称：u16 ili9320_GetPoint(u16 x,u16 y)
* 功    能：获取指定座标的颜色值
* 入口参数：x      行座标
*           y      列座标
* 出口参数：当前座标颜色值
* 说    明：
* 调用方法：i=ili9320_GetPoint(10,10);
****************************************************************************/
u16 LCD_GetPoint(u16 x,u16 y)
{
  LCD_SetCursor(x,y);
  return (LCD_ReadRAM());
}

/****************************************************************************
* 名    称：void ili9320_SetPoint(u16 x,u16 y,u16 point)
* 功    能：在指定座标画点
* 入口参数：x      行座标
*           y      列座标
*           point  点的颜色
* 出口参数：无
* 说    明：
* 调用方法：ili9320_SetPoint(10,10,0x0fe0);
****************************************************************************/
void LCD_SetPoint(u16 x,u16 y,u16 point)
{
  if ((x>240)||(y>320)) return;
  LCD_SetCursor(x,y);
  LCD_WriteRAM_Prepare();
  LCD_WriteRAM(point);
}

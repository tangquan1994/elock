#include "SDIO.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NULL 0
#define SDIO_STATIC_FLAGS               ((u32)0x000005FF)
#define SDIO_CMD0TIMEOUT                ((u32)0x00002710)
#define SDIO_FIFO_Address               ((u32)0x40018080)

/* Mask for errors Card Status R1 (OCR Register) */
#define SD_OCR_ADDR_OUT_OF_RANGE        ((u32)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((u32)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR            ((u32)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((u32)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM          ((u32)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION     ((u32)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((u32)0x01000000)
#define SD_OCR_COM_CRC_FAILED           ((u32)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((u32)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((u32)0x00200000)
#define SD_OCR_CC_ERROR                 ((u32)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((u32)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN     ((u32)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((u32)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((u32)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((u32)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((u32)0x00004000)
#define SD_OCR_ERASE_RESET              ((u32)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR            ((u32)0x00000008)
#define SD_OCR_ERRORBITS                ((u32)0xFDFFE008)

/* Masks for R6 Response */
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((u32)0x00002000)
#define SD_R6_ILLEGAL_CMD               ((u32)0x00004000)
#define SD_R6_COM_CRC_FAILED            ((u32)0x00008000)

#define SD_VOLTAGE_WINDOW_SD            ((u32)0x80100000)
#define SD_HIGH_CAPACITY                ((u32)0x40000000)
#define SD_STD_CAPACITY                 ((u32)0x00000000)
#define SD_CHECK_PATTERN                ((u32)0x000001AA)

#define SD_MAX_VOLT_TRIAL               ((u32)0x0000FFFF)
#define SD_ALLZERO                      ((u32)0x00000000)

#define SD_WIDE_BUS_SUPPORT             ((u32)0x00040000)
#define SD_SINGLE_BUS_SUPPORT           ((u32)0x00010000)
#define SD_CARD_LOCKED                  ((u32)0x02000000)
#define SD_CARD_PROGRAMMING             ((u32)0x00000007)
#define SD_CARD_RECEIVING               ((u32)0x00000006)
#define SD_DATATIMEOUT                  ((u32)0x000FFFFF)
#define SD_0TO7BITS                     ((u32)0x000000FF)
#define SD_8TO15BITS                    ((u32)0x0000FF00)
#define SD_16TO23BITS                   ((u32)0x00FF0000)
#define SD_24TO31BITS                   ((u32)0xFF000000)
#define SD_MAX_DATA_LENGTH              ((u32)0x01FFFFFF)

#define SD_HALFFIFO                     ((u32)0x00000008)
#define SD_HALFFIFOBYTES                ((u32)0x00000020)

/* Command Class Supported */
#define SD_CCCC_LOCK_UNLOCK             ((u32)0x00000080)
#define SD_CCCC_WRITE_PROT              ((u32)0x00000040)
#define SD_CCCC_ERASE                   ((u32)0x00000020)

/* Following commands are SD Card Specific commands.
   SDIO_APP_CMD should be sent before sending these commands. */
#define SDIO_SEND_IF_COND               ((u32)0x00000008)

#define SDIO_INIT_CLK_DIV                  ((u8)0xB2)
#define SDIO_TRANSFER_CLK_DIV              ((u8)0x6) 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u32 CardType =  SDIO_STD_CAPACITY_SD_CARD_V1_1;
static u32 CSD_Tab[4], CID_Tab[4], RCA = 0;
static u32 DeviceMode = SD_DMA_MODE;
static u32 TotalNumberOfBytes = 0, StopCondition = 0;
u32 *SrcBuffer, *DestBuffer;
volatile SD_Error TransferError = SD_OK;
vu32 TransferEnd = 0;
vu32 NumberOfBytes = 0;

/*******************************************************************************
* Function Name  : SDIO_DeInit
* Description    : Deinitializes the SDIO peripheral registers to their default
*                  reset values.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SDIO_DeInit(void)
{
  SDIO->POWER = 0x00000000;
  SDIO->CLKCR = 0x00000000;
  SDIO->ARG = 0x00000000;
  SDIO->CMD = 0x00000000;
  SDIO->DTIMER = 0x00000000;
  SDIO->DLEN = 0x00000000;
  SDIO->DCTRL = 0x00000000;
  SDIO->ICR = 0x00C007FF;
  SDIO->MASK = 0x00000000;
}


void SDIO_Init(void)
{
	RCC_SW(APB2_GPIOC | APB2_GPIOD,On);		//打开GPIOC/GPIOD时钟
	RCC_SW(AHB_SDIO,On);		//打开SDIO时钟
	
	GPIO_Init(GPIOC,Pin_8|Pin_9|Pin_10|Pin_11|Pin_12,GPIO_Mode_AF_PP,GPIO_Speed_50MHz);
	GPIO_Init(GPIOD,Pin_2,GPIO_Mode_AF_PP,GPIO_Speed_50MHz);
	
  SDIO_DeInit();
	
	//电源控制寄存器
	SDIO->POWER |= 0x03;		//SDIO power on
	//时钟控制寄存器
	SDIO->CLKCR &= ~(1<<14);		//关闭硬件流控制
	SDIO->CLKCR &= ~(1<<13);		//SDIOCLK上升沿产生SDIOCK
	SDIO->CLKCR &= ~(3<<11);		//清零11/12位
	SDIO->CLKCR |= 0<<11;				//1位总线模式
	SDIO->CLKCR &= ~(1<<10);		//关闭旁路,SDIO_CK是SDIOCLK的CLKDIV分频
	SDIO->CLKCR |= 1<<9;				//省电模式,只有总线有活动时SDIO_CK才输出信号
	SDIO->CLKCR |= 255<<0;			//分频系数255,慢速 SDIO_CK = SDIOCLK/(CLKDIV+2)
	SDIO->CLKCR |= 1<<8;				//open clock
}

void SDIO_Clock_Set(u8 div)
{
	SDIO->CLKCR |= div<<0;			//分频系数
}

void SDIO_Send_Cmd(u8 cmd,u32 data1,u32 data2)
{
  u32 tmpreg = 0;

/*---------------------------- SDIO ARG Configuration ------------------------*/
  /* Set the SDIO Argument value */
  SDIO->ARG = data1;
  
/*---------------------------- SDIO CMD Configuration ------------------------*/  
  /* Get the SDIO CMD value */
  tmpreg = SDIO->CMD;

  /* Clear CMDINDEX, WAITRESP, WAITINT, WAITPEND, CPSMEN bits */
  tmpreg &= (u32)0xFFFFF800;
  /* Set CMDINDEX bits according to SDIO_CmdIndex value */
  /* Set WAITRESP bits according to SDIO_Response value */
  /* Set WAITINT and WAITPEND bits according to SDIO_Wait value */
  /* Set CPSMEN bits according to SDIO_CPSM value */
  tmpreg |= (u32)cmd ;
  
  /* Write to SDIO CMD */
  SDIO->CMD = tmpreg;
}

SD_Error SD_PowerON(void)
{
	u8 i=0;
	SD_Error errorstatus=SD_OK;
	u32 response=0,count=0,validvoltage=0;
	u32 SDType=SD_STD_CAPACITY;
	//?? CLKCR ???
	SDIO->CLKCR=0; //?? CLKCR ?????
	SDIO->CLKCR|=0<<9; //?????
	SDIO->CLKCR|=0<<10;  //????,CK ????????
	SDIO->CLKCR|=0<<11;  //1 ?????
	SDIO->CLKCR|=0<<13;  //SDIOCLK ????? SDIOCK
	SDIO->CLKCR|=0<<14;  //???????
	SDIO_Clock_Set(SDIO_INIT_CLK_DIV);//??????(???? 400Khz) 
	SDIO->POWER=0X03; //????,?????
	SDIO->CLKCR|=1<<8; //SDIOCK ??
	for(i=0;i<74;i++)
	{
	SDIO_Send_Cmd(SDIO_GO_IDLE_STATE,0,0);//?? CMD0
	errorstatus=CmdError();
	if(errorstatus==SD_OK)break;
	}
	if(errorstatus)return errorstatus;//??????
	SDIO_Send_Cmd(SDIO_SEND_IF_COND,1,SD_CHECK_PATTERN);
	//?? CMD8,???,?? SD ?????.
	//arg[11:8]:01,??????,2.7~3.6V
	//arg[7:0]:?? 0XAA
	errorstatus=CmdResp7Error();  //?? R7 ??
	if(errorstatus==SD_OK) //R7 ????
	{
	CardType=SDIO_STD_CAPACITY_SD_CARD_V2_0;  //SD 2.0 ?
	SDType=SD_HIGH_CAPACITY; //????
	}
	SDIO_Send_Cmd(SD_CMD_APP_CMD,1,0);  //?? CMD55,???
	errorstatus=CmdResp1Error(SD_CMD_APP_CMD); //?? R1 ??
	if(errorstatus==SD_OK)//SD2.0/SD 1.1,??? MMC ?
	{ 
	//SD ?,?? ACMD41 SD_APP_OP_COND,???:0x80100000
	while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
	{ 
	SDIO_Send_Cmd(SD_CMD_APP_CMD,1,0);//?? CMD55,??? 
	errorstatus=CmdResp1Error(SD_CMD_APP_CMD); //?? R1 ??
	if(errorstatus!=SD_OK)return errorstatus; //????
	www.alientek.com ALIENTEK STM32??? AN1302
	www.openedv.com 2013-03-10
	SDIO_Send_Cmd(SD_CMD_SD_APP_OP_COND,1,
	SD_VOLTAGE_WINDOW_SD|SDType); //?? ACMD41,??? 
	errorstatus=CmdResp3Error(); //?? R3 ??
	if(errorstatus!=SD_OK)return errorstatus; //????
	response=SDIO->RESP1;;  //????
	validvoltage=(((response>>31)==1)?1:0);  //?? SD ???????
	count++;
	}
	if(count>=SD_MAX_VOLT_TRIAL)
	{
	errorstatus=SD_INVALID_VOLTRANGE;
	return errorstatus;
	} 
	if(response&=SD_HIGH_CAPACITY)
	{
	CardType=SDIO_HIGH_CAPACITY_SD_CARD;
	}
	}else//MMC ?
	{
	CardType=SDIO_MULTIMEDIA_CARD;
	//MMC ?,?? CMD1 SDIO_SEND_OP_COND,???:0x80FF8000
	while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
	{ 
	SDIO_Send_Cmd(SD_CMD_SEND_OP_COND,1,
	SD_VOLTAGE_WINDOW_MMC);//?? CMD1,???
	errorstatus=CmdResp3Error(); //?? R3 ??
	if(errorstatus!=SD_OK)return errorstatus; //????
	response=SDIO->RESP1;;  //????
	validvoltage=(((response>>31)==1)?1:0);
	count++;
	}
	if(count>=SD_MAX_VOLT_TRIAL)
	{
	errorstatus=SD_INVALID_VOLTRANGE;
	return errorstatus;
	} 
	}
	return(errorstatus);
}



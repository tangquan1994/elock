#include "Prtype.h"
#include "Uartport.h"
#include "usart.h" 



UINT8  UartRxBuff[MaxPakgeLen];
UINT8  UartTxBuff[MaxPakgeLen];
INT32  RevDtaNum;
INT32  RevDtaOffset; 
 

//一个简易的，大概的超时机制，主要用于串口通信 
//不能同时使用多个超时。 
INT32 TimeOutCnt0;
INT32 TimeOutCnt1;
 
#define  TimeOutSet0(tMs) do{\
	TimeOutCnt0=tMs*SYSFrequency/1000/TimeAdjust;\
}while(0); 
#define TimeOutCheck0()  TimeOutCnt0--

#define  TimeOutSet1(tMs) do{\
	TimeOutCnt1=tMs*SYSFrequency/1000/TimeAdjust;\
}while(0); 
#define TimeOutCheck1()  TimeOutCnt1--

#define UartTimeOut (SYSFrequency/UartBaudRate*10*10)//10字节时间 

UINT8 ClearRxBuff(void)
{
    UINT16 i;
    for (i=0;i<MaxPakgeLen;i++)
	   UartRxBuff[i]=0;
	RevDtaNum=0;
	RevDtaOffset=0;
    return 0;
}

UINT8 ClearTxBuff(void)
{
    UINT16 i;
    for (i=0;i<MaxPakgeLen;i++)
	   UartTxBuff[i]=0;
    return 0;
}

//Uart中断服务函数 
static void UartInterruptCallback(void)
{
    //需要客户修改 
// 	union UART_INTSTAT_REG IntFlag;
// 	volatile struct UART_REGS * pUart = GetUartRegs(0);
// 	IntFlag = pUart->INTSTAT;		
// 	pUart->INTCLR.all = IntFlag.all;	

// 	if(IntFlag.bit.RxIntf || IntFlag.bit.RxOerrIntf)	
// 	{
// 		while(pUart->CSTAT.bit.RxAvl)	
// 		{
//             if (RevDtaNum<MaxPakgeLen)   //判断数据接收buff是否满了 
//             {
//             	UartRxBuff[RevDtaNum++]=pUart->RXREG; //读数据 
//             }  
// 			else	
// 			{
// 				UINT8 temp;
// 				temp = pUart->RXREG;
// 			}
// 			TimeOutSet1(UartTimeOut);
// 		}
// 	}	
}

//Uart初始化 
UINT8 UartPortInit(void)
{
    ClearRxBuff(); 
    ClearTxBuff();
    TimeOutCnt0=0;
    TimeOutCnt1=0;
    
	//以下部分需要客户自己修改   
//     STRUCT_UART_FORMAT PRT_FRAME;
//     PRT_FRAME.BaudRate=UartBaudRate;
//     PRT_FRAME.CharBit=UartChar;
//     PRT_FRAME.Parity=0;
//     PRT_FRAME.StopBit=UartStopBit;
//     PRT_FRAME.TxMode=0;
//     UartInit(0,UART0_MAP_A18_19,(STRUCT_UART_FORMAT)PRT_FRAME,NULL,NULL,UartInterruptCallback);
    return 0;
}
 
//Uart发送数据 
static UINT8 UARTSendByte(UINT8 pDat) 
{
    //需要用户完善 
    UartSendByte(0,pDat);
	return 0;
}

UINT8 UartDataSend(UINT8*pDatBuf,UINT16 DataLen)
{
    UINT16 n = 0;

    while (n<DataLen) 
    {
    	UARTSendByte(*pDatBuf);
    	pDatBuf++;
    	n++;
    }     
	return 0;
}
//Uart接受数据 
UINT8 UartDataRev(UINT8*pDatBuf,UINT16 DataLen,UINT16 TimeOutMs)
{
    UINT16 i;
    TimeOutSet0(TimeOutMs);
    do{
         if (RevDtaNum-RevDtaOffset>=DataLen)    
         {
            for (i=0;i<DataLen;i++)
               pDatBuf[i]=UartRxBuff[RevDtaOffset+i];
            RevDtaOffset+=DataLen;
            return RT_OK;
         }     
    }
    while (TimeOutCheck0()>0);
	return RT_OVERTIME;
}







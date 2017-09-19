#include "Prtype.h"
#include "Uartport.h"
#include "usart.h" 



UINT8  UartRxBuff[MaxPakgeLen];
UINT8  UartTxBuff[MaxPakgeLen];
INT32  RevDtaNum;
INT32  RevDtaOffset; 
 

//һ�����׵ģ���ŵĳ�ʱ���ƣ���Ҫ���ڴ���ͨ�� 
//����ͬʱʹ�ö����ʱ�� 
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

#define UartTimeOut (SYSFrequency/UartBaudRate*10*10)//10�ֽ�ʱ�� 

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

//Uart�жϷ����� 
static void UartInterruptCallback(void)
{
    //��Ҫ�ͻ��޸� 
// 	union UART_INTSTAT_REG IntFlag;
// 	volatile struct UART_REGS * pUart = GetUartRegs(0);
// 	IntFlag = pUart->INTSTAT;		
// 	pUart->INTCLR.all = IntFlag.all;	

// 	if(IntFlag.bit.RxIntf || IntFlag.bit.RxOerrIntf)	
// 	{
// 		while(pUart->CSTAT.bit.RxAvl)	
// 		{
//             if (RevDtaNum<MaxPakgeLen)   //�ж����ݽ���buff�Ƿ����� 
//             {
//             	UartRxBuff[RevDtaNum++]=pUart->RXREG; //������ 
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

//Uart��ʼ�� 
UINT8 UartPortInit(void)
{
    ClearRxBuff(); 
    ClearTxBuff();
    TimeOutCnt0=0;
    TimeOutCnt1=0;
    
	//���²�����Ҫ�ͻ��Լ��޸�   
//     STRUCT_UART_FORMAT PRT_FRAME;
//     PRT_FRAME.BaudRate=UartBaudRate;
//     PRT_FRAME.CharBit=UartChar;
//     PRT_FRAME.Parity=0;
//     PRT_FRAME.StopBit=UartStopBit;
//     PRT_FRAME.TxMode=0;
//     UartInit(0,UART0_MAP_A18_19,(STRUCT_UART_FORMAT)PRT_FRAME,NULL,NULL,UartInterruptCallback);
    return 0;
}
 
//Uart�������� 
static UINT8 UARTSendByte(UINT8 pDat) 
{
    //��Ҫ�û����� 
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
//Uart�������� 
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







#ifndef UART_PORT_H_
#define UART_PORT_H_
#include "Prtype.h"

#define SYSFrequency    144000000//系统主频144MHz，用于超时参考 
#define  TimeAdjust  5 //此值是超时的精确度的调整系数，越大超时时间越小，如果超时不准可以调它。
#define UartBaudRate    57600  //波特率 
#define UartChar        8      //8位数据位
#define UartStopBit     1      //2位停止位 

//最大数据包的长度，单位字节 
#define MaxPakgeLen 1024

extern UINT8  UartRxBuff[];
extern UINT8  UartTxBuff[];

UINT8 UartPortInit(void);
UINT8 ClearRxBuff(void);
UINT8 ClearTxBuff(void);
//Uart发送数据 
UINT8 UartDataSend(UINT8*pDatBuf,UINT16 DataLen);
//Uart接受数据 
UINT8 UartDataRev(UINT8*pDatBuf,UINT16 DataLen,UINT16 TimeOutMs);

#endif

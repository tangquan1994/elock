#ifndef UART_PORT_H_
#define UART_PORT_H_
#include "Prtype.h"

#define SYSFrequency    144000000//ϵͳ��Ƶ144MHz�����ڳ�ʱ�ο� 
#define  TimeAdjust  5 //��ֵ�ǳ�ʱ�ľ�ȷ�ȵĵ���ϵ����Խ��ʱʱ��ԽС�������ʱ��׼���Ե�����
#define UartBaudRate    57600  //������ 
#define UartChar        8      //8λ����λ
#define UartStopBit     1      //2λֹͣλ 

//������ݰ��ĳ��ȣ���λ�ֽ� 
#define MaxPakgeLen 1024

extern UINT8  UartRxBuff[];
extern UINT8  UartTxBuff[];

UINT8 UartPortInit(void);
UINT8 ClearRxBuff(void);
UINT8 ClearTxBuff(void);
//Uart�������� 
UINT8 UartDataSend(UINT8*pDatBuf,UINT16 DataLen);
//Uart�������� 
UINT8 UartDataRev(UINT8*pDatBuf,UINT16 DataLen,UINT16 TimeOutMs);

#endif

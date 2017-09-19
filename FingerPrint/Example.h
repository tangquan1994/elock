#ifndef PROTOCOL_H_
#define PROTOCOL_H_
#include "Prtype.h"
/* 指令包存放地址定义 */
#define COMM_PAKGE_LEN          9                               // 包头长度 2包头+4地址+1包标示+2数据长度 
#define COMM_INS_BUF	        (UART_BUF)                      // 收发数据包起始地址,接收/发送数据包时的起始地址
#define COMM_INS_PARA_BUF	    (UART_BUF+COMM_PAKGE_LEN)       // 数据包参数起始地址,应用层处理指令后应答的参数存放地址 
#define COMM_DATA_BUF	        (IMG_BUF)                       // 包标示为数据包时,数据的存放地址(主要用于发送数据) 
#define COMM_IMG_BUF	        (IMG_BUF)                       // 收发图像数据包时，图像存放地址 

/* 包头和包标示 */
#define PKG_HEAD_BYTE0          0xef       // 包头 
#define PKG_HEAD_BYTE1          0x01
#define PKG_HEAD                0x01ef     // 包头 
#define PKG_DAT_FG              0x02       // 数据包还有后续数据包 
#define PKG_DAT_END_FG          0x08       // 最后一个数据包 
#define PKG_ACK_FG              0x07       // 应答包 
#define PKG_INS_FG              0x01       // 指令包,主机发送的指令 

//包标示类型 
#define MASK_INS        0x01    //指令包 
#define MASK_DAT        0x02    //数据包  
#define MASK_ACK        0x07    //应答包 
#define MASK_DAT_LAST    0x08    //最后数据包 



//返回码
#define CMD_OK              0   //通讯成功 
#define CMD_ERR             1   //通讯失败 
#define CMD_TIMEOUT         2   //命令超时 
#define CMD_SUM_ERR         3   //应答包校验错误 
#define CMD_COMM_ERR        4   //收到正常的应答包，但应答包的确认码报错 
#define CMD_LOGIC_ERR       0x0A //逻辑错误，收到的应答包与发送的指令不一致 
#define CMD_CODE_ERR        0x0B //代码编写有误 

void  PortocolInit(void);
UINT8 CommandSend(UINT8 mark,UINT16 Datalen,UINT8 Order,  UINT8 *pData);
UINT8 CommPakgeRev(UINT8 *pPakege,UINT16 TimeOutMs,UINT8 *ERR);
UINT8 CommDataPakgeRev(UINT8 *pData,UINT16 TimeOutMs,UINT8 *ERR);
UINT8 FunHandshakeModule(UINT8 nTime);

#endif

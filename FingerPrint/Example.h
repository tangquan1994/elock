#ifndef PROTOCOL_H_
#define PROTOCOL_H_
#include "Prtype.h"
/* ָ�����ŵ�ַ���� */
#define COMM_PAKGE_LEN          9                               // ��ͷ���� 2��ͷ+4��ַ+1����ʾ+2���ݳ��� 
#define COMM_INS_BUF	        (UART_BUF)                      // �շ����ݰ���ʼ��ַ,����/�������ݰ�ʱ����ʼ��ַ
#define COMM_INS_PARA_BUF	    (UART_BUF+COMM_PAKGE_LEN)       // ���ݰ�������ʼ��ַ,Ӧ�ò㴦��ָ���Ӧ��Ĳ�����ŵ�ַ 
#define COMM_DATA_BUF	        (IMG_BUF)                       // ����ʾΪ���ݰ�ʱ,���ݵĴ�ŵ�ַ(��Ҫ���ڷ�������) 
#define COMM_IMG_BUF	        (IMG_BUF)                       // �շ�ͼ�����ݰ�ʱ��ͼ���ŵ�ַ 

/* ��ͷ�Ͱ���ʾ */
#define PKG_HEAD_BYTE0          0xef       // ��ͷ 
#define PKG_HEAD_BYTE1          0x01
#define PKG_HEAD                0x01ef     // ��ͷ 
#define PKG_DAT_FG              0x02       // ���ݰ����к������ݰ� 
#define PKG_DAT_END_FG          0x08       // ���һ�����ݰ� 
#define PKG_ACK_FG              0x07       // Ӧ��� 
#define PKG_INS_FG              0x01       // ָ���,�������͵�ָ�� 

//����ʾ���� 
#define MASK_INS        0x01    //ָ��� 
#define MASK_DAT        0x02    //���ݰ�  
#define MASK_ACK        0x07    //Ӧ��� 
#define MASK_DAT_LAST    0x08    //������ݰ� 



//������
#define CMD_OK              0   //ͨѶ�ɹ� 
#define CMD_ERR             1   //ͨѶʧ�� 
#define CMD_TIMEOUT         2   //���ʱ 
#define CMD_SUM_ERR         3   //Ӧ���У����� 
#define CMD_COMM_ERR        4   //�յ�������Ӧ�������Ӧ�����ȷ���뱨�� 
#define CMD_LOGIC_ERR       0x0A //�߼������յ���Ӧ����뷢�͵�ָ�һ�� 
#define CMD_CODE_ERR        0x0B //�����д���� 

void  PortocolInit(void);
UINT8 CommandSend(UINT8 mark,UINT16 Datalen,UINT8 Order,  UINT8 *pData);
UINT8 CommPakgeRev(UINT8 *pPakege,UINT16 TimeOutMs,UINT8 *ERR);
UINT8 CommDataPakgeRev(UINT8 *pData,UINT16 TimeOutMs,UINT8 *ERR);
UINT8 FunHandshakeModule(UINT8 nTime);

#endif

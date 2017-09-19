#ifndef __SOCKET_H__
#define __SOCKET_H__
	#include "os_core.h"
	#include "LIB.h"
	#include "MAC.h"
	#include "enc28j60.h"

	typedef struct IPEndPoint
	{
	  unsigned char IP[4];
	  unsigned short Port;
	}IPEndPoint;

	typedef struct Socket
	{
		struct Socket *Next;						//�γɵ�������
		unsigned short Protocol;					//ʹ�õ�Э������
		unsigned short SrcPort;						//Դ�˿�
// 		IPEndPoint *SrcEP;							//ԴIP�˵�
		IPEndPoint *DstEP;							//Ŀ��IP�˵�
		void (*Send)(struct Socket *this,unsigned char *data,unsigned short len);			//���ͺ���
// 		void (*SendByte)(unsigned char data);		//�ֽڷ��ͺ���
// 		void (*SendBytes)(unsigned char *data);		//�ַ������ͺ���
		void (*DataReci)(unsigned char *data,unsigned short len);		//���ݽ��պ���
	}Socket;
	
	Socket* newSocket(unsigned short protocol,unsigned short port,IPEndPoint *ep,void (*handler)(unsigned char *data,unsigned short len));
	Socket* SocketSearch(unsigned short port);

#endif

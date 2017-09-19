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
		struct Socket *Next;						//形成单项链表
		unsigned short Protocol;					//使用的协议类型
		unsigned short SrcPort;						//源端口
// 		IPEndPoint *SrcEP;							//源IP端点
		IPEndPoint *DstEP;							//目的IP端点
		void (*Send)(struct Socket *this,unsigned char *data,unsigned short len);			//发送函数
// 		void (*SendByte)(unsigned char data);		//字节发送函数
// 		void (*SendBytes)(unsigned char *data);		//字符串发送函数
		void (*DataReci)(unsigned char *data,unsigned short len);		//数据接收函数
	}Socket;
	
	Socket* newSocket(unsigned short protocol,unsigned short port,IPEndPoint *ep,void (*handler)(unsigned char *data,unsigned short len));
	Socket* SocketSearch(unsigned short port);

#endif

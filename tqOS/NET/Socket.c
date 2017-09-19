#include "Socket.h"

static Socket *HeadSocket = null;

static void Send(Socket *this,unsigned char *data,unsigned short len);
static void SocketAdd(Socket *socket);

Socket* newSocket(unsigned short protocol,unsigned short port,IPEndPoint *ep,void (*handler)(unsigned char *data,unsigned short len))
{
	Socket *temp = OS_MemNew(mempool,sizeof(Socket)),*temp2;	//分配内存
	temp->Next		= null;
	temp->Protocol	= protocol;
	temp->SrcPort	= port;
	temp->DstEP		= ep;
	temp->DataReci	= handler;
	temp->Send		= Send;
// 	temp->SendByte	= SendByte;
// 	temp->SendBytes = SendBytes;
	SocketAdd(temp);
	
	//打印Socket链表
	temp2 = HeadSocket;
	printf("Sockets:");
	while(temp2 != null)
	{
		printf("%d->",temp2->Protocol);
		temp2 = temp2->Next;
	}
	printf("\r\n");
	
	return temp;
}

void delSocket(Socket *socket)
{
	
}

Socket* SocketSearch(unsigned short port)
{
	Socket *temp = HeadSocket;
	while(temp != null)
	{
		if(temp->SrcPort == port)
			break;
		temp = temp->Next;
	}
	return temp;
}

//添加socket到socket链表中去
static void SocketAdd(Socket *socket)
{
	Socket *temp = HeadSocket;
	if(HeadSocket == null)
		HeadSocket = socket;
	else
	{
		//找到最后一个元素
		while(temp->Next != null)
			temp = temp->Next;
		//插入到表中去
		temp->Next = socket;
		socket->Next = null;
	}
}

static void Send(Socket *this,unsigned char *data,unsigned short len)
{
	int cnt = 0;
	if(this->Protocol == IP_PROTO_UDP)
	{
		printf("发送UDP数据报\r\n");
		if(UDP_FrameWrap(BuffTx,data,this->DstEP->IP,len))
		{
			printf("找不到表现,发送ARP请求\r\n");
			ARP_SendRequest(BuffTx,this->DstEP->IP);
			while(UDP_FrameWrap(BuffTx,data,this->DstEP->IP,len))
			{
				//等待到收到ARP响应
				OS_Dly(10);
				if(++cnt > 100)
				{	//1s超时设置
					printf("发送超时\r\n");
					break;
				}
			}
		}
	}
}





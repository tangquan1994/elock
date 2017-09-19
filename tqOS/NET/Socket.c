#include "Socket.h"

static Socket *HeadSocket = null;

static void Send(Socket *this,unsigned char *data,unsigned short len);
static void SocketAdd(Socket *socket);

Socket* newSocket(unsigned short protocol,unsigned short port,IPEndPoint *ep,void (*handler)(unsigned char *data,unsigned short len))
{
	Socket *temp = OS_MemNew(mempool,sizeof(Socket)),*temp2;	//�����ڴ�
	temp->Next		= null;
	temp->Protocol	= protocol;
	temp->SrcPort	= port;
	temp->DstEP		= ep;
	temp->DataReci	= handler;
	temp->Send		= Send;
// 	temp->SendByte	= SendByte;
// 	temp->SendBytes = SendBytes;
	SocketAdd(temp);
	
	//��ӡSocket����
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

//���socket��socket������ȥ
static void SocketAdd(Socket *socket)
{
	Socket *temp = HeadSocket;
	if(HeadSocket == null)
		HeadSocket = socket;
	else
	{
		//�ҵ����һ��Ԫ��
		while(temp->Next != null)
			temp = temp->Next;
		//���뵽����ȥ
		temp->Next = socket;
		socket->Next = null;
	}
}

static void Send(Socket *this,unsigned char *data,unsigned short len)
{
	int cnt = 0;
	if(this->Protocol == IP_PROTO_UDP)
	{
		printf("����UDP���ݱ�\r\n");
		if(UDP_FrameWrap(BuffTx,data,this->DstEP->IP,len))
		{
			printf("�Ҳ�������,����ARP����\r\n");
			ARP_SendRequest(BuffTx,this->DstEP->IP);
			while(UDP_FrameWrap(BuffTx,data,this->DstEP->IP,len))
			{
				//�ȴ����յ�ARP��Ӧ
				OS_Dly(10);
				if(++cnt > 100)
				{	//1s��ʱ����
					printf("���ͳ�ʱ\r\n");
					break;
				}
			}
		}
	}
}





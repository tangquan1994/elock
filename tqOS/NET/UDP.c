#include "UDP.h"

void eUDP_DataHandler(unsigned char *buff,unsigned int len)
{
	Socket *socket;
	unsigned short port = ((unsigned short)buff[UDP_DST_PORT_H]<<8) + buff[UDP_DST_PORT_L];
	//�����ݴ���Socket������ȥ,���÷��϶˿ڵ�Socket��DataReci�ص�����
	socket = SocketSearch(port);
	//����п���Socket���Ҹ�Socket�Ľ��մ�������Ϊ��
	if(socket != null && socket->DataReci != null)
	{
		socket->DataReci(&buff[UDP_DATA],buff[UDP_LEN_L] - UDP_HEADER_LEN);
	}
}

//�½�һ��UDP�����ݱ�֡
//û�д��󷵻�false
//�д���(ARP��������Ҳ�������)����true
bool UDP_FrameWrap(unsigned char *buff,unsigned char *data,unsigned char *ip,unsigned short len)
{
	int i;
	unsigned short checksum;
	IP2MAC *ip_mac;
	if((ip_mac = eARP_TableSearch(ip)) != null)
	{
// 		printf("IP:");
// 		for(i=0;i<4;i++)
// 			printf("%d ",ip_mac->IP[i]);
// 		printf("\r\n");
// 		printf("MAC:");
// 		for(i=0;i<6;i++)
// 			printf("%X ",ip_mac->MAC[i]);
// 		printf("\r\n");
		
		//MAC֡ͷ��װ
		eMAC_FrameWrap(buff,ip_mac->MAC,MAC_PROTO_IP);
		//IP֡ͷ��װ ������������Ǹ��Ŷ԰�
		eIP_FrameWrap(buff,ip,IP_PROTO_UDP, UDP_HEADER_LEN + len);
// 		eIP_FrameWrap(buff,ip,IP_PROTO_UDP,MAC_HEADER_LEN + UDP_HEADER_LEN + len);

		//UDP֡ͷ
		//Դ�˿ں� 1200
		buff[UDP_SRC_PORT_H] = 0x04;
		buff[UDP_SRC_PORT_L] = 0xB0;
		//Ŀ�Ķ˿ں�
		buff[UDP_DST_PORT_H] = 0x04;
		buff[UDP_DST_PORT_L] = 0xB0;
		//�û����ݱ�����
		buff[UDP_LEN_H] = 0;
		buff[UDP_LEN_L] = len + 8;
		//У���
		buff[UDP_CHECKSUM_H] = 0;
		buff[UDP_CHECKSUM_L] = 0;
		//����
		for(i=0;i<len;i++)
			buff[UDP_DATA + i] = data[i];
		
		//UDP���ײ�����α�ײ�����ʵ�ײ�,����У���ʱ��Ҫ���������ײ�
		checksum = UDP_CheckSum(&buff[IP_SRC_IP],16 + len);
		buff[UDP_CHECKSUM_H] = checksum >> 8;
		buff[UDP_CHECKSUM_L] = checksum & 0xff;
		
// 		for(i=0;i<len + UDP_HEADER_LEN + IP_HEADER_LEN + MAC_HEADER_LEN;i++)
// 			printf("%.2X ",buff[i]);
// 		printf("\r\n");

		enc28j60PacketSend(len + UDP_HEADER_LEN + IP_HEADER_LEN + MAC_HEADER_LEN,buff);
		return false;
	}
	else
	{
// 		printf("�Ҳ�������,����ARP����\r\n");
// 		eARP_SendRequest(buff,ip);
// 		enc28j60PacketSend(42,buff);
		return true;
	}
}



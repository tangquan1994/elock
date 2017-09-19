#include "UDP.h"

void eUDP_DataHandler(unsigned char *buff,unsigned int len)
{
	Socket *socket;
	unsigned short port = ((unsigned short)buff[UDP_DST_PORT_H]<<8) + buff[UDP_DST_PORT_L];
	//将数据传入Socket链表中去,调用符合端口的Socket的DataReci回调函数
	socket = SocketSearch(port);
	//如果有可用Socket并且该Socket的接收处理函数不为空
	if(socket != null && socket->DataReci != null)
	{
		socket->DataReci(&buff[UDP_DATA],buff[UDP_LEN_L] - UDP_HEADER_LEN);
	}
}

//新建一个UDP的数据报帧
//没有错误返回false
//有错误(ARP缓存表中找不到表项)返回true
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
		
		//MAC帧头包装
		eMAC_FrameWrap(buff,ip_mac->MAC,MAC_PROTO_IP);
		//IP帧头包装 到底是下面的那个才对啊
		eIP_FrameWrap(buff,ip,IP_PROTO_UDP, UDP_HEADER_LEN + len);
// 		eIP_FrameWrap(buff,ip,IP_PROTO_UDP,MAC_HEADER_LEN + UDP_HEADER_LEN + len);

		//UDP帧头
		//源端口号 1200
		buff[UDP_SRC_PORT_H] = 0x04;
		buff[UDP_SRC_PORT_L] = 0xB0;
		//目的端口号
		buff[UDP_DST_PORT_H] = 0x04;
		buff[UDP_DST_PORT_L] = 0xB0;
		//用户数据报长度
		buff[UDP_LEN_H] = 0;
		buff[UDP_LEN_L] = len + 8;
		//校验和
		buff[UDP_CHECKSUM_H] = 0;
		buff[UDP_CHECKSUM_L] = 0;
		//数据
		for(i=0;i<len;i++)
			buff[UDP_DATA + i] = data[i];
		
		//UDP的首部包括伪首部和真实首部,计算校验和时需要包括两个首部
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
// 		printf("找不到表现,发送ARP请求\r\n");
// 		eARP_SendRequest(buff,ip);
// 		enc28j60PacketSend(42,buff);
		return true;
	}
}



#include "IP.h"


//IP数据报处理
void eIP_DataHandler(unsigned char *buff,unsigned int len)
{
	//判断是否是本机IP地址
	if(strcmp2(&buff[IP_DST_IP],LocalIP,4))
	{
		printf("ip:not my ip\r\n");
		return;
	}
	
	if(buff[IP_PROTO_TYPE]==IP_PROTO_ICMP && buff[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V)
	{
		printf("-ICMP\r\n");
		//如果接收到的是ping数据包,我们就发送pong回应(ping-pong乒乓)
		//make_echo_reply_from_request(buff, len);
		return;
	}
	else if(buff[IP_PROTO_TYPE]==IP_PROTO_TCP && buff[TCP_DST_PORT_H_P]==0)
	{
		printf("-TCP\r\n");
		if (buff[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
		{
			printf("TCP TCP_FLAGS_SYN_V\r\n");
			//make_tcp_synack_from_syn(buff);
			return;
		}
		if (buff[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
		{
			printf("TCP TCP_FLAGS_ACK_V\r\n");
			//plen=fill_tcp_data(buff,0,"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>汤权 TCP/IP协议栈设计</h1>");
			//make_tcp_ack_from_any(buff); 			// send ack for http get 做一次ack响应,没有数据
			//make_tcp_ack_with_data(buff,plen); // send data 将HTTP响应报文发出
			return;
		}
	}
	else if(buff[IP_PROTO_TYPE] == IP_PROTO_UDP)
	{
		printf("UDP packge recieved\r\n");
		eUDP_DataHandler(buff,len);
		//make_udp_reply_from_request(buff,"tangquan\0",5,myudpport);
	}
}

//为缓冲数组buff包装一层IP数据头
void eIP_FrameWrap(unsigned char *buff,unsigned char *dstip,unsigned char protocol,unsigned short len)
{
	unsigned short checksum;
	int i;
	//IP数据报版本号和首部长度 1byte
	buff[IP_VER_HEADLEN] = 0x45;
	//服务类型(没有用到) 1byte
	buff[IP_SER_TYPE] = 0x00;
	//数据总长度 2byte
	len += IP_HEADER_LEN;
	buff[IP_TOTLEN_H] = len >> 8;
	buff[IP_TOTLEN_L] = len & 0xff;
	//16比特标示 2byte
	buff[IP_ID_H] = 0;
	buff[IP_ID_L] = 0;
	//标志 3bit 片偏移 13bit
	buff[IP_FLAGS_OFFSET] = 0x40;
	buff[IP_FLAGS_OFFSET + 1] = 0x00;
// 	buff[IP_FLAGS_OFFSET] &= ~(7<<13);
// 	buff[IP_FLAGS_OFFSET] |= (2<<13);	//不能分片
// 	buff[IP_FLAGS_OFFSET] &= 7<<13;
	//TTL 1byte
	buff[IP_TTL] = 64;
	//上层协议 1byte
	buff[IP_PROTO_TYPE] = protocol;
	//首部校验和 2byte
	buff[IP_CHECKSUM_H] = 0;	//先把校验数据部分清零再计算首部校验和
	buff[IP_CHECKSUM_L] = 0;
	//源IP地址 4byte
	for(i=0;i<4;i++)
		buff[IP_SRC_IP + i] = LocalIP[i];
	//目的IP地址 4byte
	for(i=0;i<4;i++)
		buff[IP_DST_IP + i] = dstip[i];
	//选项(没有)
	
	//先将IP数据首部写好在计算校验和才行
	checksum = CheckSum(&buff[IP_VER_HEADLEN],20);
// 	printf("checksum:%X\r\n",checksum);
	buff[IP_CHECKSUM_H] = checksum >> 8;
	buff[IP_CHECKSUM_L] = checksum & 0xff;
}















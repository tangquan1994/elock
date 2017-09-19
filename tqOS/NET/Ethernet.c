/******************************************************************
File Name		: Ethernet.c
Author			: 汤权
Create Date 	: 2015/12/16
Version			: 1.0
Description		: 以太网MAC层协议,处理最原始的以太网数据并分析发往上一层
******************************************************************/
#include "Ethernet.h"
#include "sys.h"
#include "enc28j60.h"

unsigned char	mymac[6]	= {0x54,0x55,0x58,0x10,0x00,0x24};
unsigned char	myip[4]		= {192,168,0,3};
unsigned int	mywwwport	= 80; // listen port for tcp/www (max range 1-254)
unsigned int	myudpport	= 1200; // listen port for udp
unsigned char	password[]	= "tangquan"; // must not be longer than 9 char
unsigned char	buf[BUFFER_SIZE+1];

static void printHEX(unsigned int data)
{
	unsigned char str[8];
	int i;
	for(i=0;i<2;i++)
	{
		str[i] = data % 16;
		data /= 16;
		if(data == 0)
		{
			for(++i;i<8;i++)
				str[i] = 0;
			break;
		}
	}
	for(i=1;i>=0;i--)
	{
		if(str[i] > 9)
			UartSendByte(USART1,str[i]-10+'A');
		else
			UartSendByte(USART1,str[i]+'0');
	}
}

//返回0表示比较结果相同
//返回1表示字符串不相同
//返回2表示出错
int strcmp(unsigned char *s1,unsigned char *s2,int start1,int start2,int len)
{
#define null 0
	int i = len;
	if(s1 == null || s2 == null)
		return 2;
	while(i--)
	{
		if(s1[start1 + i] != s2[start2 + i])
			return 1;
	}
	return 0;
}

void eNET_Init(void)
{
	enc28j60Init(mymac);		//MAC地址设置
	//指示灯状态:0x476 is PHLCON LEDA(绿)=links status, LEDB(红)=receive/transmit
	enc28j60PhyWrite(PHLCON,0x7a4);								//LED指示灯显示发送和接收活动
	enc28j60clkout(2); 														// change clkout from 6.25MHz to 12.5MHz
}

void eNET_Thread(void)
{
	unsigned int plen;
	plen = enc28j60PacketReceive(BUFFER_SIZE, buf);			//接收数据包
	if(plen == 0)
	  return;

	eMac_DataReci(buf,plen);
}

/*****************************************************************
* Description	: 以太网原始数据输入函数eMac_DataReci();
* Arguments		: *buff 数据缓冲区指针,len 数据长度
* Returns		: null
* Notes			: 
*****************************************************************/
void eMac_DataReci(unsigned char *buff,unsigned int len)
{
	int i,plen;
	if(len == 0)
		return;
	
	for(i=0;i<len;i++)
	{
		printHEX(buff[i]);
		printf(" ");
	}
	

	if(buff[MAC_P_TYPE] == P_TYPE_ARP_L && buff[MAC_P_TYPE+1] == P_TYPE_ARP_H)
	{
		printf("arp\r\n");
		//判断是否是本机IP地址
		if(strcmp(buff,myip,ETH_ARP_DST_IP_P,0,4))
		{
			printf("arp:no my ip\r\n");
			return;
		}
		
		//修改数据报的MAC帧首部,将目的地址和源地址交换
		i = 0;
		while(i<6)
		{
			buff[MAC_ADDR_DST +i] = buff[MAC_ADDR_SRC +i];
			buff[MAC_ADDR_SRC +i] = mymac[i];
			i++;
		}
		
		//
		buff[ETH_ARP_OPCODE_H_P] = ETH_ARP_OPCODE_REPLY_H_V;
		buff[ETH_ARP_OPCODE_L_P] = ETH_ARP_OPCODE_REPLY_L_V;


		i=0;
		while(i<6)
		{
        
			buff[ETH_ARP_DST_MAC_P+i]=buff[ETH_ARP_SRC_MAC_P+i];
        
			buff[ETH_ARP_SRC_MAC_P+i]=mymac[i];
        
			i++;
		
		}
	
		i=0;
		while(i<4)
		{
        
			buff[ETH_ARP_DST_IP_P+i]=buff[ETH_ARP_SRC_IP_P+i];
        
			buff[ETH_ARP_SRC_IP_P+i]=myip[i];
        
			i++;
		
		}
		enc28j60PacketSend(42,buff); 

	}
	else if(buff[MAC_P_TYPE] == P_TYPE_IP_L && buff[MAC_P_TYPE+1] == P_TYPE_IP_H)
	{
		printf("ip\r\n");
		//判断是否是本机IP地址
		if(strcmp(buff,myip,IP_DST_P,0,4))
		{
			printf("ip:no my ip\r\n");
			return;
		}
		//如果接收到的是ping数据包,我们就发送pong回应(ping-pong乒乓)
		if(buff[IP_PROTO_P]==IP_PROTO_ICMP_V && buff[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V)
		{
			printf("ICMP packge recieved\r\n");
			// a ping packet, let's send pong
// 			make_echo_reply_from_request(buff, len);
			return;
		}
		else if(buff[IP_PROTO_P]==IP_PROTO_TCP_V&&buff[TCP_DST_PORT_H_P]==0&&buff[TCP_DST_PORT_L_P]==mywwwport)
		{
			printf("TCP packge recieved\r\n");
			if (buff[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
			{
				printf("TCP TCP_FLAGS_SYN_V\r\n");
// 				make_tcp_synack_from_syn(buff);
				return;
			}
			if (buff[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
			{
				printf("TCP TCP_FLAGS_ACK_V\r\n");
// 				plen=fill_tcp_data(buff,0,"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>汤权 TCP/IP协议栈设计</h1>");
// 				make_tcp_ack_from_any(buff); 			// send ack for http get 做一次ack响应,没有数据
// 				make_tcp_ack_with_data(buff,plen); // send data 将HTTP响应报文发出
				return;
			}
		}
		else if(buff[IP_PROTO_P]==IP_PROTO_UDP_V && buff[UDP_DST_PORT_H_P]==4 && buff[UDP_DST_PORT_L_P]==0xb0)
		{
			printf("UDP packge recieved\r\n");
			for(i = UDP_DATA_P;i < UDP_DATA_P + buff[UDP_LEN_L_P] - UDP_HEADER_LEN;i++)
			{
				printHEX(buff[i]);
				printf(" ");
			}
// 		make_udp_reply_from_request(buff,"tangquan\0",5,myudpport);
		}
	}
	else
	{
		printf("unknown\r\n");
	}
}

void eIP_DataProc(unsigned char *buff,unsigned int len)
{
// 	unsigned char *p;
// 	ip_frame.IPv = buff[MAC_HEAD_LEN + 0] & 0x0f;
// 	ip_frame.HeadLen = (buff[MAC_HEAD_LEN + 0] >> 4) & 0x0f;
// 	ip_frame.Service = buff[MAC_HEAD_LEN + 1];
// 	ip_frame.FrameLen = buff[MAC_HEAD_LEN + 2] + ((unsigned short)buff[MAC_HEAD_LEN + 3] << 8);
// 	ip_frame.Identification = buff[MAC_HEAD_LEN + 4] + ((unsigned short)buff[MAC_HEAD_LEN + 5] << 8);
// 	ip_frame.Flag = buff[MAC_HEAD_LEN + 6] & 0x07;
// 	ip_frame.Offset = buff[MAC_HEAD_LEN + 6] + ((unsigned short)buff[MAC_HEAD_LEN + 7] << 8);
// 	ip_frame.Offset >>= 3;
// 	ip_frame.TTL = buff[MAC_HEAD_LEN + 8];
// 	ip_frame.Protocol = buff[MAC_HEAD_LEN + 9];
// 	ip_frame.CheckSum = buff[MAC_HEAD_LEN + 10] + ((unsigned short)buff[MAC_HEAD_LEN + 11] << 8);
// 	p = (unsigned char *)&ip_frame.SRC_Addr;
// 	*(p++) = buff[MAC_HEAD_LEN + 12];
// 	*(p++) = buff[MAC_HEAD_LEN + 13];
// 	*(p++) = buff[MAC_HEAD_LEN + 14];
// 	*(p++) = buff[MAC_HEAD_LEN + 15];
// 	p = (unsigned char *)&ip_frame.DST_Addr;
// 	*(p++) = buff[MAC_HEAD_LEN + 16];
// 	*(p++) = buff[MAC_HEAD_LEN + 17];
// 	*(p++) = buff[MAC_HEAD_LEN + 18];
// 	*(p++) = buff[MAC_HEAD_LEN + 19];
	
	
}

void eUDP_DataProc(unsigned char *buff,unsigned int len)
{
	
}












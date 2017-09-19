/******************************************************************
File Name		: Ethernet.c
Author			: 汤权
Create Date 	: 2015/12/16
Version			: 1.0
Description		: 以太网MAC层协议,处理最原始的以太网数据并分析发往上一层
******************************************************************/
#include "MAC.h"
#include "stdio.h"
#include "enc28j60.h"


/*****************************************************************
* Description	: 以太网原始数据输入函数eMac_DataReci();
* Arguments		: *buff 数据缓冲区指针,len 数据长度
* Returns		: null
* Notes			:
*****************************************************************/
void eMAC_DataReci(unsigned char *buff,unsigned int len)
{
	int i,protocol;
	if(len == 0)
		return;
	
	//打印接收到的MAC帧
// 	printf("mac buffer:");
// 	for(i=0;i<len;i++)
// 		printf("%.2X ",buff[i]);
// 	printf("\r\n");
	
	//解析MAC层的上层协议类型
	protocol = eMAC_Protocol(buff);
	printf("mac protocol:0x%.4X",protocol);
	
	if(protocol == MAC_PROTO_ARP)
	{
		//ARP协议全称Address Resolution Protocol即地址解析协议，
		//是根据IP地址获取物理MAC地址的一个TCP/IP协议。
		printf("-arp\r\n");
		eARP_ReciHandler(buff,len);
	}
	else if(protocol == MAC_PROTO_IP)
	{
		printf("-ip\r\n");
		eIP_DataHandler(buff,len);
	}
	else
	{
		printf("-unknown\r\n");
	}
}

//提取mac帧的协议类型字段
unsigned short eMAC_Protocol(unsigned char *buff)
{
	return buff[MAC_PROTO_TYPE_L] | ((unsigned short)buff[MAC_PROTO_TYPE_H] << 8);
}

//为缓冲数组buff包装一层MAC数据头
void eMAC_FrameWrap(unsigned char *buff,unsigned char *dstmac,unsigned short protocol)
{
	int i;
	for(i=0;i<6;i++)
	{
		buff[MAC_DST_MAC + i] = dstmac[i];
		buff[MAC_SRC_MAC + i] = LocalMAC[i];
	}
	//上层协议类型
	buff[MAC_PROTO_TYPE_H] = (protocol & 0xff00) >> 8;
	buff[MAC_PROTO_TYPE_L] = protocol & 0x00ff;
}





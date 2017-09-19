#include "ARP.h"

//ARP缓存表,20个表项,可以使用操作系统动态内存分配得到
IP2MAC ARP_TABLE[ARP_TABLE_SIZE];
//ARP缓存表表项计数,指向缓冲区中的下一个空位置
int TableItemCnt = 0;


//ARP数据报处理,直接在接收到的数据报上进行修改不需要在生成一个新的数据报
void eARP_ReciHandler(unsigned char *buff,unsigned int len)
{
	int i;
	//判断是否是本机IP地址
	if(strcmp2(&buff[ARP_DST_IP],LocalIP,4))
	{
		printf(">arp:not my ip\r\n");
		return;
	}
	//判断op字段,若是1表示ARP请求,为2表示ARP应答
	if(buff[ARP_OP_L] == ARP_OP_REQUEST_L && buff[ARP_OP_H] == ARP_OP_REQUEST_H)
	{	
		printf(">ARP请求\r\n");
		//将发出ARP请求的IP/MAC地址对存入ARP缓存表
		eARP_TableAdd(&buff[ARP_SRC_IP],&buff[ARP_SRC_MAC]);
		
		//如果是ARP请求则做出ARP回复
		//修改数据报的MAC帧首部,将原始数据报中的源MAC地址复制到目的MAC地址
		//同时将本机的MAC地址覆盖到源MAC地址。
		for(i=0;i<6;i++)
		{
			buff[MAC_DST_MAC +i] = buff[MAC_SRC_MAC +i];
			buff[MAC_SRC_MAC +i] = LocalMAC[i];
		}
		
		//硬件类型 0x0001 ?
		//协议类型 0x0800 ?
		//硬件地址长度(MAC地址) 6
		//协议地址长度(IPv4) 4
		
		//op字段为1表示ARP请求，为2表示ARP应答
		buff[ARP_OP_L] = ARP_OP_REPLY_L;
		buff[ARP_OP_H] = ARP_OP_REPLY_H;
	
		//交换ARP数据报中的发送端以太网地址和目的以太网地址
		for(i=0;i<6;i++)
		{
			buff[ARP_DST_MAC+i] = buff[ARP_SRC_MAC+i];
			buff[ARP_SRC_MAC+i] = LocalMAC[i];
		}
		
		//交换ARP数据报中的发送端IP地址和目的IP地址
		for(i=0;i<4;i++)
		{
			buff[ARP_DST_IP+i] = buff[ARP_SRC_IP+i];
			buff[ARP_SRC_IP+i] = LocalIP[i];	
		}
		
		//发送ARP响应数据报,完整的ARP数据报(不包括CRC校验的4个字节)长度为42字节
		enc28j60PacketSend(42,buff);
	}
	else if(buff[ARP_OP_L] == ARP_OP_REPLY_L && buff[ARP_OP_H] == ARP_OP_REPLY_H)
	{
		printf(">ARP应答\r\n");
		//如果是ARP应答则将IP和MAC地址对存入ARP缓存表中去
		eARP_TableAdd(&buff[ARP_SRC_IP],&buff[ARP_SRC_MAC]);
	}
}

//ARP数据报生成函数,根据参数生成一个ARP数据报(Package Generate)
//产生错误则返回true,否则返回false
void ARP_SendRequest(unsigned char *buff,unsigned char *dst_ip)
{
	int i,j = 0;  //j负责帧的数据指针定位
		
	/****************MAC数据帧帧头*****************/
	//以太网目的MAC地址和源MAC地址
	for(i = 0;i < 6;i++,j++)
	{
		buff[j]      = 0xFF;	//目的MAC地址为FF表示广播
		buff[j + 6]  = LocalMAC[i];
	}
	j += 6;
	
	//上一层协议类型:ARP
	buff[j++] = MAC_PROTO_ARP_H;
	buff[j++] = MAC_PROTO_ARP_L;
	/**********************************************/
	
	/****************ARP数据帧帧头*****************/
	//硬件类型:1表示以太网
	buff[j++] = 0x00;
	buff[j++] = 0x01;
	
	//协议类型:0x0008表示IP地址
	buff[j++] = 0x08;
	buff[j++] = 0x00;
	
	//硬件地址长度:MAC地址长度6
	buff[j++] = 6;
	
	//协议地址长度:IP地址长度4
	buff[j++] = 4;
	
	//OP字段:1表示ARP请求 2表示ARP应答
	buff[j++] = ARP_OP_REQUEST_L;
	buff[j++] = ARP_OP_REQUEST_H;
	
	//发送端以太网地址
	for(i = 0;i < 6;i++,j++)
		buff[j] = LocalMAC[i];
	
	//发送端IP地址,32位的IP地址可以直接用int类型变量一次性赋值
	*(unsigned int *)(&buff[j]) = *(unsigned int *)LocalIP;
	j += 4; //过掉IP地址
	
	//目的以太网地址
	for(i = 0;i < 6;i++,j++)
		buff[j] = 0;
	
	//目的IP地址,32位的IP地址可以直接用int类型变量一次性赋值
	*(unsigned int *)(&buff[j]) = *(unsigned int *)dst_ip;
	
	/**********************************************/
	
	enc28j60PacketSend(42,BuffTx);
}

void eARP_TableAdd(unsigned char *ip,unsigned char *mac)
{
	int i,j;

	//如果表项数量已满
	if(TableItemCnt >= ARP_TABLE_SIZE)
	{
		printf(">ARP缓存表已满,清空表\r\n");
		//使用简单的循环替换表项算法
		TableItemCnt = 0;
	}
	
	printf(">查看是否已有表项\r\n");
	for(i=0;i<TableItemCnt;i++)
	{
		for(j=0;j<4;j++)
		{
			if(ARP_TABLE[i].IP[j] != ip[j])
				break;
		}
		if(j == 4)
			break;
	}
	
	if(i == TableItemCnt)
	{
		printf(">表中没有该项记录,添加表项\r\n");
		//如果表中没有该项记录
		*(unsigned int *)ARP_TABLE[TableItemCnt].IP = *(unsigned int *)ip;
		for(i=0;i<6;i++)
			ARP_TABLE[TableItemCnt].MAC[i] = mac[i];
		TableItemCnt++;
	}
	else
	{
		printf(">表中有该项记录,覆盖表项\r\n");
		*(unsigned int *)ARP_TABLE[i].IP = *(unsigned int *)ip;
		for(j=0;j<6;j++)
			ARP_TABLE[i].MAC[j] = mac[j];
	}
	
	for(i=0;i<TableItemCnt;i++)
	{
		printf(">表项%d:IP ADDR:",i);
		for(j=0;j<4;j++)
			printf("%d ",ARP_TABLE[i].IP[j]);
		printf(" MAC ADDR:");
		for(j=0;j<6;j++)
			printf("%X ",ARP_TABLE[i].MAC[j]);
		printf("\r\n");
	}
}

IP2MAC* eARP_TableSearch(unsigned char *ip)
{
	int i,j;
	//查找ARP表中的表项
	for(i=0;i<TableItemCnt;i++)
	{
		for(j=0;j<4;j++)
			if(ARP_TABLE[i].IP[j] != ip[j])
				break;
		if(j == 4)
			break;
	}
	if(i == TableItemCnt)
		return null;	//无表项
	else
		return &ARP_TABLE[i];
}












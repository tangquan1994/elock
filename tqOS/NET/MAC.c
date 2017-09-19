/******************************************************************
File Name		: Ethernet.c
Author			: ��Ȩ
Create Date 	: 2015/12/16
Version			: 1.0
Description		: ��̫��MAC��Э��,������ԭʼ����̫�����ݲ�����������һ��
******************************************************************/
#include "MAC.h"
#include "stdio.h"
#include "enc28j60.h"


/*****************************************************************
* Description	: ��̫��ԭʼ�������뺯��eMac_DataReci();
* Arguments		: *buff ���ݻ�����ָ��,len ���ݳ���
* Returns		: null
* Notes			:
*****************************************************************/
void eMAC_DataReci(unsigned char *buff,unsigned int len)
{
	int i,protocol;
	if(len == 0)
		return;
	
	//��ӡ���յ���MAC֡
// 	printf("mac buffer:");
// 	for(i=0;i<len;i++)
// 		printf("%.2X ",buff[i]);
// 	printf("\r\n");
	
	//����MAC����ϲ�Э������
	protocol = eMAC_Protocol(buff);
	printf("mac protocol:0x%.4X",protocol);
	
	if(protocol == MAC_PROTO_ARP)
	{
		//ARPЭ��ȫ��Address Resolution Protocol����ַ����Э�飬
		//�Ǹ���IP��ַ��ȡ����MAC��ַ��һ��TCP/IPЭ�顣
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

//��ȡmac֡��Э�������ֶ�
unsigned short eMAC_Protocol(unsigned char *buff)
{
	return buff[MAC_PROTO_TYPE_L] | ((unsigned short)buff[MAC_PROTO_TYPE_H] << 8);
}

//Ϊ��������buff��װһ��MAC����ͷ
void eMAC_FrameWrap(unsigned char *buff,unsigned char *dstmac,unsigned short protocol)
{
	int i;
	for(i=0;i<6;i++)
	{
		buff[MAC_DST_MAC + i] = dstmac[i];
		buff[MAC_SRC_MAC + i] = LocalMAC[i];
	}
	//�ϲ�Э������
	buff[MAC_PROTO_TYPE_H] = (protocol & 0xff00) >> 8;
	buff[MAC_PROTO_TYPE_L] = protocol & 0x00ff;
}





#include "ARP.h"

//ARP�����,20������,����ʹ�ò���ϵͳ��̬�ڴ����õ�
IP2MAC ARP_TABLE[ARP_TABLE_SIZE];
//ARP�����������,ָ�򻺳����е���һ����λ��
int TableItemCnt = 0;


//ARP���ݱ�����,ֱ���ڽ��յ������ݱ��Ͻ����޸Ĳ���Ҫ������һ���µ����ݱ�
void eARP_ReciHandler(unsigned char *buff,unsigned int len)
{
	int i;
	//�ж��Ƿ��Ǳ���IP��ַ
	if(strcmp2(&buff[ARP_DST_IP],LocalIP,4))
	{
		printf(">arp:not my ip\r\n");
		return;
	}
	//�ж�op�ֶ�,����1��ʾARP����,Ϊ2��ʾARPӦ��
	if(buff[ARP_OP_L] == ARP_OP_REQUEST_L && buff[ARP_OP_H] == ARP_OP_REQUEST_H)
	{	
		printf(">ARP����\r\n");
		//������ARP�����IP/MAC��ַ�Դ���ARP�����
		eARP_TableAdd(&buff[ARP_SRC_IP],&buff[ARP_SRC_MAC]);
		
		//�����ARP����������ARP�ظ�
		//�޸����ݱ���MAC֡�ײ�,��ԭʼ���ݱ��е�ԴMAC��ַ���Ƶ�Ŀ��MAC��ַ
		//ͬʱ��������MAC��ַ���ǵ�ԴMAC��ַ��
		for(i=0;i<6;i++)
		{
			buff[MAC_DST_MAC +i] = buff[MAC_SRC_MAC +i];
			buff[MAC_SRC_MAC +i] = LocalMAC[i];
		}
		
		//Ӳ������ 0x0001 ?
		//Э������ 0x0800 ?
		//Ӳ����ַ����(MAC��ַ) 6
		//Э���ַ����(IPv4) 4
		
		//op�ֶ�Ϊ1��ʾARP����Ϊ2��ʾARPӦ��
		buff[ARP_OP_L] = ARP_OP_REPLY_L;
		buff[ARP_OP_H] = ARP_OP_REPLY_H;
	
		//����ARP���ݱ��еķ��Ͷ���̫����ַ��Ŀ����̫����ַ
		for(i=0;i<6;i++)
		{
			buff[ARP_DST_MAC+i] = buff[ARP_SRC_MAC+i];
			buff[ARP_SRC_MAC+i] = LocalMAC[i];
		}
		
		//����ARP���ݱ��еķ��Ͷ�IP��ַ��Ŀ��IP��ַ
		for(i=0;i<4;i++)
		{
			buff[ARP_DST_IP+i] = buff[ARP_SRC_IP+i];
			buff[ARP_SRC_IP+i] = LocalIP[i];	
		}
		
		//����ARP��Ӧ���ݱ�,������ARP���ݱ�(������CRCУ���4���ֽ�)����Ϊ42�ֽ�
		enc28j60PacketSend(42,buff);
	}
	else if(buff[ARP_OP_L] == ARP_OP_REPLY_L && buff[ARP_OP_H] == ARP_OP_REPLY_H)
	{
		printf(">ARPӦ��\r\n");
		//�����ARPӦ����IP��MAC��ַ�Դ���ARP�������ȥ
		eARP_TableAdd(&buff[ARP_SRC_IP],&buff[ARP_SRC_MAC]);
	}
}

//ARP���ݱ����ɺ���,���ݲ�������һ��ARP���ݱ�(Package Generate)
//���������򷵻�true,���򷵻�false
void ARP_SendRequest(unsigned char *buff,unsigned char *dst_ip)
{
	int i,j = 0;  //j����֡������ָ�붨λ
		
	/****************MAC����֡֡ͷ*****************/
	//��̫��Ŀ��MAC��ַ��ԴMAC��ַ
	for(i = 0;i < 6;i++,j++)
	{
		buff[j]      = 0xFF;	//Ŀ��MAC��ַΪFF��ʾ�㲥
		buff[j + 6]  = LocalMAC[i];
	}
	j += 6;
	
	//��һ��Э������:ARP
	buff[j++] = MAC_PROTO_ARP_H;
	buff[j++] = MAC_PROTO_ARP_L;
	/**********************************************/
	
	/****************ARP����֡֡ͷ*****************/
	//Ӳ������:1��ʾ��̫��
	buff[j++] = 0x00;
	buff[j++] = 0x01;
	
	//Э������:0x0008��ʾIP��ַ
	buff[j++] = 0x08;
	buff[j++] = 0x00;
	
	//Ӳ����ַ����:MAC��ַ����6
	buff[j++] = 6;
	
	//Э���ַ����:IP��ַ����4
	buff[j++] = 4;
	
	//OP�ֶ�:1��ʾARP���� 2��ʾARPӦ��
	buff[j++] = ARP_OP_REQUEST_L;
	buff[j++] = ARP_OP_REQUEST_H;
	
	//���Ͷ���̫����ַ
	for(i = 0;i < 6;i++,j++)
		buff[j] = LocalMAC[i];
	
	//���Ͷ�IP��ַ,32λ��IP��ַ����ֱ����int���ͱ���һ���Ը�ֵ
	*(unsigned int *)(&buff[j]) = *(unsigned int *)LocalIP;
	j += 4; //����IP��ַ
	
	//Ŀ����̫����ַ
	for(i = 0;i < 6;i++,j++)
		buff[j] = 0;
	
	//Ŀ��IP��ַ,32λ��IP��ַ����ֱ����int���ͱ���һ���Ը�ֵ
	*(unsigned int *)(&buff[j]) = *(unsigned int *)dst_ip;
	
	/**********************************************/
	
	enc28j60PacketSend(42,BuffTx);
}

void eARP_TableAdd(unsigned char *ip,unsigned char *mac)
{
	int i,j;

	//���������������
	if(TableItemCnt >= ARP_TABLE_SIZE)
	{
		printf(">ARP���������,��ձ�\r\n");
		//ʹ�ü򵥵�ѭ���滻�����㷨
		TableItemCnt = 0;
	}
	
	printf(">�鿴�Ƿ����б���\r\n");
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
		printf(">����û�и����¼,��ӱ���\r\n");
		//�������û�и����¼
		*(unsigned int *)ARP_TABLE[TableItemCnt].IP = *(unsigned int *)ip;
		for(i=0;i<6;i++)
			ARP_TABLE[TableItemCnt].MAC[i] = mac[i];
		TableItemCnt++;
	}
	else
	{
		printf(">�����и����¼,���Ǳ���\r\n");
		*(unsigned int *)ARP_TABLE[i].IP = *(unsigned int *)ip;
		for(j=0;j<6;j++)
			ARP_TABLE[i].MAC[j] = mac[j];
	}
	
	for(i=0;i<TableItemCnt;i++)
	{
		printf(">����%d:IP ADDR:",i);
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
	//����ARP���еı���
	for(i=0;i<TableItemCnt;i++)
	{
		for(j=0;j<4;j++)
			if(ARP_TABLE[i].IP[j] != ip[j])
				break;
		if(j == 4)
			break;
	}
	if(i == TableItemCnt)
		return null;	//�ޱ���
	else
		return &ARP_TABLE[i];
}












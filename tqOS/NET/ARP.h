#ifndef __ARP_H__
#define __ARP_H__
	#include "LIB.h"
	#include "enc28j60.h"
	#include "MAC.h"
	
	//ARP������������
	#define ARP_TABLE_SIZE	20
	//ARP��������
	typedef struct IP2MAC
	{
		unsigned char IP[4];
		unsigned char MAC[6];
	}IP2MAC;
	extern IP2MAC ARP_TABLE[ARP_TABLE_SIZE];

	
	//ʹ��С��ģʽ,���ݻ������е��ֽڱ�ʾ���ֽ�
	// ******* ARPЭ����ֶ�ƫ�����Ϳ�ȡֵ *******
	//ARPЭ��OP�ֶ�ƫ��
	#define ARP_OP_L			0x14
	#define ARP_OP_H			0x15
	//ARPӦ��
	#define ARP_OP_REPLY_L		0x00
	#define ARP_OP_REPLY_H		0x02
	//ARP����
	#define ARP_OP_REQUEST_L	0x00
	#define ARP_OP_REQUEST_H	0x01

	//ARPĿ��MAC/IP��ַƫ��
	#define ARP_DST_MAC			0x20
	#define ARP_DST_IP			0x26
	//ARPԴMAC/IP��ַƫ��
	#define ARP_SRC_MAC			0x16
	#define ARP_SRC_IP			0x1c
	
	void eARP_ReciHandler(unsigned char *buff,unsigned int len);
	void ARP_SendRequest(unsigned char *buff,unsigned char *dst_ip);
	void eARP_TableAdd(unsigned char *ip,unsigned char *mac);
	IP2MAC* eARP_TableSearch(unsigned char *ip);

#endif

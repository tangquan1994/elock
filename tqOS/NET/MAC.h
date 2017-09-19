#ifndef __MAC_H__
#define __MAC_H__
	#include "network.h"
	#include "LIB.h"
	#include "ARP.h"
	#include "IP.h"
	#include "UDP.h"

	/************************************************************************
	*************************************************************************
	TCP/IPЭ��������֡ͳһ���ô��ģʽ,���ֽ��ں���ֽ���ǰ,���������ݻ���
	������ǰ����Ǹ��ֽں�����ǵ��ֽ�,����������������дϰ��.
	*************************************************************************
	************************************************************************/
	
	//�ϲ�Э������
	#define MAC_PROTO_ARP			0x0806	//ARPЭ��
	#define MAC_PROTO_ARP_H			0x08
	#define MAC_PROTO_ARP_L			0x06
	#define MAC_PROTO_IP			0x0800	//IPЭ��
	#define MAC_PROTO_IP_H			0x08
	#define MAC_PROTO_IP_L			0x00
	
	//MAC֡�ײ�����
	#define MAC_HEADER_LEN	14

	//MAC֡���ֶ�ƫ����(in order)
	#define MAC_DST_MAC				0			//MAC֡Ŀ��MAC��ַ
	#define MAC_SRC_MAC				6			//MAC֡ԴMAC��ַ
	#define MAC_PROTO_TYPE_H		12			//MAC֡Э������H
	#define MAC_PROTO_TYPE_L		13			//MAC֡Э������L
	#define MAC_DATA				14			//MAC֡����
	
	

	// ******* ICMP *******
	#define ICMP_TYPE_ECHOREPLY_V 0
	#define ICMP_TYPE_ECHOREQUEST_V 8
	//
	#define ICMP_TYPE_P 0x22
	#define ICMP_CHECKSUM_P 0x24
	
	// ******* TCP *******
	#define TCP_SRC_PORT_H_P 0x22
	#define TCP_SRC_PORT_L_P 0x23
	#define TCP_DST_PORT_H_P 0x24
	#define TCP_DST_PORT_L_P 0x25
	// the tcp seq number is 4 bytes 0x26-0x29
	#define TCP_SEQ_H_P 0x26
	#define TCP_SEQACK_H_P 0x2a
	// flags: SYN=2
	#define TCP_FLAGS_P 0x2f
	#define TCP_FLAGS_SYN_V 2
	#define TCP_FLAGS_FIN_V 1
	#define TCP_FLAGS_PUSH_V 8
	#define TCP_FLAGS_SYNACK_V 0x12
	#define TCP_FLAGS_ACK_V 0x10
	#define TCP_FLAGS_PSHACK_V 0x18
	//  plain len without the options:
	#define TCP_HEADER_LEN_PLAIN 20
	#define TCP_HEADER_LEN_P 0x2e
	#define TCP_CHECKSUM_H_P 0x32
	#define TCP_CHECKSUM_L_P 0x33
	#define TCP_OPTIONS_P 0x36
	
	void eMAC_DataReci(unsigned char *buff,unsigned int len);
	unsigned short eMAC_Protocol(unsigned char *buff);
	void eMAC_FrameWrap(unsigned char *buff,unsigned char *dstmac,unsigned short protocol);
	
	
#endif

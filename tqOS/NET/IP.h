#ifndef __IP_H__
#define __IP_H__
	#include "LIB.h"
	#include "enc28j60.h"
	#include "MAC.h"
	#include "UDP.h"

	typedef struct IP_FRAME
	{
		unsigned char IPv;
		unsigned char HeadLen;
		unsigned char Service;
		unsigned short FrameLen;
		unsigned short Identification;
		unsigned char Flag;
		unsigned short Offset;
		unsigned char TTL;
		unsigned char Protocol;
		unsigned short CheckSum;
		unsigned int SRC_Addr;
		unsigned int DST_Addr;
	}IP_FRAME;
	
	//IP数据帧头长度
	#define IP_HEADER_LEN		20
	//IP数据帧各字段偏移量(按顺序排列)
	#define IP_VER_HEADLEN		14
	#define IP_SER_TYPE			15
	#define IP_TOTLEN_H			16
	#define IP_TOTLEN_L			17
	#define IP_ID_H				18
	#define IP_ID_L				19
	#define IP_FLAGS_OFFSET		20
	#define IP_TTL				22
	#define IP_PROTO_TYPE		23  
	#define IP_CHECKSUM_H		24
	#define IP_CHECKSUM_L		25
	#define IP_SRC_IP			26
	#define IP_DST_IP			30
	
	//IP层协议类型取值
	#define IP_PROTO_ICMP		1
	#define IP_PROTO_TCP		6
	#define IP_PROTO_UDP		17
	
	void eIP_DataHandler(unsigned char *buff,unsigned int len);
	void eIP_FrameWrap(unsigned char *buff,unsigned char *dstip,unsigned char protocol,unsigned short len);

#endif

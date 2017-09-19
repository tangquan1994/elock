#ifndef __UDP_H__
#define __UDP_H__
	#include "LIB.h"
	#include "enc28j60.h"
	#include "Socket.h"
	#include "MAC.h"
	
	//IP数据帧头长度
	#define UDP_HEADER_LEN	8
	//IP数据帧各字段偏移量(按顺序排列)
	#define UDP_SRC_PORT_H		0x22
	#define UDP_SRC_PORT_L		0x23
	#define UDP_DST_PORT_H		0x24
	#define UDP_DST_PORT_L		0x25
	#define UDP_LEN_H			0x26
	#define UDP_LEN_L			0x27
	#define UDP_CHECKSUM_H		0x28
	#define UDP_CHECKSUM_L		0x29
	#define UDP_DATA			0x2a
	
	
	void eUDP_DataHandler(unsigned char *buff,unsigned int len);
	bool UDP_FrameWrap(unsigned char *buff,unsigned char *data,unsigned char *ip,unsigned short len);

#endif

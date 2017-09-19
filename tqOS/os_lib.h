#ifndef __LIB_H__
#define __LIB_H__
	#include "sys.h"
	#include "stdio.h"

	int strcmp2(unsigned char *s1,unsigned char *s2,int len);
	unsigned short CheckSum(unsigned char *buf,unsigned int len);
	unsigned short UDP_CheckSum(unsigned char *buf,unsigned int len);
	unsigned char* strlwr(unsigned char *buff);
	unsigned char* strtrim(unsigned char *buff);
	unsigned char* strgetw(unsigned char *buff,unsigned char divide,unsigned int num,unsigned int *len);
	unsigned int strcntw(unsigned char *buff,unsigned char divide);
	
	bool IPAddressParse(unsigned char *str,unsigned char *dst);

#endif






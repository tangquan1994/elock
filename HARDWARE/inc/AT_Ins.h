#ifndef __AT_INS_H__
#define __AT_INS_H__
	#include "SYS.h"
	#include "DELAY.h"
	#include "USART.h"

	typedef enum
	{
		AT_OK		= 0,
		AT_ERROR	= 1
	}RESULT;
	
	//CW --> Control Word
	typedef enum
	{
		MODE_STA = 1,
		MODE_AP = 2,
		MODE_STA_AP = 3,
	}CWMODE;

	#define ESP8266_RST		PBout(5)

	RESULT AT_Reset(void);
	RESULT AT_Test(void);
	RESULT AT_Version(void);
	RESULT AT_STA_AP(CWMODE mode);
	RESULT AT_ListAP(void);
	RESULT AT_JoinAP(char *ssid,char *passwd);
	RESULT AT_QuitAP(void);
	RESULT AT_IP(void);
	RESULT AT_EstTCPCon(char *addr,u16 port);
	RESULT AT_RegUDPPort(void);
	RESULT AT_DirectTrans(u8 sw);
	RESULT AT_SendData(u8 *data,u8 len);
	
	void AT_WaitResponse2(char *str);
	RESULT AT_WaitResponse3(char *str1,char *str2);

	int strcmp2(unsigned char *s1,unsigned char *s2,int len);
	unsigned char* strlwr(unsigned char *buff);
	unsigned char* strtrim(unsigned char *buff);









#endif

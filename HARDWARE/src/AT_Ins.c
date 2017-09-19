#include "AT_Ins.h"
#include "string.h"
#include "util_queue.h"
#include "os_core.h"

u8 ResultStr[256];
u8 Usart2Buf[100];
extern Queue Esp8266Queue;

//官方的方法：从接收到的字符串中寻找使用strstr函数寻找预定义的字符串
RESULT AT_WaitResponse(u16 timeout)
{
	u8 c;
	u16 pt = 0;
	
// 	先等收到第一个字节，等第一个字节不设定超时时间，
// 	当收到第一个字节之后后面的字节再设定超时机制

	//等待收到指令回显数据
	while(1)
	{
		c = UartGetByte(USART2);
		if(c == '\r' || c == '\n')
		{
			ResultStr[pt++] = '\r';
			ResultStr[pt++] = '\n';
			ResultStr[pt] = '\0';
			printf("%s",ResultStr);	//打印指令回显
			break;
		}
		ResultStr[pt++] = c;
	}
	pt = 0;	//
// 	ResultStr[pt++] = UartGetByte(USART2);	//等待获取第一个回应字符串
	//获取esp8266的回应字符串
	while(1)
	{
		if(UartGetByte2(USART2,(u32)timeout*1000,&c))
		{
			ResultStr[pt++] = c;
// 			UartSendByte(USART1,c);
		}
		else
			break;
	}
	
	ResultStr[pt] = '\0';	//添加一个结束符
	printf("%s",ResultStr);	//打印
	
	//查找返回的字符串中有没有ERROR或者OK单词
	if(strstr((const char*)ResultStr,"OK"))
	{
// 		printf("OK");
		return AT_OK;
	}
	else if(strstr((const char*)ResultStr,"ERROR"))
	{
// 		printf("ERROR");
		return AT_ERROR;
	}
	return AT_ERROR;
}

//等待收到str字符串则返回
void AT_WaitResponse2(char *str)
{
	u8 c,i = 0,len = strlen(str),data;
	QueueFlush(&Esp8266Queue);	//先清空队列缓冲区
	//等待收到指令回显数据
	while(UartGetByte3(USART1,&c) == 0)
	{
		while(QueueGet(&Esp8266Queue,&data))
		{
			UartSendByte(USART1,data);
			if(data == str[i])
			{
				i++;
				if(i == len)
				{
// 					printf("\r\nfind %s,breakout\r\n",str);
					printf("\r\n");
					return;
				}
			}
			else
				i = 0;
		}
		delay_ms(10);
	}
}

//str1表示正确,str2表示错误,函数拥有返回值
RESULT AT_WaitResponse3(char *str1,char *str2)
{
	u8 c,cnt1 = 0,cnt2 = 0,len1 = strlen(str1),len2 = strlen(str2),data;
	//等待收到指令回显数据
	while(UartGetByte3(USART1,&c) == 0)
	{
		while(QueueGet(&Esp8266Queue,&data))
		{
			UartSendByte(USART1,data);
			if(data == str1[cnt1])
			{
				cnt1++;
				if(cnt1 == len1)
				{
// 					printf("\r\nfind %s,breakout\r\n",str1);
					printf("\r\n");
					return AT_OK;
				}
			}
			else
				cnt1 = 0;
			
			if(data == str2[cnt2])
			{
				cnt2++;
				if(cnt2 == len2)
				{
// 					printf("\r\nfind %s,breakout\r\n",str2);
					printf("\r\n");
					return AT_ERROR;
				}
			}
			else
				cnt2 = 0;
		}
		delay_ms(10);
	}
	return AT_ERROR;
}

RESULT AT_Reset(void)
{
	printf("reset esp8266\r\n");
	//rst引脚的上升沿对esp8266进行复位操作
	ESP8266_RST = 0;
	delay_ms(10);
	ESP8266_RST = 1;
	delay_ms(10);
	
	AT_WaitResponse2("ready");
	printf("reset done\r\n");
	return AT_OK;
}

RESULT AT_Test(void)
{
	printf("test esp8266\r\n");
	sprintf((char*)Usart2Buf,"AT\r\n");
	UartSendStr(USART2,Usart2Buf);
	AT_WaitResponse2("OK");
	return AT_OK;
}

RESULT AT_Version(void)
{
	printf("esp8266 version\r\n");
	sprintf((char*)Usart2Buf,"AT+GMR\r\n");
	UartSendStr(USART2,Usart2Buf);
	AT_WaitResponse2("OK");
	return AT_OK;
}

RESULT AT_STA_AP(CWMODE mode)
{
	printf("esp8266 working mode\r\n");
	sprintf((char*)Usart2Buf,"AT+CWMODE=%d\r\n",mode);
	UartSendStr(USART2,Usart2Buf);
	AT_WaitResponse2("OK");
	return AT_OK;
}

RESULT AT_ListAP(void)
{
	printf("list avalible AP\r\n");
	sprintf((char*)Usart2Buf,"AT+CWLAP\r\n");
	UartSendStr(USART2,Usart2Buf);
	AT_WaitResponse2("OK");
	return AT_OK;
}

RESULT AT_JoinAP(char *ssid,char *passwd)
{
	printf("connect to ssid:%s with passwd:%s\r\n",ssid,passwd);
	sprintf((char*)Usart2Buf,"AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,passwd);
	UartSendStr(USART2,Usart2Buf);
// 	AT_WaitResponse3("OK","ERROR");
	return AT_WaitResponse3("OK","FAIL");
}

RESULT AT_QuitAP(void)
{
	printf("disconnect from AP\r\n");
	sprintf((char*)Usart2Buf,"AT+CWQAP\r\n");
	UartSendStr(USART2,Usart2Buf);
	AT_WaitResponse2("OK");
	return AT_OK;
}

RESULT AT_IP(void)
{
	printf("IP address\r\n");
	sprintf((char*)Usart2Buf,"AT+CIFSR\r\n");
	UartSendStr(USART2,Usart2Buf);
	AT_WaitResponse2("OK");
	return AT_OK;
}

//establish TCP connection
RESULT AT_EstTCPCon(char *addr,u16 port)
{
	printf("establish TCP connection\r\n");
	sprintf((char*)Usart2Buf,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",addr,port);
	UartSendStr(USART2,Usart2Buf);
	//有的模块的回应是Linked有的是CONNECTED加OK，很伤脑经啊
// 	AT_WaitResponse2("OK");
// 	AT_WaitResponse2("Linked");
	return AT_WaitResponse3("Linked","Unlink");;
}

//establish UDP server
RESULT AT_RegUDPPort(void)
{
	printf("establish UDP server\r\n");
	sprintf((char*)Usart2Buf,"AT+CIPSTART=\"UDP\",\"255.255.255.255\",5000,5000\r\n");
	UartSendStr(USART2,Usart2Buf);
	AT_WaitResponse2("OK");
	return AT_OK;
}

//非透传模式下发送len个字节的数据
RESULT AT_SendData(u8 *data,u8 len)
{
	int i;
	printf("send data\r\n");
	sprintf((char*)Usart2Buf,"AT+CIPSEND=%d\r\n",len);
	UartSendStr(USART2,Usart2Buf);
	AT_WaitResponse2("OK");
	for(i=0;i<len;i++)
		UartSendByte(USART2,data[i]);
	AT_WaitResponse2("SEND OK");//等到收到回应
	return AT_OK;
}

//Transparent transmission
RESULT AT_DirectTrans(u8 sw)
{
	printf("transparent transmission\r\n");
	if(sw == On)
	{
		sprintf((char*)Usart2Buf,"AT+CIPMODE=1\r\n");
 		UartSendStr(USART2,Usart2Buf);
		AT_WaitResponse2("OK");

		UartSendStr(USART2,"AT+CIPSEND\r\n");
		AT_WaitResponse2(">");
		return AT_OK;
	}
	else
	{
 		UartSendStr(USART2,"+++");
		return AT_OK;
	}
}
















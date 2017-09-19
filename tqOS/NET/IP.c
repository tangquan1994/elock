#include "IP.h"


//IP���ݱ�����
void eIP_DataHandler(unsigned char *buff,unsigned int len)
{
	//�ж��Ƿ��Ǳ���IP��ַ
	if(strcmp2(&buff[IP_DST_IP],LocalIP,4))
	{
		printf("ip:not my ip\r\n");
		return;
	}
	
	if(buff[IP_PROTO_TYPE]==IP_PROTO_ICMP && buff[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V)
	{
		printf("-ICMP\r\n");
		//������յ�����ping���ݰ�,���Ǿͷ���pong��Ӧ(ping-pongƹ��)
		//make_echo_reply_from_request(buff, len);
		return;
	}
	else if(buff[IP_PROTO_TYPE]==IP_PROTO_TCP && buff[TCP_DST_PORT_H_P]==0)
	{
		printf("-TCP\r\n");
		if (buff[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
		{
			printf("TCP TCP_FLAGS_SYN_V\r\n");
			//make_tcp_synack_from_syn(buff);
			return;
		}
		if (buff[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
		{
			printf("TCP TCP_FLAGS_ACK_V\r\n");
			//plen=fill_tcp_data(buff,0,"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>��Ȩ TCP/IPЭ��ջ���</h1>");
			//make_tcp_ack_from_any(buff); 			// send ack for http get ��һ��ack��Ӧ,û������
			//make_tcp_ack_with_data(buff,plen); // send data ��HTTP��Ӧ���ķ���
			return;
		}
	}
	else if(buff[IP_PROTO_TYPE] == IP_PROTO_UDP)
	{
		printf("UDP packge recieved\r\n");
		eUDP_DataHandler(buff,len);
		//make_udp_reply_from_request(buff,"tangquan\0",5,myudpport);
	}
}

//Ϊ��������buff��װһ��IP����ͷ
void eIP_FrameWrap(unsigned char *buff,unsigned char *dstip,unsigned char protocol,unsigned short len)
{
	unsigned short checksum;
	int i;
	//IP���ݱ��汾�ź��ײ����� 1byte
	buff[IP_VER_HEADLEN] = 0x45;
	//��������(û���õ�) 1byte
	buff[IP_SER_TYPE] = 0x00;
	//�����ܳ��� 2byte
	len += IP_HEADER_LEN;
	buff[IP_TOTLEN_H] = len >> 8;
	buff[IP_TOTLEN_L] = len & 0xff;
	//16���ر�ʾ 2byte
	buff[IP_ID_H] = 0;
	buff[IP_ID_L] = 0;
	//��־ 3bit Ƭƫ�� 13bit
	buff[IP_FLAGS_OFFSET] = 0x40;
	buff[IP_FLAGS_OFFSET + 1] = 0x00;
// 	buff[IP_FLAGS_OFFSET] &= ~(7<<13);
// 	buff[IP_FLAGS_OFFSET] |= (2<<13);	//���ܷ�Ƭ
// 	buff[IP_FLAGS_OFFSET] &= 7<<13;
	//TTL 1byte
	buff[IP_TTL] = 64;
	//�ϲ�Э�� 1byte
	buff[IP_PROTO_TYPE] = protocol;
	//�ײ�У��� 2byte
	buff[IP_CHECKSUM_H] = 0;	//�Ȱ�У�����ݲ��������ټ����ײ�У���
	buff[IP_CHECKSUM_L] = 0;
	//ԴIP��ַ 4byte
	for(i=0;i<4;i++)
		buff[IP_SRC_IP + i] = LocalIP[i];
	//Ŀ��IP��ַ 4byte
	for(i=0;i<4;i++)
		buff[IP_DST_IP + i] = dstip[i];
	//ѡ��(û��)
	
	//�Ƚ�IP�����ײ�д���ڼ���У��Ͳ���
	checksum = CheckSum(&buff[IP_VER_HEADLEN],20);
// 	printf("checksum:%X\r\n",checksum);
	buff[IP_CHECKSUM_H] = checksum >> 8;
	buff[IP_CHECKSUM_L] = checksum & 0xff;
}















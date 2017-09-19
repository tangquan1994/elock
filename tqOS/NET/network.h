#ifndef __NETWORK_H__
#define __NETWORK_H__
	#include "LIB.h"
	#include "os_core.h"
	#include "enc28j60.h"
	#include "MAC.h"
	
	
	//数据缓冲区大小
	#define BUFFER_SIZE_TX 1500
	#define BUFFER_SIZE_RX 1500
	
	
	//网络有限状态机枚举
	typedef enum
	{
		NET_FSM_IDLE,	//网络空闲
		
	}NET_FSM;
	
	extern unsigned char	LocalMAC[6];
	extern unsigned char	LocalIP[4];
	extern unsigned char	*BuffTx,*BuffRx;
	extern TCB *t_net_rx,*t_net_tx;

	void NET_Init(void);
	void p_net_tx(void);
	void p_net_rx(void);

#endif

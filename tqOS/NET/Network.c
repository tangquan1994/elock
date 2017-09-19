#include "network.h"

TCB *t_net_rx,*t_net_tx;
unsigned char	LocalMAC[6]	= {1,2,3,4,5,6};
unsigned char	LocalIP[4]	= {192,168,1,104};
unsigned char	*BuffTx,*BuffRx;

unsigned int NowStatus = NET_FSM_IDLE;

void NET_Init(void)
{
	BuffTx = OS_MemNew(mempool,BUFFER_SIZE_TX);	//分配内存
	BuffRx = OS_MemNew(mempool,BUFFER_SIZE_RX);	//分配内存
	
	enc28j60Init(LocalMAC);	//MAC地址设置
	//指示灯状态:0x476 is PHLCON LEDA(绿)=links status, LEDB(红)=receive/transmit
	enc28j60PhyWrite(PHLCON,0x7a4);	//LED指示灯显示发送和接收活动
	enc28j60clkout(2);	// change clkout from 6.25MHz to 12.5MHz
}

//如果接收到有效的MAC数据报则会产生一次中断
void EXTI2_IRQHandler(void)
{
	EXTI->PR |= 1<<2;
// 	printf("net data\r\n");
// 	OS_Resume(t_net);
}

void p_net_rx(void)
{
	unsigned int plen;
	NET_Init();
	//网卡初始化完成才能创建网络发送进程
	OS_TaskNew(&t_net_tx,p_net_tx,200,2);
	while(1)
	{
// 		OS_Suspend(t_net);
		plen = enc28j60PacketReceive(BUFFER_SIZE_RX, BuffRx);			//接收数据包
		if(plen != 0)
			eMAC_DataReci(BuffRx,plen);
 		OS_Dly(10);
	}
}

void p_net_tx(void)
{
	while(1)
	{
		OS_Suspend(t_net_tx);

	}
}



#include "network.h"

TCB *t_net_rx,*t_net_tx;
unsigned char	LocalMAC[6]	= {1,2,3,4,5,6};
unsigned char	LocalIP[4]	= {192,168,1,104};
unsigned char	*BuffTx,*BuffRx;

unsigned int NowStatus = NET_FSM_IDLE;

void NET_Init(void)
{
	BuffTx = OS_MemNew(mempool,BUFFER_SIZE_TX);	//�����ڴ�
	BuffRx = OS_MemNew(mempool,BUFFER_SIZE_RX);	//�����ڴ�
	
	enc28j60Init(LocalMAC);	//MAC��ַ����
	//ָʾ��״̬:0x476 is PHLCON LEDA(��)=links status, LEDB(��)=receive/transmit
	enc28j60PhyWrite(PHLCON,0x7a4);	//LEDָʾ����ʾ���ͺͽ��ջ
	enc28j60clkout(2);	// change clkout from 6.25MHz to 12.5MHz
}

//������յ���Ч��MAC���ݱ�������һ���ж�
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
	//������ʼ����ɲ��ܴ������緢�ͽ���
	OS_TaskNew(&t_net_tx,p_net_tx,200,2);
	while(1)
	{
// 		OS_Suspend(t_net);
		plen = enc28j60PacketReceive(BUFFER_SIZE_RX, BuffRx);			//�������ݰ�
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



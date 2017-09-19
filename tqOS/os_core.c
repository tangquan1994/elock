#include "os_core.h"

//�ڴ��
#define MEMPOOL_SIZE_IN_KB		16
unsigned int __align(8) MemPool[1024 * MEMPOOL_SIZE_IN_KB / 4];

TCB *Idle;				//��������,�������û�����û������ʱ����

u32 OS_CPU_Usage;		//CPUʹ����
u32 SysTicks;			//ϵͳʱ�Ӽ����� 
u32 StatTicks;			//ͳ��ʱ�Ӽ�����
bool StStat;			//ͳ�Ʊ�־

// TCB *TCBDlyHead;		//��ʱ����ͷ
TCB *TCBEventHead = null;		//�¼�����ͷ

TCB *Curr;				//��ǰ����ָ��
TCB *Next;				//�������е�����ָ��

static unsigned char		OsTcbFlag[8];			//�����־λ����,һ���ֽڱ�ʾһ����,һ��8��,ÿ��8������
													//һ��64λ���ڱ�ʾ64������ľ�����־,Ϊ1��ʾ�������,Ϊ0��ʾΪ����
													
unsigned char				OsTcbFlagFlag;			//�����������־,���ڱ�ʾÿһ��(8������)���Ƿ��о�������,���ĳһ��
													//�������о���������,��OsTcbFlagFlag��Ӧƫ������λ��1��������


static const unsigned char	OsMaskTable[] =			//��������
{
	0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80
};

static const unsigned char	OsMapTable[256] =		//�������ȼ�ӳ���
{
	255,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,	/*255��ʾû��λ����1*/
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
};

TCB *OsTcbTable[64];			//�����8*8,һ��64������,ÿ������һ�����ȼ�0-63,��ֵԽ�߱�ʾ
								//���ȼ�Խ��,������е�Ԫ��Ϊnull,��ʾ�����ȼ�������û��ʹ��
								
								
								
						
int OsTcbGetIdeal(void)
{	
	int temp = OsMapTable[OsTcbFlagFlag];	//temp��ʾ��ߵľ����������ڵ��������ƫ����(0~7)
	if(temp == 255)	/*255��ʾû�о�������*/
		return -1;	//û�о������񷵻�-1
	return ( (temp << 3) + OsMapTable[OsTcbFlag[temp]] );	//���ȼ����ھ���������ų���8�������������ƫ��
}

void OsTcbFlagSet(int prio)
{
	int x,y;
	if(prio > 63)
		return;
	x = prio & 0x07;	//prio % 8;
	y = prio >> 3;		//prio / 8;
	OsTcbFlagFlag |= OsMaskTable[y];	//���־
	OsTcbFlag[y]  |= OsMaskTable[x];	//���ڱ�־
}

void OsTcbFlagClr(int prio)
{
	int x,y;
	if(prio > 63)
		return;
	x = prio & 0x07;	//prio % 8;
	y = prio >> 3;		//prio / 8;
	OsTcbFlag[y] &= ~OsMaskTable[x];	//���ڱ�־
	if(OsTcbFlag[y] == 0)	//��������û�о���������,�ͽ�OsTcbFlagFlag�е���Ӧλ����
		OsTcbFlagFlag &= ~OsMaskTable[y];	//���־
}

void PrintFlags(void)
{
	int i;
	printf("OsTcbFlagFlag:%d\r\n",OsTcbFlagFlag);
	for(i=0;i<8;i++)
		printf("%.2X\r\n",(unsigned int)OsTcbFlag[i]);
}

static void IdleTask(void)
{
	//��������������Ⱥ��¼�����
	while(1)
	{
		StStat = true;	//CPUʹ���ʼ���ԭ��:
						//��������������ʱ,û����һ��ʱ���жϾͽ���һ��ͳ�Ƽ���
						//Ҳ������ͳ�ƿ����������е�ʱ���ܺ�,1s��֮�����ͳ�Ƽ�
						//�����ܼ����ı���,������ǿ�����,1��ȥ�����ʾ���ʹ����
		if(OS_ANY_TASK)
		{
			StStat = false;
			OS_Schedule();
		}
	}
}

void OS_Init(void)
{
	int i;
	OsTcbFlagFlag = 0;
	for(i=0;i<8;i++)
		OsTcbFlag[i] = 0;
	for(i=0;i<64;i++)
		*( (unsigned char *)OsTcbTable + i ) = null;
	
	SysTicks		= 0;
	StatTicks		= 0;
	OS_CPU_Usage 	= 0;
	OS_MEM_Usage 	= 0;
	StStat			= false;
	
	//�ڴ�س�ʼ��
	OS_MemInit(MemPool,sizeof(MemPool));
		
	//������������Idle
	OS_TaskNew(&Idle,IdleTask,100,0,"Idle");

	//Ϊ�˽�ʡ�ռ�,ʹ��Idle��Ϊ��ʱ����ı�ͷ,����ʡ�´���TCBDlyHead������ƿ�Ŀռ�
	Idle->Next = Idle->Prev = Idle;	//ѭ������
		
	//PendSV���жϳ�ʼ��,���ȼ�����Ϊ���
	PendSVInit();
}

void OS_TaskNew(TCB **tcb,void (*task)(void),u32 stksize,u32 prio,char *name)
{
	u32 *temp;
	//Ϊ����TCB����ռ�
	*tcb = new(MemPool,sizeof(TCB));
	
	//���������и����ȼ���λ���Ѿ�������ռ��,�򷵻�,���񴴽�ʧ��
	if( OsTcbTable[prio] != null )
	{
		printf("error:�����Ѵ���\r\n");
		return;
	}
	//���������и����ȼ���λ��Ϊnull��ʾδ��ռ��,����и�ֵ
	( (TCB **)OsTcbTable)[prio] = *tcb;
	
	//�������������־λ
	OsTcbFlagSet(prio);
	
	//Ϊ�����ջ����ռ�
	temp = new(MemPool,stksize*4);
	//����Ҫ��TCB�����һ����¼��ջ�����ַ���ֶκ�һ����ջ��С���ֶ�,��������ڴ����

	(*tcb)->Name						= name;
	(*tcb)->Prio						= prio;
	//����ATPCS�涨ARM��ջ����Ϊ8�ֽڶ���,����������䵽��ջ�ռ䲻��8�ֽڶ������Ҫ���м���
	//����ַ�ռ�ĵ���λ����,����ARMջ��FD���ݼ���,ջ�ĳ�ʼָ��ָ�����ջ������ֽڵ�ַ,��
	//0xFFFFFFF8��������������ջ��ָ������ƶ���8�ֽڶ����λ��.�����������ջ���ǵݼ�����
	//��,��ô����ʹ�����ַ�ʽ,����ǵ���������Ҫ��ջָ�������ƶ���8�ֽڶ��봦Ϊֹ.
	(*tcb)->StkPtr						= (u32*)((int)(&temp[stksize-1]) & 0xFFFFFFF8);
	(*tcb)->TskPtr						= task;
	(*tcb)->Flags						= 0;
	(*tcb)->TimDly						= 0;
	(*tcb)->Flags						= OS_READY;
	OS_StkInit((*tcb));
}

void OS_TaskDel(TCB *tcb)
{
	if(tcb == Idle)	//���򲻿�ɾ����������
		return;
	ENTER_CRITICAL;
	OsTcbFlagClr(tcb->Prio);		//��ձ�־λ
	OsTcbTable[tcb->Prio] = null;	//��������������ɾ��
	//�����ڴ�:��ô�����أ���ջ�ռ䶼�����˵Ļ�ջ�Ͳ������˰�
	//��������ƿ��ڴ�����˵Ļ����task��û�˰�,��ô������
	//������Ȩģʽʹ��MSP�����հ�Ҫ��......
	OS_Schedule();					//���µ���
	EXIT_CRITICAL;
}

void OS_TaskReturn(void)
{
	//����һ��ʼ�е�����:
	//�����Ҫִ������ĳ���֮ǰ������һ���ж�Curr���ı���
	//��ôִ�����������ɾ���ǲ��Ǿʹ��ˣ�ɾ�����������ˣ�
	//��ϸ����Ļ�֪�����ⲻ�Ǹ�����,Ҫ֪��һ��ԭ��:
	//ֻҪ�������������Ļ���֮����ôCurr�ض��ǵ�ǰ�������
	//������ƿ��ָ��,���㱻�жϸ�������ٴλص������ʱ��
	//Currָ��ض����Ǳ�����������Ŀ��ƿ�ĵ�ַ.
	ENTER_CRITICAL;
	printf("task %s return\r\n",Curr->Name);
	OS_TaskDel(Curr);
	EXIT_CRITICAL;
}

void OS_StkInit(TCB *task)
{
	u32 *stk = task->StkPtr;
	
	//�ж��Զ�����ͻָ��ļĴ�����
	*(--stk)  = 0x01000000L;					//xPSR
	*(--stk)  = (u32)task->TskPtr;				//task entry
	*(--stk)  = (u32)OS_TaskReturn;				//LR
	*(--stk)  = 0x12121212L;					//R12
	*(--stk)  = 0x03030303L;					//R3
	*(--stk)  = 0x02020202L;					//R2
	*(--stk)  = 0x01010101L;					//R1
	*(--stk)  = 0x00000000L;					//R0
	
	//�ֶ�����ļĴ�����
	*(--stk) = 0x11111111L;                     // R11
	*(--stk) = 0x10101010L;                     // R10
	*(--stk) = 0x09090909L;                     // R9
	*(--stk) = 0x08080808L;                     // R8
	*(--stk) = 0x07070707L;                     // R7
	*(--stk) = 0x06060606L;                     // R6
	*(--stk) = 0x05050505L;                     // R5
	*(--stk) = 0x04040404L;                     // R4
    
	//����ջָ��
	task->StkPtr = stk;
}

void printfTCBList(TCB* tcb)
{
	int i;
	for(i=0;i<64;i++)
	{
		if(OsTcbTable[i] != null)
		{
			printf("(%d:%s) -> ",(int)OsTcbTable[i]->Prio,OsTcbTable[i]->Name);
		}
	}
	printf("\r\n");
}

void OS_Start(void)
{
	SysTick->CTRL |= 0x01 ;		//SYSTICKʹ�� 	 
	Curr = null;	//��ǰû���������е�����
	Next = Idle;	//��������Idle����
	TrigPendSV();
}

//�����������,�����������������κεط����������л�
//��PendSV�쳣�ж����ȼ�����Ϊ���,������ʹ�����жϴ������г���
//PendSV�쳣Ҳ��������ִ���쳣�ж�,���ǵȵ�ǰ�����������ж϶�����
//���֮���ٽ���PendSV�쳣�ж�,�������Ա�֤�Ĵ���������ļĴ���
//�����Ǳ������ж��޸Ĺ��ļĴ���ֵ��
void OS_Schedule(void)
{
	ENTER_CRITICAL;		//�������̲�Ҫ�����
	
	Next = OsTcbTable[OsTcbGetIdeal()];	//��ȡ��ǰ������ȼ���������
	//�����ǰ��������ȼ����ڵ�ǰ������ȼ���������������л�
	if(Curr != Next)
		TrigPendSV();
		
	EXIT_CRITICAL;
}

void OS_TimeTick(void)
{
	TCB *temp = Idle->Next,*temp2;
	SysTicks++;	//ϵͳʱ������Ϊ1msʱ,���㿪ʼ������Լ49�������
	
	if(	StStat )	//����������п�������,����п����������
		StatTicks++;
	if((SysTicks % 1000) == 0)	//1s�ӽ���һ��CPUʹ���ʵļ���
	{
		OS_CPU_Usage = (100 - StatTicks / 10);
		StatTicks = 0;
	}
		
	//��ʱ�ȴ�������������
	while(temp != Idle)
	{
		temp2 = temp->Next;
		if( temp->TimDly == SysTicks )	//��ʱʱ�䵽��,����" >= "������ʽ,��Ϊϵͳ����ֵ�������
		{
			temp->Flags &= ~OS_DELAY;	//�����ʱ�����־λ
			//�����ʱʱ�䵽�˾ͽ��������ʱ�ȴ�����ɾ��
			temp->Prev->Next = temp->Next;
			temp->Next->Prev = temp->Prev;
			
			//����������ڹ���״̬,������Ϊ��ʱ������ȡ������
			if(!(temp->Flags & OS_SUSPEND))
			{
				//��������ɾ���
				OsTcbFlagSet(temp->Prio);	
				OS_Schedule();	//�����л�
			}
		}
		temp = temp2;
	}
// 	OS_Schedule();	//�����л�
}

//����ֻ���������ڲ�����,�������ж��е���
void OS_Sleep(u32 ticks)
{
	//������ʱʱ��͵�ǰϵͳʱ��TimTicks�ĺ�,Ԥ�Ƶ���ʱ��
	if(ticks > 0)
	{
		ENTER_CRITICAL;	//�����ٽ���
		Curr->TimDly = SysTicks + ticks;	//�������,�жϵ�����Ȳ��Ǵ��ڻ�С��
		Curr->Flags |= OS_DELAY;	//����Ϊ��ʱ̬
		
		//����ʱ�ȴ�������뵽��ʱ�ȴ������������
		Curr->Next = Idle;
		Curr->Prev = Idle->Prev;
		Idle->Prev->Next = Curr;
		Idle->Prev = Curr;
	
		OsTcbFlagClr(Curr->Prio);	//��յ�ǰ���������־λ
		EXIT_CRITICAL;	//�˳��ٽ���
		OS_Schedule();	//�����л�
	}
}

void OS_Suspend(TCB *tcb)
{
	if(!(tcb->Flags & OS_SUSPEND))	//���������δ������
	{
		ENTER_CRITICAL;	//�����ٽ���
		tcb->Flags |= OS_SUSPEND;	//����Ϊ����̬		
		OsTcbFlagClr(tcb->Prio);	//������������־λ
		if(tcb == Curr)	//��������������,�����һ���������
		{
			EXIT_CRITICAL;	//�˳��ٽ���
			OS_Schedule();	//�����л�
		}
		EXIT_CRITICAL;	//�˳��ٽ���
	}
}

void OS_Resume(TCB *tcb)
{
	if(tcb->Flags & OS_SUSPEND)	//��������񱻹���
	{
		ENTER_CRITICAL;	//�����ٽ���
		tcb->Flags &= ~OS_SUSPEND;	//��չ����־λ
		
		//�������������ʱ״̬,������Ϊ���������ȡ����ʱ
		if(!(tcb->Flags & OS_DELAY))
			OsTcbFlagSet(tcb->Prio);	//�������������־λ
		
		EXIT_CRITICAL;	//�˳��ٽ���
		OS_Schedule();	//�����л�
	}
}


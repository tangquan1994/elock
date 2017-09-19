#ifndef __OS_CORE_H__
#define __OS_CORE_H__
	#include "sys.h"
	#include "os_lib.h"
	
	int OsTcbGetIdeal(void);
	void OsTcbFlagSet(int prio);
	void OsTcbFlagClr(int prio);
	void PrintFlags(void);
	
	#define ENTER_CRITICAL	__asm("CPSID  I")	//���ж�	
	#define EXIT_CRITICAL	__asm("CPSIE  I")	//���ж�
	
	extern u32 SysTicks;			//ϵͳʱ�Ӽ����� 
	extern struct tcb *Idle;	//root task
	extern struct tcb *Curr;	//point to the current task
	extern struct tcb *Next;	//point to the next ready task
	extern struct tcb *TCBEventHead;		//TCB����ͷ
	extern unsigned int MemPool[];		//4 * 7 = 28KB�ڴ�
	extern unsigned int OS_MEM_Usage;
	
	typedef enum
	{
		OS_READY				= 0x00000000,
		OS_DELAY				= 0x00000001,
		OS_SUSPEND				= 0x00000002,
		OS_SEM					= 0x00000004,
		OS_DEAD					= 0x00000008,
	}OS_TSK_STA;	//task status
	
	typedef enum
	{
		OS_PEND_NOR			= 0,		//normal suspend
		OS_PEND_DLY			= 1,		//delay suspend
		OS_PEND_EVE			= 2,		//event suspend
	}OS_PEND_TYPE;	//task status
	
	/*
	*********************************************************************************************************
	*                                          TASK CONTROL BLOCK
	*********************************************************************************************************
	*/
	typedef struct tcb {
		u32					*StkPtr;						// �����ջָ��,�������TCB���ͷ,��Ϊ�����ֱ��ʹ��TCBָ����Ϊ��ջָ���
		char				*Name;							// ������
		struct tcb			*Prev;							// Pointer to previous TCB in the TCB list
		struct tcb			*Next;							// Pointer to next TCB in the TCB list
		void 				(*TskPtr)(void);				// Pointer to the task service function
// 		u32					TimSli;							// Time slice of task
		u32					Prio;							// Priority of task,higher priority with higher value
// 		u32					Id;								// ����ÿ����������ȼ�������ͬ,���Կ�����Prio��Ϊ�����ID��
		u32					Flags;							// now status of task
		u32					TimDly;							// ��ʱֱ��ϵͳʱ��Ϊ���ֵ,��ʱʱ��Ϊ��ֵ�뵱ǰϵͳʱ��Ĳ�ֵ
		struct event		*Eve;							// �ȴ����ź���
	}TCB;
	extern TCB *OsTcbTable[64];
	extern TCB *OsTcbTable[64];
	extern unsigned char OsTcbFlagFlag;
	
	//��������IdleTask�ǲ���64������֮�ڵ�,�����ķֿ����,������������
	//Ҫ����CPUʹ���ʼ�����ж��Ƿ��о�������,���������о������е�����
	#define OS_ANY_TASK		(OsTcbFlagFlag != 0)	//����OsTcbFlagFlag���ж��Ƿ��о�������
													//��Ϊ0��ʾ��������о���������,Ȼ����
													//��ȡ�������о�����������ȼ�����TCBָ��
													//������ȥ��ÿ�ζ�Ҫ��ȡ�ж�������ȼ�ָ��
													//�Ƿ�Ϊ-1��������������Ƿ��о�������


	/*
	*********************************************************************************************************
	*                                       SEMAPHORE CONTROL BLOCK
	*********************************************************************************************************
	*/
	typedef struct event {
		int Count;
		int Type;

	}EVENT;
	
	typedef enum {
		EVENT_TYPE_SEM,
		EVENT_TYPE_MSG,
	}EVENT_TYPE;
	
	/*
	*********************************************************************************************************
	*                                       MEMERY CONTROL BLOCK
	*********************************************************************************************************
	*/
	typedef struct mem {
		unsigned int ADDR_FLAG;

	}MEM;
	
	#define OS_MEM_FLG		0xFFFFFFFE
	

	/*
	*********************************************************************************************************
	*                                       DEBUG
	*********************************************************************************************************
	*/
	#define _DEBUG
	#ifdef _DEBUG
		#define Debug(msg)	printf("%s-file:%s,line:%d\r\n",msg,__FILE__,__LINE__) 
	#else 
		#define Debug(msg)
	#endif
	
	
	/*
	*********************************************************************************************************
	*                                       FILE
	*********************************************************************************************************
	*/
	typedef enum
	{
		DIR_PRINT_MODE_SIM,
		DIR_PRINT_MODE_FULL,
	}DIR_PRINT_MODE;


	/*
	*********************************************************************************************************
	*                                       DATE_TIME
	*********************************************************************************************************
	*/
	typedef struct DateTime
	{
		u8 year;
		u8 month;
		u8 day;
		u8 hour;
		u8 min;
		u8 second;
		bool is_leap_year;
	}DateTime;
	void OS_TimeRefresh(void);
	extern DateTime LocalTime;

	


	extern void PendSVInit(void);
	extern void TrigPendSV(void);

	extern u32 OS_CPU_Usage;
	extern u32 SysTicks;
	extern u32 StatTicks;

	void OS_Init(void);
	void OS_TaskNew(TCB **tcb,void (*task)(void),u32 stksize,u32 prio,char *name);
	void OS_TaskDel(TCB *tcb);
	void OS_StkInit(TCB *task);
	
	void printfTCBList(TCB* tcb);
	void OS_Start(void);
	void OS_Schedule(void);
	void OS_Sleep(u32 ticks);
	void OS_Suspend(TCB *tcb);
	void OS_Resume(TCB *tcb);

	void OS_SemNew(EVENT **sem,int cnt);
	void OS_SemP(EVENT* sem);
	void OS_SemV(EVENT* sem);
	void OS_SemPend(EVENT* eve);
	void OS_SemPost(EVENT* eve);
	
	void OS_MemInit(unsigned int *mempool,unsigned int size);
	void OS_MemPrint(unsigned int *mempool);
	void OS_MemUsage(unsigned int *mempool);
	void *new(unsigned int *mempool,unsigned int lenth);
	void free(void *addr);
	void OS_MemMerge(unsigned int *mempool);

#endif



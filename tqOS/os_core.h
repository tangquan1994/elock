#ifndef __OS_CORE_H__
#define __OS_CORE_H__
	#include "sys.h"
	#include "os_lib.h"
	
	int OsTcbGetIdeal(void);
	void OsTcbFlagSet(int prio);
	void OsTcbFlagClr(int prio);
	void PrintFlags(void);
	
	#define ENTER_CRITICAL	__asm("CPSID  I")	//关中断	
	#define EXIT_CRITICAL	__asm("CPSIE  I")	//开中断
	
	extern u32 SysTicks;			//系统时钟计数器 
	extern struct tcb *Idle;	//root task
	extern struct tcb *Curr;	//point to the current task
	extern struct tcb *Next;	//point to the next ready task
	extern struct tcb *TCBEventHead;		//TCB链表头
	extern unsigned int MemPool[];		//4 * 7 = 28KB内存
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
		u32					*StkPtr;						// 任务堆栈指针,必须放在TCB的最开头,因为汇编中直接使用TCB指针作为堆栈指针的
		char				*Name;							// 任务名
		struct tcb			*Prev;							// Pointer to previous TCB in the TCB list
		struct tcb			*Next;							// Pointer to next TCB in the TCB list
		void 				(*TskPtr)(void);				// Pointer to the task service function
// 		u32					TimSli;							// Time slice of task
		u32					Prio;							// Priority of task,higher priority with higher value
// 		u32					Id;								// 由于每个人物的优先级都不相同,所以可以用Prio作为人物的ID号
		u32					Flags;							// now status of task
		u32					TimDly;							// 延时直到系统时间为这个值,延时时间为该值与当前系统时间的差值
		struct event		*Eve;							// 等待的信号量
	}TCB;
	extern TCB *OsTcbTable[64];
	extern TCB *OsTcbTable[64];
	extern unsigned char OsTcbFlagFlag;
	
	//空闲任务IdleTask是不在64个任务之内的,单独的分开设计,空闲任务中主
	//要进行CPU使用率计算和判断是否有就绪任务,若有则运行就绪表中的任务
	#define OS_ANY_TASK		(OsTcbFlagFlag != 0)	//根据OsTcbFlagFlag来判断是否有就绪任务
													//不为0表示任务表中有就绪的任务,然后再
													//读取就绪表中就绪的最高优先级任务TCB指针
													//这样免去了每次都要获取判断最高优先级指针
													//是否为-1来决定任务表中是否有就绪任务


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



#include "os_core.h"

//内存池
#define MEMPOOL_SIZE_IN_KB		16
unsigned int __align(8) MemPool[1024 * MEMPOOL_SIZE_IN_KB / 4];

TCB *Idle;				//闲置任务,在所有用户任务都没有运行时运行

u32 OS_CPU_Usage;		//CPU使用率
u32 SysTicks;			//系统时钟计数器 
u32 StatTicks;			//统计时钟计数器
bool StStat;			//统计标志

// TCB *TCBDlyHead;		//延时链表头
TCB *TCBEventHead = null;		//事件链表头

TCB *Curr;				//当前任务指针
TCB *Next;				//即将运行的任务指针

static unsigned char		OsTcbFlag[8];			//任务标志位数组,一个字节表示一个组,一共8组,每组8个任务
													//一共64位用于表示64个任务的就绪标志,为1表示任务就绪,为0表示为就绪
													
unsigned char				OsTcbFlagFlag;			//任务组就绪标志,用于表示每一组(8个任务)中是否有就绪任务,如果某一组
													//任务中有就绪的任务,则OsTcbFlagFlag相应偏移量的位置1否则清零


static const unsigned char	OsMaskTable[] =			//掩码数组
{
	0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80
};

static const unsigned char	OsMapTable[256] =		//任务优先级映射表
{
	255,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,	/*255表示没有位被置1*/
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

TCB *OsTcbTable[64];			//任务表8*8,一共64个任务,每个任务一个优先级0-63,数值越高表示
								//优先级越高,如果表中的元素为null,表示该优先级的任务没有使用
								
								
								
						
int OsTcbGetIdeal(void)
{	
	int temp = OsMapTable[OsTcbFlagFlag];	//temp表示最高的就绪任务所在的任务组的偏移量(0~7)
	if(temp == 255)	/*255表示没有就绪任务*/
		return -1;	//没有就绪任务返回-1
	return ( (temp << 3) + OsMapTable[OsTcbFlag[temp]] );	//优先级等于就绪任务组号乘以8加上任务的组内偏移
}

void OsTcbFlagSet(int prio)
{
	int x,y;
	if(prio > 63)
		return;
	x = prio & 0x07;	//prio % 8;
	y = prio >> 3;		//prio / 8;
	OsTcbFlagFlag |= OsMaskTable[y];	//组标志
	OsTcbFlag[y]  |= OsMaskTable[x];	//组内标志
}

void OsTcbFlagClr(int prio)
{
	int x,y;
	if(prio > 63)
		return;
	x = prio & 0x07;	//prio % 8;
	y = prio >> 3;		//prio / 8;
	OsTcbFlag[y] &= ~OsMaskTable[x];	//组内标志
	if(OsTcbFlag[y] == 0)	//如果该组别没有就绪任务了,就将OsTcbFlagFlag中的相应位清零
		OsTcbFlagFlag &= ~OsMaskTable[y];	//组标志
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
	//根任务负责任务调度和事件管理
	while(1)
	{
		StStat = true;	//CPU使用率计算原理:
						//当空闲任务运行时,没进行一次时钟中断就进行一次统计计数
						//也就是在统计空闲任务运行的时间总和,1s钟之后计算统计计
						//数与总计数的比率,结果就是空闲率,1减去空闲率就是使用率
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
	
	//内存池初始化
	OS_MemInit(MemPool,sizeof(MemPool));
		
	//创建空闲任务Idle
	OS_TaskNew(&Idle,IdleTask,100,0,"Idle");

	//为了节省空间,使用Idle作为延时链表的表头,可以省下创建TCBDlyHead任务控制块的空间
	Idle->Next = Idle->Prev = Idle;	//循环链表
		
	//PendSV软中断初始化,优先级设置为最低
	PendSVInit();
}

void OS_TaskNew(TCB **tcb,void (*task)(void),u32 stksize,u32 prio,char *name)
{
	u32 *temp;
	//为任务TCB分配空间
	*tcb = new(MemPool,sizeof(TCB));
	
	//如果任务表中该优先级的位置已经有任务占用,则返回,任务创建失败
	if( OsTcbTable[prio] != null )
	{
		printf("error:任务已存在\r\n");
		return;
	}
	//如果任务表中该优先级的位置为null表示未被占用,则进行赋值
	( (TCB **)OsTcbTable)[prio] = *tcb;
	
	//设置任务就绪标志位
	OsTcbFlagSet(prio);
	
	//为任务堆栈分配空间
	temp = new(MemPool,stksize*4);
	//还需要在TCB中添加一个记录堆栈分配地址的字段和一个堆栈大小的字段,方便进行内存回收

	(*tcb)->Name						= name;
	(*tcb)->Prio						= prio;
	//由于ATPCS规定ARM的栈必须为8字节对齐,所以如果分配到的栈空间不是8字节对齐的需要进行剪裁
	//将地址空间的低三位清零,由于ARM栈是FD满递减的,栈的初始指针指向的是栈的最高字节地址,和
	//0xFFFFFFF8相与后可以做到将栈顶指针向低移动至8字节对齐的位置.如果处理器的栈不是递减生长
	//的,那么不能使用这种方式,如果是递增的则需要将栈指针向上移动至8字节对齐处为止.
	(*tcb)->StkPtr						= (u32*)((int)(&temp[stksize-1]) & 0xFFFFFFF8);
	(*tcb)->TskPtr						= task;
	(*tcb)->Flags						= 0;
	(*tcb)->TimDly						= 0;
	(*tcb)->Flags						= OS_READY;
	OS_StkInit((*tcb));
}

void OS_TaskDel(TCB *tcb)
{
	if(tcb == Idle)	//万万不可删除空闲任务
		return;
	ENTER_CRITICAL;
	OsTcbFlagClr(tcb->Prio);		//清空标志位
	OsTcbTable[tcb->Prio] = null;	//将任务从任务表中删除
	//回收内存:怎么回收呢？把栈空间都回收了的话栈就不能用了啊
	//把任务控制块内存回收了的话这个task就没了啊,怎么回收呢
	//进入特权模式使用MSP来回收吧要不......
	OS_Schedule();					//重新调度
	EXIT_CRITICAL;
}

void OS_TaskReturn(void)
{
	//这里一开始有点疑虑:
	//如果在要执行下面的程序之前进行了一次中断Curr被改变了
	//那么执行下面的任务删除是不是就错了？删除错了任务了？
	//仔细想想的话知道了这不是个问题,要知道一个原理:
	//只要运行在这个任务的环境之中那么Curr必定是当前的任务的
	//任务控制块的指针,就算被中断给打断了再次回到这里的时候
	//Curr指针必定又是变成了这个任务的控制块的地址.
	ENTER_CRITICAL;
	printf("task %s return\r\n",Curr->Name);
	OS_TaskDel(Curr);
	EXIT_CRITICAL;
}

void OS_StkInit(TCB *task)
{
	u32 *stk = task->StkPtr;
	
	//中断自动保存和恢复的寄存器组
	*(--stk)  = 0x01000000L;					//xPSR
	*(--stk)  = (u32)task->TskPtr;				//task entry
	*(--stk)  = (u32)OS_TaskReturn;				//LR
	*(--stk)  = 0x12121212L;					//R12
	*(--stk)  = 0x03030303L;					//R3
	*(--stk)  = 0x02020202L;					//R2
	*(--stk)  = 0x01010101L;					//R1
	*(--stk)  = 0x00000000L;					//R0
	
	//手动保存的寄存器组
	*(--stk) = 0x11111111L;                     // R11
	*(--stk) = 0x10101010L;                     // R10
	*(--stk) = 0x09090909L;                     // R9
	*(--stk) = 0x08080808L;                     // R8
	*(--stk) = 0x07070707L;                     // R7
	*(--stk) = 0x06060606L;                     // R6
	*(--stk) = 0x05050505L;                     // R5
	*(--stk) = 0x04040404L;                     // R4
    
	//更新栈指针
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
	SysTick->CTRL |= 0x01 ;		//SYSTICK使能 	 
	Curr = null;	//当前没有正在运行的任务
	Next = Idle;	//首先运行Idle任务
	TrigPendSV();
}

//进行任务调度,可以随心所欲的在任何地方进行任务切换
//将PendSV异常中断优先级设置为最低,这样即使是在中断处理函数中出发
//PendSV异常也不会立即执行异常中断,而是等当前所有其他的中断都处理
//完成之后再进入PendSV异常中断,这样可以保证寄存器是任务的寄存器
//而不是被其他中断修改过的寄存器值。
void OS_Schedule(void)
{
	ENTER_CRITICAL;		//整个过程不要被打断
	
	Next = OsTcbTable[OsTcbGetIdeal()];	//获取当前最高优先级任务索引
	//如果当前任务的优先级低于当前最高优先级任务则进行任务切换
	if(Curr != Next)
		TrigPendSV();
		
	EXIT_CRITICAL;
}

void OS_TimeTick(void)
{
	TCB *temp = Idle->Next,*temp2;
	SysTicks++;	//系统时钟周期为1ms时,从零开始计数大约49天后会溢出
	
	if(	StStat )	//如果正在运行空闲任务,则进行空闲任务计数
		StatTicks++;
	if((SysTicks % 1000) == 0)	//1s钟进行一次CPU使用率的计算
	{
		OS_CPU_Usage = (100 - StatTicks / 10);
		StatTicks = 0;
	}
		
	//延时等待挂起任务链表
	while(temp != Idle)
	{
		temp2 = temp->Next;
		if( temp->TimDly == SysTicks )	//定时时间到达,不用" >= "这种形式,因为系统计数值可能溢出
		{
			temp->Flags &= ~OS_DELAY;	//清空延时挂起标志位
			//如果延时时间到了就将任务从延时等待表中删除
			temp->Prev->Next = temp->Next;
			temp->Next->Prev = temp->Prev;
			
			//如果该任务处于挂起状态,不会因为延时结束被取消挂起
			if(!(temp->Flags & OS_SUSPEND))
			{
				//设置任务成就绪
				OsTcbFlagSet(temp->Prio);	
				OS_Schedule();	//任务切换
			}
		}
		temp = temp2;
	}
// 	OS_Schedule();	//任务切换
}

//函数只能在任务内部调用,不可在中断中调用
void OS_Sleep(u32 ticks)
{
	//计算延时时间和当前系统时间TimTicks的和,预计到达时间
	if(ticks > 0)
	{
		ENTER_CRITICAL;	//进入临界区
		Curr->TimDly = SysTicks + ticks;	//可以溢出,判断的是相等不是大于或小于
		Curr->Flags |= OS_DELAY;	//设置为延时态
		
		//将延时等待任务加入到延时等待挂起任务表中
		Curr->Next = Idle;
		Curr->Prev = Idle->Prev;
		Idle->Prev->Next = Curr;
		Idle->Prev = Curr;
	
		OsTcbFlagClr(Curr->Prio);	//清空当前任务就绪标志位
		EXIT_CRITICAL;	//退出临界区
		OS_Schedule();	//任务切换
	}
}

void OS_Suspend(TCB *tcb)
{
	if(!(tcb->Flags & OS_SUSPEND))	//如果该任务未被挂起
	{
		ENTER_CRITICAL;	//进入临界区
		tcb->Flags |= OS_SUSPEND;	//设置为挂起态		
		OsTcbFlagClr(tcb->Prio);	//清空任务就绪标志位
		if(tcb == Curr)	//如果挂起的是自身,则进行一次任务调度
		{
			EXIT_CRITICAL;	//退出临界区
			OS_Schedule();	//任务切换
		}
		EXIT_CRITICAL;	//退出临界区
	}
}

void OS_Resume(TCB *tcb)
{
	if(tcb->Flags & OS_SUSPEND)	//如果该任务被挂起
	{
		ENTER_CRITICAL;	//进入临界区
		tcb->Flags &= ~OS_SUSPEND;	//清空挂起标志位
		
		//如果该任务处于延时状态,不会因为挂起结束被取消延时
		if(!(tcb->Flags & OS_DELAY))
			OsTcbFlagSet(tcb->Prio);	//设置任务就绪标志位
		
		EXIT_CRITICAL;	//退出临界区
		OS_Schedule();	//任务切换
	}
}


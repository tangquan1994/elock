#include "os_core.h"

//新建信号量
void OS_SemNew(EVENT **sem,int cnt)
{
	*sem = new(MemPool,sizeof(EVENT));
	(*sem)->Type = EVENT_TYPE_SEM;
	(*sem)->Count = cnt;
}

//信号量PV原语
void OS_SemPend(EVENT* sem)
{
	if(sem->Type == EVENT_TYPE_SEM)
	{
		if(sem->Count == 0)
		{
			Curr->Flags |= OS_SEM;
			Curr->Eve = sem;

			if(TCBEventHead == null)
			{
				TCBEventHead = Curr;
				TCBEventHead->Next = TCBEventHead;
				TCBEventHead->Prev = TCBEventHead;
			}
			else
			{
				//将任务加入到事件等待挂起任务表中
				Curr->Next = TCBEventHead;
				Curr->Prev = TCBEventHead->Prev;
				TCBEventHead->Prev->Next = Curr;
				TCBEventHead->Prev = Curr;
			}
			
			OS_Schedule();	//任务切换
		}
		else
		{
			sem->Count --;
		}
	}
}

void OS_SemPost(EVENT* sem)
{
	TCB *temp = TCBEventHead,*temp2;
	if(sem->Type == EVENT_TYPE_SEM)
	{
		sem->Count ++;	//如果信号量加一
		
		//事件等待挂起任务链表
		do
		{
			temp2 = temp->Next;
			if(temp->Eve->Type == EVENT_TYPE_SEM && temp->Eve->Count > 0)
			{
				temp->Eve->Count --;
				temp->Flags &= ~OS_SEM;	//清空信号量等待标志位

				//如果删除的是链表头
				if(temp == TCBEventHead)
					TCBEventHead = temp2;
				temp->Prev->Next = temp->Next;
				temp->Next->Prev = temp->Prev;

				//设置任务成就绪
				OsTcbFlagSet(temp->Prio);
			}
			temp = temp2;
		}while(temp != TCBEventHead);
	}
}




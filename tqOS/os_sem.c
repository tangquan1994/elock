#include "os_core.h"

//�½��ź���
void OS_SemNew(EVENT **sem,int cnt)
{
	*sem = new(MemPool,sizeof(EVENT));
	(*sem)->Type = EVENT_TYPE_SEM;
	(*sem)->Count = cnt;
}

//�ź���PVԭ��
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
				//��������뵽�¼��ȴ������������
				Curr->Next = TCBEventHead;
				Curr->Prev = TCBEventHead->Prev;
				TCBEventHead->Prev->Next = Curr;
				TCBEventHead->Prev = Curr;
			}
			
			OS_Schedule();	//�����л�
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
		sem->Count ++;	//����ź�����һ
		
		//�¼��ȴ�������������
		do
		{
			temp2 = temp->Next;
			if(temp->Eve->Type == EVENT_TYPE_SEM && temp->Eve->Count > 0)
			{
				temp->Eve->Count --;
				temp->Flags &= ~OS_SEM;	//����ź����ȴ���־λ

				//���ɾ����������ͷ
				if(temp == TCBEventHead)
					TCBEventHead = temp2;
				temp->Prev->Next = temp->Next;
				temp->Next->Prev = temp->Prev;

				//��������ɾ���
				OsTcbFlagSet(temp->Prio);
			}
			temp = temp2;
		}while(temp != TCBEventHead);
	}
}




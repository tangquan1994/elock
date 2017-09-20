/***********************************************************************************
  Filename:     util_buffer.c

  Description:  Ringbuffer implementation.

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include "util_queue.h"


void QueueInit(Queue *q)
{
    q->front= 0;
    q->rear= 0;
	q->count = 0;
	q->maxsize = BUF_SIZE;
}

bool IsEmpty(Queue *Q)
{
    if(Q->front==Q->rear)    //ÅĞ¶ÏÊÇ·ñÎª¿Õ  
        return true;  
    else  
        return false;  
}

bool IsFull(Queue *Q)
{
    if(Q->front==(Q->rear+1)%Q->maxsize)    //ÅĞ¶ÏÑ­»·Á´±íÊÇ·ñÂú£¬ÁôÒ»¸öÔ¤Áô¿Õ¼ä²»ÓÃ  
        return true;  
    else  
        return false;  
}

bool QueuePut(Queue *Q,uint8 data)
{
    if(IsFull(Q))  
        return false;  
    else  
    {  
        Q->buff[Q->rear]=data;  
        Q->rear=(Q->rear+1)%Q->maxsize;
		Q->count++;
        return true;  
    }  
}

bool QueueGet(Queue *Q,uint8 *data)
{
    if(IsEmpty(Q))  
    {  
        return false;  
    }  
    else  
    {  
        *data=Q->buff[Q->front];  
        Q->front=(Q->front+1)%Q->maxsize;  
		Q->count--;
        return true;  
    }  
}

uint8 QueueGet2(Queue *Q)
{
	uint8 temp;
    if(IsEmpty(Q))  
    {  
        return 0;  
    }  
    else  
    {  
        temp=Q->buff[Q->front];  
        Q->front=(Q->front+1)%Q->maxsize;  
		Q->count--;
        return temp;  
    }  
}

void QueueFlush(Queue *q)
{
	q->front= 0;
	q->rear= 0;
	q->count = 0;
}









/***********************************************************************************

  Filename:     util_buffer.h

  Description:  Ringbuffer interface

***********************************************************************************/
#ifndef UTIL_QUEUE_H
#define UTIL_QUEUE_H

/***********************************************************************************
* INCLUDES
*/
#include "SYS.h"

#define uint8 u8
#define uint16 u16

/***********************************************************************************
* CONSTANTS AND DEFINES
*/
#define BUF_SIZE	256

/***********************************************************************************
* TYPEDEFS
*/
typedef struct {
    uint8 buff[BUF_SIZE];
    uint16 front;
    uint16 rear;
    uint16 count;
	uint16 maxsize;
} Queue;

/***********************************************************************************
* MACROS
*/
#define bufFlush(pBuf)  bufInit(pBuf)

/***********************************************************************************
* GLOBAL FUNCTIONS
*/
void  QueueInit(Queue *q);
bool IsEmpty(Queue *Q);
bool IsFull(Queue *Q);
bool QueuePut(Queue *Q,uint8 data);
bool QueueGet(Queue *Q,uint8 *data);
uint8 QueueGet2(Queue *Q);
void QueueFlush(Queue *Q);

#endif








/*******************************************************************************
 * @file    my_queue.h
 * @brief
 * @author  chengjie
 * @date    created on 2026-02-10
 * @version V1.0
 * @note
*******************************************************************************/
#ifndef __my_queue_h__
#define __my_queue_h__

#include "stdint.h"

typedef uint32_t Queue_BaseType_t;
typedef struct 
{
  Queue_BaseType_t mutex;
  Queue_BaseType_t offset;
  Queue_BaseType_t usedLen;
  Queue_BaseType_t queueMemSize;
  uint8_t *pQueueMem;
}MYQUEUE_Type_t;

int8_t MyQueue_CreateStatic(MYQUEUE_Type_t *const pQueueHandle,
                            uint8_t *const pQueueMem,
                            const Queue_BaseType_t memSize);
int8_t MyQueue_GetItemCount(MYQUEUE_Type_t *const pQueueHandle,
                            Queue_BaseType_t *pItemCount);
int8_t MyQueue_Write(MYQUEUE_Type_t *const pQueueHandle,
                      const uint8_t *pData,
                      const Queue_BaseType_t dataLen);
int8_t MyQueue_Read(MYQUEUE_Type_t *const pQueueHandle,
                    uint8_t *const pData,
                    const Queue_BaseType_t dataLen);


#endif /* __my_queue_h__ */


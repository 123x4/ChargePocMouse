/*******************************************************************************
 * @file    my_queue.c
 * @brief
 * @author  chengjie
 * @date    created on 2026-02-10
 * @version V1.0
 * @note    V1.0 does not support simultaneous reading and writing
*******************************************************************************/

#include "my_queue.h"

int8_t MyQueue_CreateStatic(MYQUEUE_Type_t *const pQueueHandle,
                            uint8_t *const pQueueMem,
                            const Queue_BaseType_t memSize)
{
  if(0 == pQueueHandle || 0 == pQueueMem || 0 ==memSize)
  {
    return -1;
  }
  
  if(pQueueHandle->mutex)
  {
    return -2;
  }
  else
  {
    pQueueHandle->mutex = 1;//lock
    pQueueHandle->offset = 0;
    pQueueHandle->usedLen = 0;
    pQueueHandle->queueMemSize = memSize;
    pQueueHandle->pQueueMem = pQueueMem;
    pQueueHandle->mutex = 0;//Unlock
  }
  return 0;
}

int8_t MyQueue_GetItemCount(MYQUEUE_Type_t *const pQueueHandle,
                            Queue_BaseType_t *pItemCount)
{
  if(0 == pQueueHandle || 0 == pItemCount)
  {
    return -1;
  }
  *pItemCount = pQueueHandle->usedLen;
  return 0;
}

int8_t MyQueue_Write(MYQUEUE_Type_t *const pQueueHandle,
                    const uint8_t *pData,
                    const Queue_BaseType_t dataLen)
{
  Queue_BaseType_t i = 0;
  
  if(0 == pQueueHandle || 0 == pData || 0 == dataLen)
  {
    return -1;
  }
  
  if(pQueueHandle->mutex)
  {
    return -2;
  }
  else
  {
    pQueueHandle->mutex = 1;//Lock
    if(dataLen > pQueueHandle->queueMemSize - pQueueHandle->usedLen)
    {
      pQueueHandle->mutex = 0;//Unlock
      return -3;
    }
    
    for(i = 0; i < dataLen; i++)
    {
      *(pQueueHandle->pQueueMem + pQueueHandle->offset) = *(pData + i);
      pQueueHandle->usedLen += 1;
      pQueueHandle->offset += 1;
      if(pQueueHandle->offset >= pQueueHandle->queueMemSize)
      {
        pQueueHandle->offset = 0;
      }
    }
    pQueueHandle->mutex = 0;//Unlock
  }
  return 0;
}

int8_t MyQueue_Read(MYQUEUE_Type_t *const pQueueHandle,
                    uint8_t *const pData,
                    const Queue_BaseType_t dataLen)
{
  Queue_BaseType_t i = 0;
  
  if(0 == pQueueHandle || 0 == pData || 0 == dataLen)
  {
    return -1;
  }
  
  if(pQueueHandle->mutex)
  {
    return -2;
  }
  else
  {
    pQueueHandle->mutex = 1;//Lock
    if(!(pQueueHandle->usedLen) || dataLen > pQueueHandle->usedLen)
    {
      pQueueHandle->mutex = 0;//Unlock
      return -3;
    }
    
    for(i = 0; i < dataLen; i++)
    {
      if(pQueueHandle->usedLen == pQueueHandle->queueMemSize)
      {
        *(pData + i) = *(pQueueHandle->pQueueMem + pQueueHandle->offset);
        pQueueHandle->usedLen -= 1;
      }
      else if(pQueueHandle->usedLen > pQueueHandle->offset)
      {
        *(pData + i) = *(pQueueHandle->pQueueMem + pQueueHandle->queueMemSize
                          - pQueueHandle->usedLen + pQueueHandle->offset);
        pQueueHandle->usedLen -= 1;
      }
      else //if(pQueueHandle->usedLen <= pQueueHandle->offset)
      {
        *(pData + i) = *(pQueueHandle->pQueueMem + pQueueHandle->offset
                          - pQueueHandle->usedLen);
        pQueueHandle->usedLen -= 1;
      }
    }
    pQueueHandle->mutex = 0;//Unlock
  }
  return 0;
}



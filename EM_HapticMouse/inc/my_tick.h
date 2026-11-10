/*******************************************************************************
 * @file    my_tick.h
 * @brief
 * @author  chengjie
 * @date    created on 2026-02-10
 * @version V1.0
 * @note
*******************************************************************************/

#ifndef __my_tick_h__
#define __my_tick_h__

#include "stdint.h"

#define POLLING_INTERVAL_MS(x) do{ \
  static uint32_t lastStamp = 0; \
  uint32_t nowStamp = MyTick_GetTickMs(); \
  if(nowStamp - lastStamp < (x)) {return;} \
  lastStamp = nowStamp; \
}while(0)

void MyTick_IncreaseSysTickMs(void);
uint32_t MyTick_GetTickMs(void);
void MyTick_DelayMs(uint32_t ms);



#endif /* __my_tick_h__ */




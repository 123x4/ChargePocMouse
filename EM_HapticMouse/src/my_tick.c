/*******************************************************************************
 * @file    my_tick.c
 * @brief
 * @author  chengjie
 * @date    created on 2026-02-10
 * @version V1.0
 * @note
*******************************************************************************/

#include "my_tick.h"

#define MY_TICK_MAX_DELAY 0xFFFFFFFF

static uint32_t mySysTickMs = 0;


void MyTick_IncreaseSysTickMs(void)
{
  mySysTickMs += 1;
}

uint32_t MyTick_GetTickMs(void)
{
  return mySysTickMs;
}

void MyTick_DelayMs(uint32_t ms)
{
  uint32_t tickStart = MyTick_GetTickMs();
  uint32_t wait = ms;
  
  if(wait < MY_TICK_MAX_DELAY)
  {
    wait += 1;
  }
  while(MyTick_GetTickMs() - tickStart < wait)
  {
    ;
  }
}

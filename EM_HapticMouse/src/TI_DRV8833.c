/*******************************************************************************
 * @file    TI_DRV8833.c
 * @brief
 * @author  chengjie
 * @date    created on 2026-06-25
 * @version V0.1
 * @note
*******************************************************************************/
#include "TI_DRV8833.h"

#if (defined(GEN_2_H_V1))

int32_t TI_DRV8833_Init(TI_DRV8833_ts *p)
{
  return 0;
}

void TI_DRV8833_Deinit(TI_DRV8833_ts *p)
{
  TI_DRV8833_EnterSleepMode(p);
}

void TI_DRV8833_EnterSleepMode(TI_DRV8833_ts *p)
{
  IOCTRL_LowLevel(p->io.nSleepPort, p->io.nSleepPin);
}

void TI_DRV8833_ExitSleepMode(TI_DRV8833_ts *p)
{
  IOCTRL_HighLevel(p->io.nSleepPort, p->io.nSleepPin);
}

void TI_DERV8833_Coast(TI_DRV8833_ts *p, TI_DRV8833_Bridge_te bridge)
{
  switch(bridge)
  {
    case BRIDGE_A:
    {
      IOCTRL_LowLevel(p->io.AIN1Port, p->io.AIN1Pin);
      IOCTRL_LowLevel(p->io.AIN2Port, p->io.AIN2Pin);
      break;
    }
    case BRIDGE_B:
    {
      IOCTRL_LowLevel(p->io.BIN1Port, p->io.BIN1Pin);
      IOCTRL_LowLevel(p->io.BIN2Port, p->io.BIN2Pin);
      break;
    }
  }
}

void TI_DERV8833_Reverse(TI_DRV8833_ts *p, TI_DRV8833_Bridge_te bridge)
{
  switch(bridge)
  {
    case BRIDGE_A:
    {
      IOCTRL_LowLevel(p->io.AIN1Port, p->io.AIN1Pin);
      IOCTRL_HighLevel(p->io.AIN2Port, p->io.AIN2Pin);
      break;
    }
    case BRIDGE_B:
    {
      IOCTRL_LowLevel(p->io.BIN1Port, p->io.BIN1Pin);
      IOCTRL_HighLevel(p->io.BIN2Port, p->io.BIN2Pin);
      break;
    }
  }
}

void TI_DERV8833_Forward(TI_DRV8833_ts *p, TI_DRV8833_Bridge_te bridge)
{
  switch(bridge)
  {
    case BRIDGE_A:
    {
      IOCTRL_HighLevel(p->io.AIN1Port, p->io.AIN1Pin);
      IOCTRL_LowLevel(p->io.AIN2Port, p->io.AIN2Pin);
      break;
    }
    case BRIDGE_B:
    {
      IOCTRL_HighLevel(p->io.BIN1Port, p->io.BIN1Pin);
      IOCTRL_LowLevel(p->io.BIN2Port, p->io.BIN2Pin);
      break;
    }
  }
}

void TI_DERV8833_Brake(TI_DRV8833_ts *p, TI_DRV8833_Bridge_te bridge)
{
  switch(bridge)
  {
    case BRIDGE_A:
    {
      IOCTRL_HighLevel(p->io.AIN1Port, p->io.AIN1Pin);
      IOCTRL_HighLevel(p->io.AIN2Port, p->io.AIN2Pin);
      break;
    }
    case BRIDGE_B:
    {
      IOCTRL_HighLevel(p->io.BIN1Port, p->io.BIN1Pin);
      IOCTRL_HighLevel(p->io.BIN2Port, p->io.BIN2Pin);
      break;
    }
  }
}

#endif









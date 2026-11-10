/*******************************************************************************
 * @file    TI_DRV8833.h
 * @brief
 * @author  chengjie
 * @date    created on 2026-06-25
 * @version V0.1
 * @note
*******************************************************************************/
#ifndef __TI_DRV8833_h__
#define __TI_DRV8833_h__

#include "stdint.h"
#include "my_board.h"


#if (defined(GEN_2_H_V1))

typedef struct{
  uint8_t nSleepPort;
  uint8_t nSleepPin;
  
  uint8_t AIN1Port;
  uint8_t AIN1Pin;
  uint8_t AIN2Port;
  uint8_t AIN2Pin;
  
  uint8_t BIN1Port;
  uint8_t BIN1Pin;
  uint8_t BIN2Port;
  uint8_t BIN2Pin;
}TI_DRV8833_IO_ts;

typedef struct{
  TI_DRV8833_IO_ts io;
}TI_DRV8833_ts;

typedef enum{
  BRIDGE_A,
  BRIDGE_B
}TI_DRV8833_Bridge_te;


int32_t TI_DRV8833_Init(TI_DRV8833_ts *p);
void TI_DRV8833_Deinit(TI_DRV8833_ts *p);
void TI_DRV8833_EnterSleepMode(TI_DRV8833_ts *p);
void TI_DRV8833_ExitSleepMode(TI_DRV8833_ts *p);
void TI_DERV8833_Coast(TI_DRV8833_ts *p, TI_DRV8833_Bridge_te bridge);
void TI_DERV8833_Reverse(TI_DRV8833_ts *p, TI_DRV8833_Bridge_te bridge);
void TI_DERV8833_Forward(TI_DRV8833_ts *p, TI_DRV8833_Bridge_te bridge);
void TI_DERV8833_Brake(TI_DRV8833_ts *p, TI_DRV8833_Bridge_te bridge);

#endif

#endif /* __TI_DRV8833_h__ */


/*******************************************************************************
 * @file    my_adc.h
 * @brief
 * @author  chengjie
 * @date    created on 2026-08-24
 * @version V0.1
 * @note
*******************************************************************************/
#ifndef __my_adc_h__
#define __my_adc_h__


#include "stdint.h"
#include "my_board.h"

#if (defined(IRPT_V4))

typedef struct{
  uint8_t ch;
  uint8_t port;
  uint8_t pin;
  volatile uint32_t *pFuncReg;
}MyADC_ts;

extern MyADC_ts *pLeftAdcHandle;
extern MyADC_ts *pRightAdcHandle;


int32_t MyADC_Init(MyADC_ts *p);
int32_t MyADC_GetValue(MyADC_ts *p, uint16_t *pVal);
void MyADC_Poll_Test(void);

#endif

#endif /* __my_adc_h__ */






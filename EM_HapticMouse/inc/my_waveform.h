/*******************************************************************************
 * @file    my_wareform.h
 * @brief   waveform driver based on LPC11U35 CT32B0 timer
 * @author  chengjie.deng@gtech.com.cn
 * @date    created on 2026-07-14
 * @version 
 * @copyright Copyright (c) 2026 G.TECH TECHNOLOGY LTD.
 * @note    
*******************************************************************************/
#ifndef __my_waveform_h__
#define __my_waveform_h__

#include "stdint.h"
#include "my_board.h"

#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))

#define FREQ_HZ_TO_PERIOD_US(x) ((1000000u) / (x))
#define PERIOD_US_TO_FREQ_HZ(x) ((1000000u) / (x))

typedef enum
{
  WF_STATE_UNINIT = 0,
  WF_STATE_STOPPED,
  WF_STATE_RUNNING,
}WaveformState_te;

typedef enum
{
  CHANNEL_0 = 0,
  CHANNEL_1,
}WaveformChannel_te;

typedef struct __attribute__((aligned(4)))
{
  const uint8_t *pWfData;
  uint16_t wfDataLen;
}WaveformInfo_ts;


#if MYDEBUG_ENABLE
extern uint32_t costTime;
#endif

int32_t MyWaveform_Init(void);
int32_t MyWaveform_StartGenerate(WaveformChannel_te ch, WaveformInfo_ts *pInfo, uint32_t wfPeriodUs, uint8_t cycles);
int32_t MyWaveform_StopGenerate(WaveformChannel_te ch);
WaveformState_te MyWaveform_GetGenerateStatus(WaveformChannel_te ch);
#endif /* GT_LINA_T1A_H5_PWM */

#endif /* __my_waveform_h__ */




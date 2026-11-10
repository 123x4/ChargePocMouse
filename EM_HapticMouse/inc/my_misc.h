/*******************************************************************************
 * @file    my_misc.h
 * @brief
 * @author  chengjie
 * @date    created on 2026-03-16
 * @version V0.1
 * @note
*******************************************************************************/

#ifndef __my_misc_h__
#define __my_misc_h__

#include "stdint.h"
#include "my_hallkey_func.h"
#include "my_protocol_common.h"
#include "my_board.h"


#if (defined(HAPTIC_V2) || defined (HAPTIC_V3))
  #define PRESS_KEY_AND_VALUE_INCREASE  (1)
#elif (defined(HAPTIC_V4))
#if (defined(HAPTIC_V4_PWM))
  #define PRESS_KEY_AND_VALUE_INCREASE  (0)
#else
  #define PRESS_KEY_AND_VALUE_INCREASE  (1)
#endif
#elif (defined(P723_POC))
  #define PRESS_KEY_AND_VALUE_INCREASE  (0)
#elif (defined(GEN_2_H_V1))
  #define PRESS_KEY_AND_VALUE_INCREASE  (1)
#elif (defined(IRPT_V4))
  #define PRESS_KEY_AND_VALUE_INCREASE  (0)
#elif (defined(V4_HALL_HPTIC_V4))
  #if(defined(V4_HALL_HPTIC_V4_G2_0923))
  #define PRESS_KEY_AND_VALUE_INCREASE  (1)
  #else
  #define PRESS_KEY_AND_VALUE_INCREASE  (0)
  #endif
#elif (defined(GT_LINA_T1A_H5))
#if (defined(GT_LINA_T1A_H5_PWM))
    #define PRESS_KEY_AND_VALUE_INCREASE  (0)
#else
    #define PRESS_KEY_AND_VALUE_INCREASE  (1)
#endif
#else
  #define PRESS_KEY_AND_VALUE_INCREASE  (1)
#endif

#define PRESS_KEY_AND_VALUE_DECREASE  (!(PRESS_KEY_AND_VALUE_INCREASE))

#define MAX_SEGMENT   100
#define DEFAULT_AP    10
#define DEFAULT_RTP   30
#define DEFAULT_RTR   20


#define HALL_LIMIT_MAX_VALUE    15927
#define HALL_LIMIT_MIN_VALUE    457
#define MAX_VALUE_14BIT         16383
#define MAX_VALUE_10BIT         1023

extern uint8_t gbKeyState[ANA_TOTAL_COUNT];


void Hall_UpdateParam(uint8_t virtualKey, const RapidTriggerConfig_ts *pRTcfg);
int32_t Hall_InitBaseline(uint16_t keyIndex, uint16_t rawData, uint16_t *const pTopBaseline, uint16_t *const pBottomBaseline);
void Hall_CalibrateTopBaseline(uint16_t keyIndex, uint8_t nowKeySta, uint16_t rawData, uint16_t *const pTopBaseline);
void Hall_CalibrateBottomBaseline(uint16_t keyIndex, uint8_t nowKeySta, uint16_t rawData, uint16_t *const pBottomBaseline);
uint16_t Hall_CalculateNode(uint16_t rawData, uint16_t bottomBaseline, uint16_t topBaseline);
void Hall_HandleKeyRapidTrigger(uint8_t keyIndex, uint16_t segment);
uint8_t ProcessRecvSampleRawData(uint8_t* pData, uint16_t length);

#endif /* __my_misc_h__ */


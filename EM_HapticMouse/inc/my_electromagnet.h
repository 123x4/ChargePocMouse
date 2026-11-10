/*******************************************************************************
 * @file    my_electromagnet.h
 * @brief
 * @author  chengjie
 * @date    created on 2026-06-26
 * @version V0.1
 * @note
*******************************************************************************/
#ifndef __my_electromagnet_h__
#define __my_electromagnet_h__


#include "LPC11Uxx.h"
#include "GPIO.h"
#include "my_debug.h"
#include "my_tick.h"
#include "my_protocol_common.h"
#include "my_misc.h"

typedef enum
{
  ELE_POS_LEFT,
  ELE_POS_RIGHT
}Ele_Position_te;

#define DEFAULT_CFG_ONTIME_US             500 //500us
#define DEFAULT_CFG_BRAKE_EN              0   //0: diable, 1: enable
#define DEFAULT_CFG_BRAKE_DELAY_US        10  //10us
#define DEFAULT_CFG_BRAKE_DURATION_US     10  //10us
#define DEFAULT_CFG_DRIVE_MODE            0   //false: level   true: pwm

#define USE_HALL_VALUE_DURING_VIBRATION   0
#define PUSH_VIBRATING_HALL_VALUE_TO_GUI  0
#if PUSH_VIBRATING_HALL_VALUE_TO_GUI
extern uint8_t gbLeftVibratingFlag;
#endif


void Ele_Init(void);
void Ele_DisableVibrate(void);
void Ele_UpadteSegment(uint8_t virtualKey, uint8_t seg);
int32_t Ele_GetDebugParam(uint8_t virtualKey, CoilDebug_ts *const pxCoilDebugCfg);
void Ele_UpdateDebugParam(uint8_t virtualKey, const CoilDebug_ts *pxCoilDebugCfg);
void Ele_UpdateParam(uint8_t virtualKey, const CoilConfig_ts *pxCoilCfg);
void Ele_UpdateReleaseParam(uint8_t virtualKey, const CoilReleaseConfig_ts *pxCoilReleaseCfg);
void Ele_UpdateBrakeParam(uint8_t virtualKey, const CoilBrakeConfig_ts *pxCoilBrakeCfg);
void Ele_UpdateRelBrakeParam(uint8_t virtualKey, const CoilBrakeRelConfig_ts *pxCoilBrakeRelCfg);

//uint8_t Ele_IsReleaseVibrating(Ele_Position_te pos);
uint8_t Ele_IsVibrating(Ele_Position_te pos);
uint8_t Ele_IsTureVibrating(Ele_Position_te pos);
void Ele_PollingTask(void);
void Ele_LeftKeyPressProcess(void);
void Ele_LeftKeyReleaseProcess(void);
void Ele_RightKeyPressProcess(void);
void Ele_RightKeyReleaseProcess(void);
void Ele_K4KeyPressProcess(void);
void Ele_K5KeyPressProcess(void);


#endif /*__my_electromagnet_h__*/



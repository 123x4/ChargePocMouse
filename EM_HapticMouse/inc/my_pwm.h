/******************************************************************************
 * @file    my_pwm.h
 * @brief   PWM driver based on LPC11U35 CT32B1 timer
 * @author  chengjie.deng@gtech.com.cn
 * @date    created on 2026-07-08
 * @version 
 * @copyright Copyright (c) 2026 G.TECH TECHNOLOGY LTD.
 * @note    
 ******************************************************************************/
#ifndef __my_pwm_h__
#define __my_pwm_h__

#include "stdint.h"
#include "my_board.h"

#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))

#define MyPWM_Start(ch)       MyPWM_CT32B1_Start(ch)
#define MyPWM_Stop(ch)        MyPWM_CT32B1_Stop(ch)
//#define MyPWM_SetDuty(ch, p)  MyPWM_CT32B1_SetCompare((ch), Freq200kHzDuty[(p)])


extern const uint8_t Freq200kHzDuty[101];



int32_t MyPWM_Freq200kHzInit(void);
void MyPWM_Freq200kHzDeinit(void);
//void MyPWM_SetDuty(uint8_t channel, uint8_t percent);

int32_t MyPWM_CT32B1_Init(uint32_t periodCnt);
void MyPWM_CT32B1_GlobalStart(void);
void MyPWM_CT32B1_GlobalStop(void);
void MyPWM_CT32B1_SetCompare(uint8_t ch, uint32_t cmp);
void MyPWM_CT32B1_Start(uint8_t ch);
void MyPWM_CT32B1_Stop(uint8_t ch);


static inline void MyPWM_SetDuty(uint8_t channel, uint8_t percent)
{
  percent = percent > 100 ? 100 : percent;
  MyPWM_CT32B1_SetCompare(channel, Freq200kHzDuty[percent]);
}

#endif /* GT_LINA_T1A_H5_PWM */


#endif /* __my_pwm_h__ */






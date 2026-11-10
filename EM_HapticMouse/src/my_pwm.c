/*******************************************************************************
 * @file    my_pwm.c
 * @brief   PWM driver based on LPC11U35 CT32B1 timer
 * @author  chengjie.deng@gtech.com.cn
 * @date    created on 2026-07-08
 * @version 
 * @copyright Copyright (c) 2026 G.TECH TECHNOLOGY LTD.
 * @note    
*******************************************************************************/
#include "my_pwm.h"
#include "LPC11Uxx.h"
#include "my_board.h"
#include "stdint.h"
#include "OSC.h"

#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))

//
const uint8_t Freq200kHzDuty[101] = {
  241, //0
  239, 236, 234, 231, 229, 227, 224, 222, 219, 217, //1~10
  214, 212, 210, 207, 205, 202, 200, 198, 195, 193, 
  190, 188, 186, 183, 181, 178, 176, 174, 171, 169, 
  166, 164, 161, 159, 157, 154, 152, 149, 147, 145, 
  142, 140, 137, 135, 133, 130, 128, 125, 123, 121, 
  118, 116, 113, 111, 108, 106, 104, 101, 99,  96, 
  94,  92,  89,  87,  84,  82,  80,  77,  75,  72, 
  70,  67,  65,  63,  60,  58,  55,  53,  51,  48, 
  46,  43,  41,  39,  36,  34,  31,  29,  27,  24, 
  22,  19,  17,  14,  12,  10,  7,   5,   2,   0, 
};

int32_t MyPWM_Freq200kHzInit(void)
{
  if(0 == MyPWM_CT32B1_Init(240))
  {
    MyPWM_CT32B1_Stop(0); //channel 0
    MyPWM_CT32B1_Stop(1); //channel 1
    MyPWM_CT32B1_GlobalStart();
    return 0;
  }
  else
  {
    return -1;
  }
}

void MyPWM_Freq200kHzDeinit(void)
{
  MyPWM_CT32B1_Stop(0); //channel 0
  MyPWM_CT32B1_Stop(1); //channel 1
  MyPWM_CT32B1_GlobalStop();
}

//void MyPWM_SetDuty(uint8_t channel, uint8_t percent)
//{
//  if(percent > 100)
//  {
//    percent = 100;
//  }
//  MyPWM_CT32B1_SetCompare(channel, Freq200kHzDuty[percent]);
//}







/* hardware abstract layer */

int32_t MyPWM_CT32B1_Init(uint32_t periodCnt)
{
  /* initialzation in InitGPIO function. */
  /* selects pin function as CT32B1_MAT0 */
  LPC_IOCON->TDO_PIO0_13 &= ~0x07 ;
  LPC_IOCON->TDO_PIO0_13 |= 0x03 ;
  /* selects pin function as CT32B1_MAT1 */
  LPC_IOCON->TRST_PIO0_14 &= ~0x07 ;
  LPC_IOCON->TRST_PIO0_14 |= 0x03 ; 
  
  LPC_SYSCON->SYSAHBCLKCTRL |= (0x01 << 10); //enables clock for 32bit counter/timer1
  LPC_CT32B1->TCR = 0 ; //stop timer
  
  LPC_CT32B1->PC = 0;
  LPC_CT32B1->TC = 0;
  LPC_CT32B1->PR = 0; //TC will increase by 1 every 0.0208333us(48MHz = 48MHz/1).
  
  LPC_CT32B1->MR3 = periodCnt; // periodCnt = MCU_OSC / pwmFreqHz;
  LPC_CT32B1->MCR &= ~(1u << 9 | 1u << 10 | 1u << 11);
  LPC_CT32B1->MCR = (1u << 10); //the TC will be reset if MR3 matches it.
  
  //LPC_CT32B1->PWMC = 0;
  LPC_CT32B1->PWMC &= ~(1u << 0 | 1u << 1); //CT32Bn_MAT0 and CT32Bn_MAT1 is controlled by EM0.
  LPC_CT32B1->EMR &= ~(3u << 4 | 3u << 6);
  LPC_CT32B1->EMR |= (1u << 4 | 1u << 6); //[5:4]EMC0 [7:6]:EMC1 /* EMC0/1 = 0x1: Clear EMx on match (force LOW when exiting PWM) */

//  LPC_CT32B1->PWMC = (0x01 << 0)  /* PWM mode is enabled for CT32Bn_MAT0. */
//                   | (0x01 << 1); /* PWM mode is enabled for CT32Bn_MAT1. */
  
  LPC_CT32B1->MR0 = periodCnt + 1; //default duty is 0;
  LPC_CT32B1->MR1 = periodCnt + 1;
  
  return 0;
}

void MyPWM_CT32B1_GlobalStart(void)
{
  LPC_CT32B1->TCR |= (1u << 0); //enable counter
}

void MyPWM_CT32B1_GlobalStop(void)
{
  LPC_CT32B1->TCR &= ~(1u << 0) ; //disable counter
}

void MyPWM_CT32B1_SetCompare(uint8_t ch, uint32_t cmp)
{ /* 0~cmp: low level; cmp~MR3: high level; MR3: TC will be reset
   * duty: 0%:MRx > MR3;  100%: MRx = 0
   */
  if(ch == 0)
  {
    LPC_CT32B1->MR0 = cmp;
  }
  else if(ch == 1)
  {
    LPC_CT32B1->MR1 = cmp;
  }
}

void MyPWM_CT32B1_Start(uint8_t ch)
{
  if(ch == 0)
  {
    LPC_CT32B1->PWMC |= (1u << 0);
  }
  else if (ch == 1)
  {
    LPC_CT32B1->PWMC |= (1u << 1);
  }
}

void MyPWM_CT32B1_Stop(uint8_t ch)
{
  if(ch == 0)
  {
    LPC_CT32B1->PWMC &= ~(1u << 0); // Stop PWM: switch MATx to EMR control, EMCx=Clear ensures LOW
  }
  else if(ch == 1)
  {
    LPC_CT32B1->PWMC &= ~(1u << 1);
  }
}

#endif /* GT_LINA_T1A_H5_PWM */












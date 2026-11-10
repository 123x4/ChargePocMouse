/*******************************************************************************
 * @file    my_wareform.c
 * @brief   waveform driver based on LPC11U35 CT32B0 timer
 * @author  chengjie.deng@gtech.com.cn
 * @date    created on 2026-07-14
 * @version 
 * @copyright Copyright (c) 2026 G.TECH TECHNOLOGY LTD.
 * @note    
*******************************************************************************/

/* 1. include section *********************************************************/
#include "my_waveform.h"
#include "LPC11Uxx.h"
#include "my_pwm.h"
#include "my_debug.h"
#include "my_board.h"

#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))

/* 2. macro definition ********************************************************/
#define UPDATE_PERIOD_US          (20U)
#define MAX_WAVEFORM_PERIOD_US    (5000U)   //5ms
#define MIN_WAVEFORM_PERIOD_US    (UPDATE_PERIOD_US)
#define MAX_WAVEFORM_CYCLES_CNT   (10)
#define PHASE_SCALE               (15U)


/* 3. type definition *********************************************************/
typedef struct
{
  uint8_t id;
  WaveformState_te sta;
  WaveformInfo_ts info;
  uint32_t periodUs;
  uint32_t cycles;
  uint32_t curCnt;
  uint32_t inc;
  uint32_t acc;
  uint32_t updateInterval;
  uint32_t nextUpdateTick;
}WaveformGen_ts;


/* 4. variables ***************************************************************/
static WaveformGen_ts wfCh0 = {
  .id = 0,
  .sta = WF_STATE_UNINIT,
};
static WaveformGen_ts wfCh1 = {
  .id = 1,
  .sta = WF_STATE_UNINIT,
};

#if MYDEBUG_ENABLE
uint32_t timeStartTc = 0;
uint32_t costTime = 0;
#endif
/* 5. fucntions declaration ***************************************************/
static void MyWaveform_UpdateFromIsr(WaveformGen_ts *pGen);
static WaveformGen_ts* MyWaveform_GetHandle(WaveformChannel_te ch);
static void MyWaveform_Start(WaveformGen_ts *p);
static void MyWaveform_Stop(WaveformGen_ts *p);




/* 6. function definition *****************************************************/
int32_t MyWaveform_InitCT32B0(void)
{
  NVIC_DisableIRQ(TIMER_32_0_IRQn);
  LPC_CT32B0->TCR = 0 ;
  
  LPC_SYSCON->SYSAHBCLKCTRL |= (0x01 << 9);
  LPC_CT32B0->TC = 0;
  LPC_CT32B0->PC = 0;
  LPC_CT32B0->PR = 47; //TC will increase by 1 every 1us(1MHz = 48MHz/48).
//  LPC_CT32B0->MR0 = Time;
  LPC_CT32B0->MCR = 0;
  
  LPC_CT32B0->TCR = 1 ;
  NVIC_ClearPendingIRQ(TIMER_32_0_IRQn);
  NVIC_EnableIRQ(TIMER_32_0_IRQn) ;
  
  return 0;
}

void TIMER32_0_IRQHandler(void)
{
  #if MYDEBUG_ENABLE
  timeStartTc = LPC_CT32B0->TC;
  #endif
  if(LPC_CT32B0->IR & (0x01 << 0))   //MR0INT
  {
    // Writing a logic one to the corresponding IR bit will reset the interrupt. Writing a zero has no effect.
    LPC_CT32B0->IR = (0x01 << 0);    //clear MR0 interrupt
    MyWaveform_UpdateFromIsr(&wfCh0);
    // The next matching value
    LPC_CT32B0->MR0 = wfCh0.nextUpdateTick;
  }
  
  if(LPC_CT32B0->IR & (0x01 << 1))   //MR1INT
  {
    // Writing a logic one to the corresponding IR bit will reset the interrupt. Writing a zero has no effect.
    LPC_CT32B0->IR = (0x01 << 1);   //clear MR1 interrupt
    MyWaveform_UpdateFromIsr(&wfCh1);
    // The next matching value
    LPC_CT32B0->MR1 = wfCh1.nextUpdateTick;;
  }
  #if MYDEBUG_ENABLE
  costTime = LPC_CT32B0->TC - timeStartTc;
  #endif
#if 0
  if(LPC_CT32B0->IR & (0x01 << 2))   //MR2INT
  {
    // Writing a logic one to the corresponding IR bit will reset the interrupt. Writing a zero has no effect.
    LPC_CT32B0->IR = (0x01 << 2);   //clear MR2 interrupt
    
    LPC_CT32B0->MCR &= ~(0x01 << 6);   //disable MR2 interrupt
  }
  
  if(LPC_CT32B0->IR & (0x01 << 3))   //MR3INT
  {
    // Writing a logic one to the corresponding IR bit will reset the interrupt. Writing a zero has no effect.
    LPC_CT32B0->IR = (0x01 << 3);   //clear MR3 interrupt
    
    LPC_CT32B0->MCR &= ~(0x01 << 9);   //disable MR3 interrupt
  }
#endif
}





int32_t MyWaveform_Init(void)
{
  int32_t ret = 0;
  
  ret = MyPWM_Freq200kHzInit();
  ret = ret | MyWaveform_InitCT32B0();
  
  if(!ret)
  {
    wfCh0.sta = WF_STATE_STOPPED;
    wfCh1.sta = WF_STATE_STOPPED;
  }
  
  return ret;
}





int32_t MyWaveform_StartGenerate(WaveformChannel_te ch, WaveformInfo_ts *pInfo, uint32_t wfPeriodUs, uint8_t cycles)
{ 
  WaveformGen_ts *pGen = 0;
  uint32_t periodUs = 0;
  uint32_t updateTimes = 0;
  
  pGen = MyWaveform_GetHandle(ch);
  if(!pInfo || !cycles || !pGen || (pGen->sta == WF_STATE_UNINIT))
  {
    return -1;
  }
  if(!wfPeriodUs) // zero
  {
    return -2;
  }
  periodUs = wfPeriodUs;
  if(periodUs > MAX_WAVEFORM_PERIOD_US) //200Hz, period 5ms
  {
    periodUs = MAX_WAVEFORM_PERIOD_US;
  }
  else if(periodUs < MIN_WAVEFORM_PERIOD_US)
  {
    periodUs = MIN_WAVEFORM_PERIOD_US;
  }
  updateTimes = periodUs / UPDATE_PERIOD_US;
//  if(updateTimes == 0)
//  {
//    return -3;
//  }
  
  MyWaveform_Stop(pGen);
  pGen->sta = WF_STATE_STOPPED;
  
  //update waveform information
  pGen->info.pWfData = pInfo->pWfData;
  pGen->info.wfDataLen = pInfo->wfDataLen;
  pGen->periodUs = periodUs;
  
  pGen->cycles = cycles > MAX_WAVEFORM_CYCLES_CNT ? MAX_WAVEFORM_CYCLES_CNT : cycles;
  pGen->curCnt = 0;
  pGen->inc = ((uint32_t)(pGen->info.wfDataLen) << PHASE_SCALE) / updateTimes;
  pGen->acc = pGen->inc >> 1; //init_val
  pGen->updateInterval = UPDATE_PERIOD_US;
  
  pGen->sta = WF_STATE_RUNNING;
  MyWaveform_Start(pGen);
  return 0;
}

int32_t MyWaveform_StopGenerate(WaveformChannel_te ch)
{
  WaveformGen_ts *pGen = MyWaveform_GetHandle(ch);
  
  if(!pGen || (pGen->sta == WF_STATE_UNINIT))
  {
    return -1;
  }
  
  MyWaveform_Stop(pGen);
  pGen->sta = WF_STATE_STOPPED;
  
  return 0;
}

WaveformState_te MyWaveform_GetGenerateStatus(WaveformChannel_te ch)
{
  WaveformGen_ts *pGen = MyWaveform_GetHandle(ch);
  
  if(pGen)
  {
    return pGen->sta;
  }
  
  return WF_STATE_UNINIT;
}



static void MyWaveform_UpdateFromIsr(WaveformGen_ts *pGen)
{
  uint32_t idx = 0;
  
  pGen->acc += pGen->inc;
  idx = (pGen->acc >> PHASE_SCALE);
  if(idx < pGen->info.wfDataLen)
  {
    MyPWM_SetDuty(pGen->id, pGen->info.pWfData[idx]);
    pGen->nextUpdateTick += pGen->updateInterval;
  }
  else
  {
    pGen->curCnt++;
    if(pGen->curCnt >= pGen->cycles)
    {
      MyWaveform_Stop(pGen);
      pGen->sta = WF_STATE_STOPPED;
    }
    else
    {
      pGen->acc = pGen->inc >> 1; //init_val
      //pGen->sta = WF_STATE_RUNNING; //It is already in the "running" state.
      MyWaveform_Start(pGen);
    }
  }
}



static WaveformGen_ts* MyWaveform_GetHandle(WaveformChannel_te ch)
{
  WaveformGen_ts *ret = 0;
  switch(ch)
  {
    case CHANNEL_0:
      ret = &wfCh0;
      break;
    case CHANNEL_1:
      ret = &wfCh1;
      break;
  }
  return ret;
}

static void MyWaveform_Start(WaveformGen_ts *p)
{
  switch(p->id)
  {
    case 0:
      MyPWM_SetDuty(p->id, p->info.pWfData[p->acc >> PHASE_SCALE]);
      MyPWM_Start(p->id);
    
      __disable_irq();
      p->nextUpdateTick = LPC_CT32B0->TC + p->updateInterval;
      LPC_CT32B0->MR0 = p->nextUpdateTick;
      LPC_CT32B0->MCR |= (0x01 << 0);   //Enable MR0 Interrupt
      __enable_irq ();
      break;
    case 1:
      MyPWM_SetDuty(p->id, p->info.pWfData[p->acc >> PHASE_SCALE]);
      MyPWM_Start(p->id);
    
      __disable_irq();
      p->nextUpdateTick = LPC_CT32B0->TC + p->updateInterval;
      LPC_CT32B0->MR1 = p->nextUpdateTick;
      LPC_CT32B0->MCR |= (0x01 << 3);   //Enable MR1 Interrupt
      __enable_irq ();
      break;
  }
}

static void MyWaveform_Stop(WaveformGen_ts *p)
{
  switch(p->id)
  {
    case 0:
      MyPWM_Stop(p->id);
      LPC_CT32B0->MCR &= ~(0x01 << 0);    //disable MR0 interrupt
      break;
    case 1:
      MyPWM_Stop(p->id);
      LPC_CT32B0->MCR &= ~(0x01 << 3);    //disable MR1 interrupt
      break;
  }
}

#endif /* GT_LINA_T1A_H5_PWM */




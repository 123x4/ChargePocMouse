/*******************************************************************************
 * @file    my_adc.c
 * @brief
 * @author  chengjie
 * @date    created on 2026-08-24
 * @version V0.1
 * @note
*******************************************************************************/
#include "my_adc.h"
#include "GPIO.h"
#include "Timer.h"
#include "my_tick.h"
#include "my_debug.h"

#if (defined(IRPT_V4))

#define MyADC_DelayUs(x)  CT16B1_DelayUs(x)


//Only applicable to TDO_PIO0_13 and TRST_PIO0_14
#define MYADC_FUNC_MASK      (0x07U)
#define MYADC_FUNC_AS_GPIO   (0x01U)
#define MYADC_FUNC_AS_AD     (0x02U)
#define MYADC_SET_FUNC(reg, func) do{(reg) = ((reg) & (~MYADC_FUNC_MASK)) | (func); }while(0)


MyADC_ts leftAdcInst = {//ad2
  .ch = 2,
  .port = LB_ADC_PORT,
  .pin = LB_ADC_PIN,
  .pFuncReg = &(LPC_IOCON->LB_ADC),
};
MyADC_ts rightAdcInst = {//ad3
  .ch = 3,
  .port = RB_ADC_PORT,
  .pin = RB_ADC_PIN,
  .pFuncReg = &(LPC_IOCON->RB_ADC),
};
MyADC_ts *pLeftAdcHandle = &leftAdcInst;
MyADC_ts *pRightAdcHandle = &rightAdcInst;

#if MYDEBUG_ENABLE
static uint8_t cnt = 0;
#endif
static uint8_t hwInitFlag = 0;
static void MyADC_HwInit(void)
{
  if(!hwInitFlag)
  {
    hwInitFlag = 0xFF;
    LPC_SYSCON->PDRUNCFG &= ~(1U << 4); // Open adc power
    LPC_SYSCON->SYSAHBCLKCTRL |= (1u << 13); //enables clock for ADC
   
    LPC_ADC->CR &= ~(0xFFU << 8); 
    LPC_ADC->CR |= (15U << 8); // 48MHz / (15+1) = 3Mhz
    LPC_ADC->CR &= ~(0x7U << 17);  //11 clocks,  10bit accuracy
  }
}
__attribute__((unused)) static void MyADC_HwDeinit(void)
{
  LPC_ADC->CR &= ~(1u << 24); //stop conversion
  LPC_SYSCON->SYSAHBCLKCTRL &= ~(1u << 13); //disable clock for ADC
  LPC_SYSCON->PDRUNCFG |= (1U << 4); // close adc power
  hwInitFlag = 0;
}

int32_t MyADC_Init(MyADC_ts *p)
{
   MyADC_HwInit();
  //GPIO
  *(p->pFuncReg) &= (~((0x01 << 3) | (0x01 << 4))); // no pull-down, no pull-up
  DrvGPIOInput(p->port, p->pin); //input mode
  *(p->pFuncReg) &= ~(1U << 7);  //analog input mode
  MYADC_SET_FUNC(*(p->pFuncReg), MYADC_FUNC_AS_AD); //AD FUNC
  return 0;
}


int32_t MyADC_GetValue(MyADC_ts *p, uint16_t *pVal)
{
  uint32_t dummy;
  uint8_t ch = 0;
  #if MYDEBUG_ENABLE
  cnt = 0;
  #else
  uint8_t cnt = 0;
  #endif
  
  if(!p || p->ch > 7)
  {
    return -1;
  }

  ch = p->ch;
  LPC_ADC->CR &= ~(1u << 24); //stop conversion
  //LPC_ADC->CR &= ~(0xFFU);  //0~7bit clear
  LPC_ADC->CR |= (1u << ch);  //select channel
  dummy = LPC_ADC->DR[ch]; // clear DONE of bit31
  (void)dummy; //cancel warnning
  LPC_ADC->CR |= (1u << 24); //start conversion now
  
  while(1)
  {
    cnt += 1;
    MyADC_DelayUs(2);
    if((LPC_ADC->DR[ch]) & (1u << 31))
    {
      LPC_ADC->CR &= ~(1u << 24); //stop conversion
      break;
    }
    if(cnt >= 10)
    {
      return -2;
    }
  }
  *pVal = (LPC_ADC->DR[ch] >> 6) & 0x3FF; //[15:6] is data, 10BIT
  LPC_ADC->CR &= ~(1u << ch);  //cancel selected channel
  
  //If necessary, convert to voltage
  //float voltage = (float)(*pVal) * 3.3f / 1023.0f;
  
  return 0;
}



void MyADC_Poll_Test(void)
{
#if 0
#if MYDEBUG_ENABLE
  POLLING_INTERVAL_MS(250);
  
  static uint16_t leftVal = 0;
  static uint16_t rightVal = 0;
  static float leftVol = 0;
  static float rightVol = 0;
  
  if(MyADC_GetValue(pLeftAdcHandle, &leftVal))
  {
    InfoLog("leftADC: fail");
  }
  else
  {
    leftVol = (float)(leftVal) * 3.3f / 1023.0f;
    InfoLog("leftADC: %u, %fV, cnt: %u", leftVal, leftVol, cnt);
  }
  
  if(MyADC_GetValue(pRightAdcHandle, &rightVal))
  {
    InfoLog("rightADC: fail");
  }
  else
  {
    rightVol = (float)(rightVal) * 3.3f / 1023.0f;
    InfoLog("rihtADC: %u, %fV, cnt: %u", rightVal, rightVol, cnt);
  }
#endif
#endif
}


#endif






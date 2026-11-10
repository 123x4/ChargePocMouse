/*******************************************************************************
 * @file    my_electromagnet.c
 * @brief
 * @author  chengjie
 * @date    created on 2026-06-26
 * @version V0.1
 * @note
*******************************************************************************/
/* 1. include section *********************************************************/
#include "my_electromagnet.h"
#include "my_protocol_mouse.h"
#include "string.h"
#include "Timer.h"
#include "my_board.h"
#if (defined(GEN_2_H_V1))
#include "TI_DRV8833.h"
#endif

#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
#include "my_waveform.h"
#endif




/* 2. macro definition ********************************************************/
#define MAX_VIBRATE_DURATION_US         5000 //5ms
#define SINGLE_COIL_AT_A_TIME           1    //1: enable, 0: disable
#define VIBRATE_DEBOUNCE_TIME_MS        8    //ms
#define DAEMON_LIMIT_TURN_ON_TIME_MS    40   //ms. multiples of 8. if < 8, disable daemon.
#define MIN_AUTO_VIB_TRIG_PERIOD_MS     20   //20ms
#define DYN_ON_TIME_EN_DEFAULT          0    //0: default disable. 1: default enable
#define DIFF_PRESS_RELEASE_TURN_ON_TIME 1    //0: disbale, 1: enable

#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
#define BRAKE_FUNC_ENABLE               0
#else
#define BRAKE_FUNC_ENABLE               1
#endif
//debug define
#define DEBUG_STARTUP_FIX_VIB_PARAM_EN   0 //
#define DEBUG_STARTUP_FIX_VIB_PARAM_US   1000000 //us //nend close  DAEMON_LIMIT_TURN_ON_TIME_MS


#if (defined(HAPTIC_V2) || defined (HAPTIC_V3))
#define Ele_LeftOpen()     DrvGPIOOutput(PWM1_PORT, PWM1_PIN, 1)
#define Ele_LeftClose()    DrvGPIOOutput(PWM1_PORT, PWM1_PIN, 0)
#define Ele_RightOpen()    DrvGPIOOutput(PWM2_PORT, PWM2_PIN, 1)
#define Ele_RightClose()   DrvGPIOOutput(PWM2_PORT, PWM2_PIN, 0)
#define Ele_EnableBoost()  DrvGPIOOutput(ETA1626_EN_PORT, ETA1626_EN_PIN, 1)
#define Ele_DisableBoost() DrvGPIOOutput(ETA1626_EN_PORT, ETA1626_EN_PIN, 0)
#define Ele_Enable12V()   do{ DrvGPIOOutput(NMOS1_PORT, NMOS1_PIN, 0); \
                              DrvGPIOOutput(NMOS2_PORT, NMOS2_PIN, 0); }while(0)
#define Ele_Enable15V()   do{ DrvGPIOOutput(NMOS1_PORT, NMOS1_PIN, 1); \
                              DrvGPIOOutput(NMOS2_PORT, NMOS2_PIN, 0); }while(0)
#define Ele_Enable18V()   do{ DrvGPIOOutput(NMOS1_PORT, NMOS1_PIN, 0); \
                              DrvGPIOOutput(NMOS2_PORT, NMOS2_PIN, 1); }while(0)
#define Ele_EnableVoltageCfg0()   Ele_Enable12V();
#define Ele_EnableVoltageCfg1()   Ele_Enable15V();
#define Ele_EnableVoltageCfg2()   Ele_Enable18V();

#elif (defined(HAPTIC_V4))
#define Ele_LeftOpen()        DrvGPIOOutput(PWM1_PORT, PWM1_PIN, 1)
#define Ele_LeftClose()       DrvGPIOOutput(PWM1_PORT, PWM1_PIN, 0)
#define Ele_RightOpen()       DrvGPIOOutput(PWM2_PORT, PWM2_PIN, 1)
#define Ele_RightClose()      DrvGPIOOutput(PWM2_PORT, PWM2_PIN, 0)
#define Ele_EnableBoost()     DrvGPIOOutput(VDD_MOTOR_EN_PORT, VDD_MOTOR_EN_PIN, 1)
#define Ele_DisableBoost()    DrvGPIOOutput(VDD_MOTOR_EN_PORT, VDD_MOTOR_EN_PIN, 0)

#elif (defined(P723_POC))
#define Ele_LeftOpen()     DrvGPIOOutput(PWM1_PORT, PWM1_PIN, 1)
#define Ele_LeftClose()    DrvGPIOOutput(PWM1_PORT, PWM1_PIN, 0)
#define Ele_RightOpen()    DrvGPIOOutput(PWM2_PORT, PWM2_PIN, 1)
#define Ele_RightClose()   DrvGPIOOutput(PWM2_PORT, PWM2_PIN, 0)
#define Ele_EnableBoost()   DrvGPIOOutput(ETA1626_EN_PORT, ETA1626_EN_PIN, 1)
#define Ele_DisableBoost()  DrvGPIOOutput(ETA1626_EN_PORT, ETA1626_EN_PIN, 0)
#define Ele_Enable12V()   do{ DrvGPIOOutput(NMOS_D1_PORT, NMOS_D1_PIN, 0); \
                              DrvGPIOOutput(NMOS_D2_PORT, NMOS_D2_PIN, 0); }while(0)
#define Ele_Enable15V()   do{ DrvGPIOOutput(NMOS_D1_PORT, NMOS_D1_PIN, 1); \
                              DrvGPIOOutput(NMOS_D2_PORT, NMOS_D2_PIN, 0); }while(0)
#define Ele_Enable18V()   do{ DrvGPIOOutput(NMOS_D1_PORT, NMOS_D1_PIN, 0); \
                              DrvGPIOOutput(NMOS_D2_PORT, NMOS_D2_PIN, 1); }while(0)
#define Ele_EnableVoltageCfg0()   Ele_Enable12V();
#define Ele_EnableVoltageCfg1()   Ele_Enable15V();
#define Ele_EnableVoltageCfg2()   Ele_Enable18V();
                              
#elif (defined(GEN_2_H_V1))
#define Ele_LeftOpen()     TI_DERV8833_Forward(&motorLeft, BRIDGE_A)
#define Ele_LeftClose()    TI_DERV8833_Coast(&motorLeft, BRIDGE_A)
#define Ele_RightOpen()    TI_DERV8833_Forward(&motorRight, BRIDGE_A)
#define Ele_RightClose()   TI_DERV8833_Coast(&motorRight, BRIDGE_A)

#elif (defined(IRPT_V4) || defined(V4_HALL_HPTIC_V4) || defined(GT_LINA_T1A_H5))
#define Ele_LeftOpen()        DrvGPIOOutput(PWM1_PORT, PWM1_PIN, 1)
#define Ele_LeftClose()       DrvGPIOOutput(PWM1_PORT, PWM1_PIN, 0)
#define Ele_RightOpen()       DrvGPIOOutput(PWM2_PORT, PWM2_PIN, 1)
#define Ele_RightClose()      DrvGPIOOutput(PWM2_PORT, PWM2_PIN, 0)
#define Ele_EnableBoost()     DrvGPIOOutput(HP_EN_PORT, HP_EN_PIN, 1)
#define Ele_DisableBoost()    DrvGPIOOutput(HP_EN_PORT, HP_EN_PIN, 0)

#else
  #error "Hardware information is undefined."
#endif


/* 3. type definition *********************************************************/
typedef enum
{
  ELE_NO_VIBRATING = 0,
  ELE_PRESS_VIBRATING,
  ELE_RELEASE_VIBRATING,
  ELE_RELEASE_DEBOUNCE
}Ele_Status_te;

typedef struct
{
  Ele_Position_te pos;
  Ele_Status_te status;
  
  uint32_t vibrateBeginTick;
  
  uint32_t debounceBeginTick;
  uint32_t pressVibrateDurationUs;
  uint32_t releaseVibrateDurationUs;
  uint8_t pressDriveMode;
  uint8_t releaseDriveMode;
#if BRAKE_FUNC_ENABLE
  uint16_t brakeDelayUs;
  uint16_t brakeDurationUs;
  uint8_t brakeEnable;
  
  uint16_t relBrakeDelayUs;
  uint16_t relBrakeDurationUs;
  uint8_t relBrakeEnable;
#endif
  
  uint8_t mutex;
#if MYDEBUG_ENABLE
  uint8_t *pStr;
#endif
}Ele_Handle_ts;


/* 4. variables ***************************************************************/
static uint8_t updateFlagL = 0;
static uint8_t updateFlagR = 0;

Ele_Handle_ts leftEle = {
#if MYDEBUG_ENABLE
  .pStr = (uint8_t*)"left",
#endif
  .pos = ELE_POS_LEFT, 
  .status = ELE_NO_VIBRATING,
  .vibrateBeginTick = 0,
  .debounceBeginTick = 0,
  .pressVibrateDurationUs = 100,
  .releaseVibrateDurationUs = 100,
  .pressDriveMode = 0,
  .releaseDriveMode = 0,
#if BRAKE_FUNC_ENABLE
  .brakeDelayUs = 0,
  .brakeDurationUs = 0,
  .brakeEnable = 0,
  
  .relBrakeDelayUs = 0,
  .relBrakeDurationUs = 0,
  .relBrakeEnable = 0,
#endif
  .mutex = 0
};

Ele_Handle_ts rightEle = {
#if MYDEBUG_ENABLE
  .pStr = (uint8_t*)"right",
#endif
  .pos = ELE_POS_RIGHT,
  .status = ELE_NO_VIBRATING,
  .vibrateBeginTick = 0,
  .debounceBeginTick = 0,
  .pressVibrateDurationUs = 100,
  .releaseVibrateDurationUs = 100,
#if BRAKE_FUNC_ENABLE
  .brakeDelayUs = 0,
  .brakeDurationUs = 0,
  .brakeEnable = 0,
  
  .relBrakeDelayUs = 0,
  .relBrakeDurationUs = 0,
  .relBrakeEnable = 0,
#endif
  .mutex = 0
};

CoilDebug_ts xCoilDebugCfg[2] = {
  {
    .bDynOnTimeEn = DYN_ON_TIME_EN_DEFAULT,
    .bAutoVibrationEn = 0,
    .wAutoVibTrigPeriodMs = MIN_AUTO_VIB_TRIG_PERIOD_MS
  },
  {
    .bDynOnTimeEn = DYN_ON_TIME_EN_DEFAULT,
    .bAutoVibrationEn = 0,
    .wAutoVibTrigPeriodMs = MIN_AUTO_VIB_TRIG_PERIOD_MS
  }
};

static uint8_t nowHallSegment[ANA_TOTAL_COUNT] = {0};
//length = Hall MAX_SEGMENT + 1
//static uint16_t durationScale[MAX_SEGMENT + 1] = {0};
static uint16_t durationScale[MAX_SEGMENT + 1] = {
1024,	1003,	984,	966,	948,	930,	914,	898,	882,	867,	
853,	839,	825,	812,	800,	787,	775,	764,	752,	742,	
731,	721,	711,	701,	691,	682,	673,	664,	656,	648,	
640,	632,	624,	616,	609,	602,	595,	588,	581,	575,	
568,	562,	556,	550,	544,	538,	533,	527,	522,	517,	
512,	506,	501,	497,	492,	487,	483,	478,	474,	469,	
465,	461,	457,	453,	449,	445,	441,	437,	433,	430,	
426,	423,	419,	416,	412,	409,	406,	403,	400,	396,	
393,	390,	387,	384,	382,	379,	376,	373,	371,	368,	
365,	363,	360,	358,	355,	353,	350,	348,	345,	343,	
341,	
};

#if SINGLE_COIL_AT_A_TIME
typedef struct{
  uint32_t tick;
  uint8_t flag;
  Ele_Position_te pos;
}CacheVibrate_ts;
CacheVibrate_ts gxCacheVibrate = {0};
#endif

#if PUSH_VIBRATING_HALL_VALUE_TO_GUI
  uint8_t gbLeftVibratingFlag = 0;
#endif



#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
#if 0
static const uint8_t wfHighTbl[] = {100};
static WaveformInfo_ts wfHigh = {
  .pWfData = wfHighTbl,
  .wfDataLen = sizeof(wfHighTbl) / sizeof(wfHighTbl[0]),
};

static const uint8_t wfTrapzTbl[100] = {
      0,   4,   8,  12,  16,  20,  24,  28,  32,  36,
     40,  44,  48,  52,  56,  60,  64,  68,  72,  76,
     80,  84,  88,  92,  96, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100,  96,  92,  88,  84,  80,
     76,  72,  68,  64,  60,  56,  52,  48,  44,  40,
     36,  32,  28,  24,  20,  16,  12,   8,   4,   0,
};
static WaveformInfo_ts wfTrapz = {
  .pWfData = wfTrapzTbl,
  .wfDataLen = sizeof(wfTrapzTbl) / sizeof(wfTrapzTbl[0]),
};

static const uint8_t wfHalfSineTbl[100] = {
      0,  3,  6,  9, 12, 15, 18, 22, 25, 28,
     31, 34, 37, 40, 42, 45, 48, 51, 54, 56,
     59, 61, 64, 66, 69, 71, 73, 75, 77, 79,
     81, 83, 84, 86, 88, 89, 90, 92, 93, 94,
     95, 96, 97, 97, 98, 98, 99, 99, 99, 99,
     99, 99, 99, 99, 98, 98, 97, 97, 96, 95,
     94, 93, 92, 90, 89, 88, 86, 84, 83, 81,
     79, 77, 75, 73, 71, 69, 66, 64, 61, 59,
     56, 54, 51, 48, 45, 42, 40, 37, 34, 31,
     28, 25, 22, 18, 15, 12,  9,  6,  3,  0,
};
static WaveformInfo_ts wfHalfSine = {
  .pWfData = wfHalfSineTbl,
  .wfDataLen = sizeof(wfHalfSineTbl) / sizeof(wfHalfSineTbl[0]),
};

static const uint8_t wfTriTbl[100] = {
      0,  2,  4,  6,  8, 10, 12, 14, 16, 18,
     20, 22, 24, 26, 28, 30, 32, 34, 36, 38,
     40, 42, 44, 46, 48, 50, 52, 54, 56, 58,
     60, 62, 64, 66, 68, 70, 72, 74, 76, 78,
     80, 82, 84, 86, 88, 90, 92, 94, 96, 98,
    100, 98, 96, 94, 92, 90, 88, 86, 84, 82,
     80, 78, 76, 74, 72, 70, 68, 66, 64, 62,
     60, 58, 56, 54, 52, 50, 48, 46, 44, 42,
     40, 38, 36, 34, 32, 30, 28, 26, 24, 22,
     20, 18, 16, 14, 12, 10,  8,  6,  4,  2,
};
static WaveformInfo_ts wfTri = {
  .pWfData = wfTriTbl,
  .wfDataLen = sizeof(wfTriTbl) / sizeof(wfTriTbl[0]),
};
/*
static const uint8_t wfCustomTbl[100] = {
     52, 53, 54, 55, 56, 57, 58, 59, 60, 61, //0
     62, 63, 64, 65, 66, 67, 68, 69, 70, 71, //1
     72, 73, 74, 75, 76, 77, 78, 79, 80, 81, //2
     82, 83, 84, 85, 86, 87, 88, 89, 90, 91, //3
     92, 93, 94, 95, 96, 97, 98, 99,100,100, //4
    100,100, 99, 98, 97, 96, 95, 94, 93, 92, //5
     91, 90, 89, 88, 87, 86, 85, 84, 83, 82, //6
     81, 80, 79, 78, 77, 76, 75, 74, 73, 72, //7
     71, 70, 69, 68, 67, 66, 65, 64, 63, 62, //8
     61, 60, 59, 58, 57, 56, 55, 54, 53, 52, //9
};

*/
/*
static const uint8_t wfCustomTbl[100] = {
     60, 61, 62, 62, 63, 64, 65, 66, 67, 67, //0
     68, 69, 70, 71, 72, 72, 73, 74, 75, 76, //1
     77, 77, 78, 79, 80, 81, 82, 82, 83, 84, //2
     85, 86, 87, 87, 88, 89, 90, 91, 92, 92, //3
     93, 94, 95, 96, 97, 97, 98, 99,100, 100,//4
    100,100, 99, 98, 97, 97, 96, 95, 94, 93, //5
     92, 92, 91, 90, 89, 88, 87, 87, 86, 85, //6
     84, 83, 82, 82, 81, 80, 79, 78, 77, 77, //7
     76, 75, 74, 73, 72, 72, 71, 70, 69, 68, //8
     67, 67, 66, 65, 64, 63, 62, 62, 61, 60, //9
};
*/
static const uint8_t wfCustomTbl[100] = {
     43, 44, 46, 47, 49, 50, 52, 53, 54, 56, //0
     57, 59, 60, 61, 63, 64, 66, 67, 69, 70, //1
     71, 73, 74, 76, 77, 79, 80, 81, 83, 84, //2
     86, 87, 88, 90, 91, 93, 94, 96, 97, 98, //3
    100,100,100,100,100,100,100,100,100,100, //4
    100,100,100,100,100,100,100,100,100,100, //5
     98, 97, 96, 94, 93, 91, 90, 88, 87, 86, //6
     84, 83, 81, 80, 79, 77, 76, 74, 73, 71, //7
     70, 69, 67, 66, 64, 63, 61, 60, 59, 57, //8
     56, 54, 53, 52, 50, 49, 47, 46, 44, 43, //9
};
static WaveformInfo_ts wfCustom = {
  .pWfData = wfCustomTbl,
  .wfDataLen = sizeof(wfCustomTbl) / sizeof(wfCustomTbl[0]),
};
#endif
static const uint8_t wfHighTbl[] = {100};
static WaveformInfo_ts wfHigh = {
  .pWfData = wfHighTbl,
  .wfDataLen = sizeof(wfHighTbl) / sizeof(wfHighTbl[0]),
};
static WaveformInfo_ts wfCustom = {0};

static WaveformInfo_ts *pLeftWf = &wfHigh;
static WaveformInfo_ts *pRightWf = &wfHigh;


#endif



#if (defined(GEN_2_H_V1))
TI_DRV8833_ts motorLeft = {
  .io.nSleepPort = NSLEEP_PORT,
  .io.nSleepPin = NSLEEP_PIN,
  .io.AIN1Port = PWM1_PORT,
  .io.AIN1Pin = PWM1_PIN,
  .io.AIN2Port = PWM2_PORT,
  .io.AIN2Pin = PWM2_PIN,
  .io.BIN1Port = PWM1_PORT,
  .io.BIN1Pin = PWM1_PIN,
  .io.BIN2Port = PWM2_PORT,
  .io.BIN2Pin = PWM2_PIN,
};

TI_DRV8833_ts motorRight = {
  .io.nSleepPort = NSLEEP_PORT,
  .io.nSleepPin = NSLEEP_PIN,
  .io.AIN1Port = PWM3_PORT,
  .io.AIN1Pin = PWM3_PIN,
  .io.AIN2Port = PWM4_PORT,
  .io.AIN2Pin = PWM4_PIN,
  .io.BIN1Port = PWM3_PORT,
  .io.BIN1Pin = PWM3_PIN,
  .io.BIN2Port = PWM4_PORT,
  .io.BIN2Pin = PWM4_PIN,
};
#endif

/* 5. fucntions declaration ***************************************************/
#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
#else
static void Ele_InitCT32B0(void);
#endif
//static void Ele_InitDurationScaleArray(void);
static uint32_t Ele_CalculateDuration(Ele_Position_te pos, uint8_t segment, uint32_t settingDuration);
static void Ele_StatusTask(Ele_Handle_ts *pEle);
#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
#else
static void Ele_Daemon(void);
#endif
static void Ele_OnlyOne(void);
static void Ele_LeftAutoTrigger(void);
static void Ele_RightAutoTrigger(void);
static void Ele_DebugTask(void);


#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
#else
/* 6. function definition *****************************************************/
void TIMER32_0_IRQHandler(void)
{
  if(LPC_CT32B0->IR & (0x01 << 0))   //MR0INT
  {
    // Writing a logic one to the corresponding IR bit will reset the interrupt. Writing a zero has no effect.
    LPC_CT32B0->IR = (0x01 << 0);    //clear MR0 interrupt
    
    LPC_CT32B0->MCR &= ~(0x01 << 0); //disable MR0 interrupt
    Ele_LeftClose();
    //if(leftEle.status == ELE_RELEASE_VIBRATING || leftEle.status == ELE_PRESS_VIBRATING)
    {
      leftEle.status = ELE_RELEASE_DEBOUNCE;
      leftEle.debounceBeginTick = MyTick_GetTickMs();
    }
  }
  
  if(LPC_CT32B0->IR & (0x01 << 1))   //MR1INT
  {
    // Writing a logic one to the corresponding IR bit will reset the interrupt. Writing a zero has no effect.
    LPC_CT32B0->IR = (0x01 << 1);   //clear MR0 interrupt
    
    LPC_CT32B0->MCR &= ~(0x01 << 3);   //disable MR1 interrupt
    Ele_RightClose();
    //if(rightEle.status == ELE_RELEASE_VIBRATING || rightEle.status == ELE_PRESS_VIBRATING)
    {
      rightEle.status = ELE_RELEASE_DEBOUNCE;
      rightEle.debounceBeginTick = MyTick_GetTickMs();
    }
  }
}
#endif

void Ele_Init(void)
{
#if (defined(HAPTIC_V2) || defined (HAPTIC_V3) || defined(P723_POC))
  Ele_DisableBoost();      //ETA1626_EN disable
  Ele_LeftClose();
  Ele_RightClose();
  Ele_EnableVoltageCfg0();
//  Ele_EnableBoost();       //ETA1626_EN enable
#elif (defined(IRPT_V4) || defined(V4_HALL_HPTIC_V4))
  Ele_DisableBoost();      //ETA1628_EN disable
  Ele_LeftClose();
  Ele_RightClose();
  Ele_EnableBoost();       //ETA1628_EN enable
  
#elif (defined(GEN_2_H_V1))
  TI_DRV8833_Init(&motorLeft);
  TI_DRV8833_Init(&motorRight);
  TI_DRV8833_ExitSleepMode(&motorLeft);
  TI_DRV8833_ExitSleepMode(&motorRight);
  Ele_LeftClose();
  Ele_RightClose();
  
#elif (defined(GT_LINA_T1A_H5))

#ifdef GT_LINA_T1A_H5_PWM
  Ele_DisableBoost();      //ETA1628_EN disable
  MyWaveform_Init();
  Ele_EnableBoost();       //ETA1628_EN enable
#else
  Ele_DisableBoost();      //ETA1628_EN disable
  Ele_LeftClose();
  Ele_RightClose();
  Ele_EnableBoost();       //ETA1628_EN enable
#endif

#elif (defined(HAPTIC_V4))
#if (defined(HAPTIC_V4_PWM))
  Ele_DisableBoost();      //ETA1628_EN disable
  MyWaveform_Init();
  Ele_EnableBoost();       //ETA1628_EN enable
#else
  Ele_DisableBoost();      //ETA1628_EN disable
  Ele_LeftClose();
  Ele_RightClose();
  Ele_EnableBoost();       //ETA1628_EN enable
#endif
  
#else
  #error "Hardware information is undefined."
#endif


#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))

#else
  Ele_InitCT32B0();
#endif

}

void Ele_DisableVibrate(void)
{
#if (defined(HAPTIC_V2) || defined (HAPTIC_V3) || defined(P723_POC) || defined (IRPT_V4) || defined(V4_HALL_HPTIC_V4))
  Ele_LeftClose();
  Ele_RightClose();
  Ele_DisableBoost(); //ETA1626_EN disable
#elif (defined(GEN_2_H_V1))
  Ele_LeftClose();
  Ele_RightClose();
  TI_DRV8833_EnterSleepMode(&motorLeft);
  TI_DRV8833_EnterSleepMode(&motorRight);
  TI_DRV8833_Deinit(&motorLeft);
  TI_DRV8833_Deinit(&motorRight);
#elif (defined(GT_LINA_T1A_H5))

#ifdef GT_LINA_T1A_H5_PWM
  MyWaveform_StopGenerate(CHANNEL_0);
  MyWaveform_StopGenerate(CHANNEL_1);
  Ele_DisableBoost(); //ETA1626_EN disable
#else
  Ele_LeftClose();
  Ele_RightClose();
  Ele_DisableBoost(); //ETA1626_EN disable
#endif

#elif (defined(HAPTIC_V4))
#ifdef HAPTIC_V4_PWM
  MyWaveform_StopGenerate(CHANNEL_0);
  MyWaveform_StopGenerate(CHANNEL_1);
  Ele_DisableBoost(); //ETA1626_EN disable
#else
  Ele_LeftClose();
  Ele_RightClose();
  Ele_DisableBoost(); //ETA1626_EN disable
#endif

#else
  #error "Hardware information is undefined."
#endif
//  MyTick_DelayMs(5);
}

void Ele_UpadteSegment(uint8_t virtualKey, uint8_t seg)
{
  uint8_t index = 0xFF;
  
  switch(virtualKey)
  {
    case VK_LBUTTON:
      {
        index = 0;
      }
      break;
    case VK_RBUTTON:
      {
        index = 1;
      }
      break;
    default:
      return; //exit
  }
  
  nowHallSegment[index] = seg;
}

int32_t Ele_GetDebugParam(uint8_t virtualKey, CoilDebug_ts *const pxCoilDebugCfg)
{
  uint8_t index = 0xFF;
  
  if(0 == pxCoilDebugCfg)
  {
    return -1;
  }
  
  switch(virtualKey)
  {
    case VK_LBUTTON:
      {
        index = 0;
      }
      break;
    case VK_RBUTTON:
      {
        index = 1;
      }
      break;
    default:
      return -2; //exit
  }
  
  memcpy(pxCoilDebugCfg, &(xCoilDebugCfg[index]), sizeof(CoilDebug_ts));
  
  return 0;
}

void Ele_UpdateDebugParam(uint8_t virtualKey, const CoilDebug_ts *pxCoilDebugCfg)
{
  uint8_t index = 0xFF;
  
  switch(virtualKey)
  {
    case VK_LBUTTON:
      {
        index = 0;
      }
      break;
    case VK_RBUTTON:
      {
        index = 1;
      }
      break;
    default:
      return; //exit
  }
  
  xCoilDebugCfg[index].bDynOnTimeEn = pxCoilDebugCfg->bDynOnTimeEn;
  xCoilDebugCfg[index].bAutoVibrationEn = pxCoilDebugCfg->bAutoVibrationEn;
  if(pxCoilDebugCfg->wAutoVibTrigPeriodMs > MIN_AUTO_VIB_TRIG_PERIOD_MS)
  {
    xCoilDebugCfg[index].wAutoVibTrigPeriodMs = pxCoilDebugCfg->wAutoVibTrigPeriodMs;
  }
  else
  {
    xCoilDebugCfg[index].wAutoVibTrigPeriodMs = MIN_AUTO_VIB_TRIG_PERIOD_MS;
  }
}

void Ele_UpdateParam(uint8_t virtualKey, const CoilConfig_ts *pxCoilCfg)
{
  if(virtualKey == VK_LBUTTON)
  {
    updateFlagL = 1;
    if((pxCoilCfg->bmEnableMask) & (0x01 << 7))
    {
      if(pxCoilCfg->wCoilOnTimeUs > MAX_VIBRATE_DURATION_US)
      {
        leftEle.pressVibrateDurationUs = MAX_VIBRATE_DURATION_US;
        #if !DIFF_PRESS_RELEASE_TURN_ON_TIME
        leftEle.releaseVibrateDurationUs = MAX_VIBRATE_DURATION_US;
        #endif
        
      }
      else
      {
        leftEle.pressVibrateDurationUs = pxCoilCfg->wCoilOnTimeUs;
        #if !DIFF_PRESS_RELEASE_TURN_ON_TIME
        leftEle.releaseVibrateDurationUs = pxCoilCfg->wCoilOnTimeUs;
        #endif
      }
    }
    else
    {
      leftEle.pressVibrateDurationUs = 0;
      #if !DIFF_PRESS_RELEASE_TURN_ON_TIME
      leftEle.releaseVibrateDurationUs = 0;
      #endif
    }
    
    switch((pxCoilCfg->bmEnableMask) & (0x3))
    {
      case 0:
        #if (defined(HAPTIC_V2) || defined (HAPTIC_V3) || defined(P723_POC))
        Ele_DisableBoost();
        Ele_EnableVoltageCfg0();
        Ele_EnableBoost();
        #endif
        break;
      case 1:
        #if (defined(HAPTIC_V2) || defined (HAPTIC_V3) || defined(P723_POC))
        Ele_DisableBoost();
        Ele_EnableVoltageCfg1();
        Ele_EnableBoost();
        #endif
        break;
      case 2:
        #if (defined(HAPTIC_V2) || defined (HAPTIC_V3) || defined(P723_POC))
        Ele_DisableBoost();
        Ele_EnableVoltageCfg2();
        Ele_EnableBoost();
        #endif
        break;
      default :
        ;
        break;
    }
    
  }
  else if(virtualKey == VK_RBUTTON)
  {
    updateFlagR = 1;
    if((pxCoilCfg->bmEnableMask) & (0x01 << 7))
    {
      if(pxCoilCfg->wCoilOnTimeUs > MAX_VIBRATE_DURATION_US)
      {
        rightEle.pressVibrateDurationUs = MAX_VIBRATE_DURATION_US;
        #if !DIFF_PRESS_RELEASE_TURN_ON_TIME
        rightEle.releaseVibrateDurationUs = MAX_VIBRATE_DURATION_US;
        #endif
      }
      else
      {
        rightEle.pressVibrateDurationUs = pxCoilCfg->wCoilOnTimeUs;
        #if !DIFF_PRESS_RELEASE_TURN_ON_TIME
        rightEle.releaseVibrateDurationUs = pxCoilCfg->wCoilOnTimeUs;
        #endif
      }
    }
    else
    {
      rightEle.pressVibrateDurationUs = 0;
      #if !DIFF_PRESS_RELEASE_TURN_ON_TIME
      rightEle.releaseVibrateDurationUs = 0;
      #endif
    }
    
    switch((pxCoilCfg->bmEnableMask) & (0x3))
    {
      case 0:
        #if (defined(HAPTIC_V2) || defined (HAPTIC_V3) || defined(P723_POC))
        Ele_DisableBoost();
        Ele_EnableVoltageCfg0();
        Ele_EnableBoost();
        #endif
        break;
      case 1:
        #if (defined(HAPTIC_V2) || defined (HAPTIC_V3) || defined(P723_POC))
        Ele_DisableBoost();
        Ele_EnableVoltageCfg1();
        Ele_EnableBoost();
        #endif
        break;
      case 2:
        #if (defined(HAPTIC_V2) || defined (HAPTIC_V3) || defined(P723_POC))
        Ele_DisableBoost();
        Ele_EnableVoltageCfg2();
        Ele_EnableBoost();
        #endif
        break;
      default :
        ;
        break;
    }
  }
}

void Ele_UpdateReleaseParam(uint8_t virtualKey, const CoilReleaseConfig_ts *pxCoilReleaseCfg)
{
#if DIFF_PRESS_RELEASE_TURN_ON_TIME
  if(virtualKey == VK_LBUTTON)
  {
    updateFlagL = 1;
    if((pxCoilReleaseCfg->bmEnableMask) & (0x01 << 7))
    {
      if(pxCoilReleaseCfg->wCoilOnTimeUs > MAX_VIBRATE_DURATION_US)
      {
        leftEle.releaseVibrateDurationUs = MAX_VIBRATE_DURATION_US;
        
      }
      else
      {
        leftEle.releaseVibrateDurationUs = pxCoilReleaseCfg->wCoilOnTimeUs;
      }
    }
    else
    {
      leftEle.releaseVibrateDurationUs = 0;
    }
    
  }
  else if(virtualKey == VK_RBUTTON)
  {
    updateFlagR = 1;
    if((pxCoilReleaseCfg->bmEnableMask) & (0x01 << 7))
    {
      if(pxCoilReleaseCfg->wCoilOnTimeUs > MAX_VIBRATE_DURATION_US)
      {
        rightEle.releaseVibrateDurationUs = MAX_VIBRATE_DURATION_US;
      }
      else
      {
        rightEle.releaseVibrateDurationUs = pxCoilReleaseCfg->wCoilOnTimeUs;
      }
    }
    else
    {
      rightEle.releaseVibrateDurationUs = 0;
    }
  }
#endif
}

void Ele_UpdateBrakeParam(uint8_t virtualKey, const CoilBrakeConfig_ts *pxCoilBrakeCfg)
{
#if BRAKE_FUNC_ENABLE
  Ele_Handle_ts *p = 0;
  
  switch(virtualKey)
  {
    case VK_LBUTTON:
    {
      p = &leftEle;
    }
    break;
    case VK_RBUTTON:
    {
      p = &rightEle;
    }
    break;
    default:
      return;
  }
  if((pxCoilBrakeCfg->bmEnableMask) & (0x01 << 7))
  {
    p->brakeEnable = 0xFF; //true
  }
  else
  {
    p->brakeEnable = 0; //false
  }
  p->brakeDelayUs = pxCoilBrakeCfg->wBrakeDlyUs;
  p->brakeDurationUs = pxCoilBrakeCfg->wBrakeDurUs > MAX_VIBRATE_DURATION_US ? 
  MAX_VIBRATE_DURATION_US : pxCoilBrakeCfg->wBrakeDurUs;
#endif
}

void Ele_UpdateRelBrakeParam(uint8_t virtualKey, const CoilBrakeRelConfig_ts *pxCoilBrakeRelCfg)
{
#if BRAKE_FUNC_ENABLE
  Ele_Handle_ts *p = 0;
  
  switch(virtualKey)
  {
    case VK_LBUTTON:
    {
      p = &leftEle;
    }
    break;
    case VK_RBUTTON:
    {
      p = &rightEle;
    }
    break;
    default:
      return;
  }
  if((pxCoilBrakeRelCfg->bmEnableMask) & (0x01 << 7))
  {
    p->relBrakeEnable = 0xFF; //true
  }
  else
  {
    p->relBrakeEnable = 0; //false
  }
  p->relBrakeDelayUs = pxCoilBrakeRelCfg->wBrakeDlyUs;
  p->relBrakeDurationUs = pxCoilBrakeRelCfg->wBrakeDurUs > MAX_VIBRATE_DURATION_US ? 
  MAX_VIBRATE_DURATION_US : pxCoilBrakeRelCfg->wBrakeDurUs;
#endif
}

/*
void Ele_UpdateCoilDriveMode(uint8_t virtualKey, const CoilDriveModeConfig_ts *pxCoilDriveModeCfg)
{
  Ele_Handle_ts *p = 0;
  
  switch(virtualKey)
  {
    case VK_LBUTTON:
    {
      p = &leftEle;
    }
    break;
    case VK_RBUTTON:
    {
      p = &rightEle;
    }
    break;
    default:
      return;
  }
}
*/

#if 0
uint8_t Ele_IsReleaseVibrating(Ele_Position_te pos)
{
  uint8_t ret = 0;
  switch(pos)
  {
    case ELE_POS_LEFT:
      if(ELE_RELEASE_VIBRATING == leftEle.status || \
         ELE_RELEASE_DEBOUNCE == leftEle.status    )
      {
        ret = 1;
      }
      else
      {
        ret = 0;
      }
      break;
    case ELE_POS_RIGHT:
      if(ELE_RELEASE_VIBRATING == rightEle.status || \
         ELE_RELEASE_DEBOUNCE == rightEle.status     )
      {
        ret = 1;
      }
      else
      {
        ret = 0;
      }
      break;
    default:
      break;
  }
  
  return ret;
}
#endif

uint8_t Ele_IsVibrating(Ele_Position_te pos)
{
  uint8_t ret = 0;
  switch(pos)
  {
    case ELE_POS_LEFT:
      if(ELE_NO_VIBRATING != leftEle.status)
      {
        ret = 1;
      }
      else
      {
        ret = 0;
      }
      break;
    case ELE_POS_RIGHT:
      if(ELE_NO_VIBRATING != rightEle.status)
      {
        ret = 1;
      }
      else
      {
        ret = 0;
      }
      break;
    default:
      break;
  }
  
  return ret;
}

uint8_t Ele_IsTureVibrating(Ele_Position_te pos)
{
  uint8_t ret = 0;
  switch(pos)
  {
    case ELE_POS_LEFT:

      if(ELE_PRESS_VIBRATING == leftEle.status || ELE_RELEASE_VIBRATING == leftEle.status)
      {
        ret = 1;
      }
      else
      {
        ret = 0;
      }
      break;
    case ELE_POS_RIGHT:
      if(ELE_PRESS_VIBRATING == rightEle.status || ELE_RELEASE_VIBRATING == rightEle.status)
      {
        ret = 1;
      }
      else
      {
        ret = 0;
      }
      break;
    default:
      break;
  }
  
  return ret;
}


void Ele_PollingTask(void)
{
  Ele_StatusTask(&leftEle);
  Ele_StatusTask(&rightEle);
  
#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
#else
  Ele_Daemon();
#endif
  
  #if SINGLE_COIL_AT_A_TIME
  Ele_OnlyOne();
  #endif
  
  Ele_LeftAutoTrigger();
  Ele_RightAutoTrigger();
  
  Ele_DebugTask();

}


void Ele_LeftKeyPressProcess(void)
{
#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
  uint32_t wfPeriodUs = 0;
#endif
  if(leftEle.pressVibrateDurationUs)
  {
    #if SINGLE_COIL_AT_A_TIME
    if(ELE_PRESS_VIBRATING == rightEle.status || ELE_RELEASE_VIBRATING == rightEle.status)
    {
      gxCacheVibrate.flag = 1;
      gxCacheVibrate.pos = ELE_POS_LEFT;
      gxCacheVibrate.tick = MyTick_GetTickMs();
      return;
    }
    #endif
#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
    wfPeriodUs = Ele_CalculateDuration(ELE_POS_LEFT, nowHallSegment[0], leftEle.pressVibrateDurationUs);
    leftEle.status = ELE_PRESS_VIBRATING;
    leftEle.vibrateBeginTick = MyTick_GetTickMs();
    if(Config_GetCoilDriveMode())
    {
      wfCustom.pWfData = Config_GetPwmTable();
      wfCustom.wfDataLen = 100;
      pLeftWf = &wfCustom;
    }
    else
    {
      pLeftWf = &wfHigh;
    }
    MyWaveform_StartGenerate(CHANNEL_0, pLeftWf, wfPeriodUs, 1);

#else
    #if BRAKE_FUNC_ENABLE
    leftEle.status = ELE_PRESS_VIBRATING;
    Ele_LeftOpen(); //consider 1us ontime
    CT16B1_DelayUs((uint16_t)Ele_CalculateDuration(ELE_POS_LEFT, nowHallSegment[0], leftEle.pressVibrateDurationUs));
    Ele_LeftClose();
    if(leftEle.brakeEnable)
    {
      CT16B1_DelayUs(leftEle.brakeDelayUs);
      Ele_LeftOpen();
      CT16B1_DelayUs(leftEle.brakeDurationUs);
      Ele_LeftClose();
    }
    leftEle.status = ELE_RELEASE_DEBOUNCE;
    leftEle.debounceBeginTick = MyTick_GetTickMs();
    #else
    LPC_CT32B0->MCR &= ~(0x01 << 0); //disable MRO interrupt
    leftEle.status = ELE_PRESS_VIBRATING;
    Ele_LeftOpen(); //consider 1us ontime
    __disable_irq();
    /* Ele_CalculateDuration() takes about 3us.*/
    LPC_CT32B0->MR0 = LPC_CT32B0->TC + 1 + Ele_CalculateDuration(ELE_POS_LEFT, nowHallSegment[0], leftEle.pressVibrateDurationUs);
    LPC_CT32B0->MCR |= (0x01 << 0);   //Enable MR0 Interrupt
    __enable_irq ();
    #endif
#endif
  }
  InfoLog("%u", MyTick_GetTickMs());
}
void Ele_LeftKeyReleaseProcess(void)
{
#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
  uint32_t wfPeriodUs = 0;
#endif
  if(leftEle.releaseVibrateDurationUs)
  {
    #if SINGLE_COIL_AT_A_TIME
    if(ELE_PRESS_VIBRATING == rightEle.status || ELE_RELEASE_VIBRATING == rightEle.status)
    {
      gxCacheVibrate.flag = 1;
      gxCacheVibrate.pos = ELE_POS_LEFT;
      gxCacheVibrate.tick = MyTick_GetTickMs();
      return;
    }
    #endif
     
#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
    wfPeriodUs = Ele_CalculateDuration(ELE_POS_LEFT, nowHallSegment[0], leftEle.releaseVibrateDurationUs);
    leftEle.status = ELE_RELEASE_VIBRATING;
    leftEle.vibrateBeginTick = MyTick_GetTickMs();
    if(Config_GetCoilDriveMode())
    {
      wfCustom.pWfData = Config_GetPwmTable();
      wfCustom.wfDataLen = 100;
      pLeftWf = &wfCustom;
    }
    else
    {
      pLeftWf = &wfHigh;
    }
    MyWaveform_StartGenerate(CHANNEL_0, pLeftWf, wfPeriodUs, 1);
#else
    #if BRAKE_FUNC_ENABLE
    leftEle.status = ELE_PRESS_VIBRATING;
    Ele_LeftOpen(); //consider 1us ontime
    CT16B1_DelayUs((uint16_t)Ele_CalculateDuration(ELE_POS_LEFT, nowHallSegment[0], leftEle.releaseVibrateDurationUs));
    Ele_LeftClose();
    if(leftEle.relBrakeEnable)
    {
      CT16B1_DelayUs(leftEle.relBrakeDelayUs);
      Ele_LeftOpen();
      CT16B1_DelayUs(leftEle.relBrakeDurationUs);
      Ele_LeftClose();
    }
    leftEle.status = ELE_RELEASE_DEBOUNCE;
    leftEle.debounceBeginTick = MyTick_GetTickMs();
    #else
    LPC_CT32B0->MCR &= ~(0x01 << 0); //disable MRO interrupt
    leftEle.status = ELE_RELEASE_VIBRATING;
    Ele_LeftOpen();
    __disable_irq();
    LPC_CT32B0->MR0 = LPC_CT32B0->TC + 1 + Ele_CalculateDuration(ELE_POS_LEFT, nowHallSegment[0], leftEle.releaseVibrateDurationUs);
    LPC_CT32B0->MCR |= (0x01 << 0);   //Enable MR0 Interrupt
    __enable_irq ();
    #endif
#endif
  }
  InfoLog("%u", MyTick_GetTickMs());
}

void Ele_RightKeyPressProcess(void)
{
#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
  uint32_t wfPeriodUs = 0;
#endif
  if(rightEle.pressVibrateDurationUs)
  {
    #if SINGLE_COIL_AT_A_TIME
    if(ELE_PRESS_VIBRATING == leftEle.status || ELE_RELEASE_VIBRATING == leftEle.status)
    {
      gxCacheVibrate.flag = 1;
      gxCacheVibrate.pos = ELE_POS_RIGHT;
      gxCacheVibrate.tick = MyTick_GetTickMs();
      return;
    }
    #endif
     
#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
     wfPeriodUs = Ele_CalculateDuration(ELE_POS_RIGHT, nowHallSegment[1], rightEle.pressVibrateDurationUs);
     rightEle.status = ELE_PRESS_VIBRATING;
     rightEle.vibrateBeginTick = MyTick_GetTickMs();
     if(Config_GetCoilDriveMode())
     {
        wfCustom.pWfData = Config_GetPwmTable();
        wfCustom.wfDataLen = 100;
        pRightWf = &wfCustom;
     }
     else
     {
        pRightWf = &wfHigh;
     }
     MyWaveform_StartGenerate(CHANNEL_1, pRightWf, wfPeriodUs, 1);
#else
    #if BRAKE_FUNC_ENABLE
    rightEle.status = ELE_PRESS_VIBRATING;
    Ele_RightOpen(); //consider 1us ontime
    CT16B1_DelayUs((uint16_t)Ele_CalculateDuration(ELE_POS_RIGHT, nowHallSegment[1], rightEle.pressVibrateDurationUs));
    Ele_RightClose();
    if(rightEle.brakeEnable)
    {
      CT16B1_DelayUs(rightEle.brakeDelayUs);
      Ele_RightOpen();
      CT16B1_DelayUs(rightEle.brakeDurationUs);
      Ele_RightClose();
    }
    rightEle.status = ELE_RELEASE_DEBOUNCE;
    rightEle.debounceBeginTick = MyTick_GetTickMs();
    #else
    LPC_CT32B0->MCR &= ~(0x01 << 3);   //disable MR1 interrupt
    rightEle.status = ELE_PRESS_VIBRATING;
    Ele_RightOpen();
    __disable_irq();
    LPC_CT32B0->MR1 = LPC_CT32B0->TC + 1 + Ele_CalculateDuration(ELE_POS_RIGHT, nowHallSegment[1], rightEle.pressVibrateDurationUs);
    LPC_CT32B0->MCR |= (0x01 << 3);   //Enable MR1 Interrupt
    __enable_irq ();
    #endif
#endif
  }
  InfoLog("%u", MyTick_GetTickMs());
}
void Ele_RightKeyReleaseProcess(void)
{
#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
  uint32_t wfPeriodUs = 0;
#endif
  if(rightEle.releaseVibrateDurationUs)
  {
    #if SINGLE_COIL_AT_A_TIME
    if(ELE_PRESS_VIBRATING == leftEle.status || ELE_RELEASE_VIBRATING == leftEle.status)
    {
      gxCacheVibrate.flag = 1;
      gxCacheVibrate.pos = ELE_POS_RIGHT;
      gxCacheVibrate.tick = MyTick_GetTickMs();
      return;
    }
    #endif
     
#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
    wfPeriodUs = Ele_CalculateDuration(ELE_POS_RIGHT,nowHallSegment[1], rightEle.releaseVibrateDurationUs);
    rightEle.status = ELE_RELEASE_VIBRATING;
    rightEle.vibrateBeginTick = MyTick_GetTickMs();
    if(Config_GetCoilDriveMode())
    {
       wfCustom.pWfData = Config_GetPwmTable();
       wfCustom.wfDataLen = 100;
       pRightWf = &wfCustom;
    }
    else
    {
       pRightWf = &wfHigh;
    }
    MyWaveform_StartGenerate(CHANNEL_1, pRightWf, wfPeriodUs, 1);
#else
    #if BRAKE_FUNC_ENABLE
    rightEle.status = ELE_PRESS_VIBRATING;
    Ele_RightOpen(); //consider 1us ontime
    CT16B1_DelayUs((uint16_t)Ele_CalculateDuration(ELE_POS_RIGHT, nowHallSegment[1], rightEle.releaseVibrateDurationUs));
    Ele_RightClose();
    if(rightEle.relBrakeEnable)
    {
      CT16B1_DelayUs(rightEle.relBrakeDelayUs);
      Ele_RightOpen();
      CT16B1_DelayUs(rightEle.relBrakeDurationUs);
      Ele_RightClose();
    }
    rightEle.status = ELE_RELEASE_DEBOUNCE;
    rightEle.debounceBeginTick = MyTick_GetTickMs();
    #else
    LPC_CT32B0->MCR &= ~(0x01 << 3);   //disable MR1 interrupt
    rightEle.status = ELE_RELEASE_VIBRATING;
    Ele_RightOpen();
    __disable_irq();
    LPC_CT32B0->MR1 = LPC_CT32B0->TC + 1 + Ele_CalculateDuration(ELE_POS_RIGHT,nowHallSegment[1], rightEle.releaseVibrateDurationUs);
    LPC_CT32B0->MCR |= (0x01 << 3);   //Enable MR1 Interrupt
    __enable_irq ();
    #endif
#endif
  }
  InfoLog("%u", MyTick_GetTickMs());
}

void Ele_K4KeyPressProcess(void)
{
#if 1
  (void)updateFlagL;
#else
  static uint32_t vibrateDruationUs = (uint32_t)(-100);
  
  vibrateDruationUs += 100;
  if(vibrateDruationUs > MAX_VIBRATE_DURATION_US)
  {
    vibrateDruationUs = MAX_VIBRATE_DURATION_US;
  }
  
  if(updateFlagL)
  {
    updateFlagL = 0;
    vibrateDruationUs = 0;
  }
  leftEle.pressVibrateDurationUs = vibrateDruationUs;
  leftEle.releaseVibrateDurationUs = vibrateDruationUs;
  
  InfoLog("us: %d", leftEle.pressVibrateDurationUs);
#endif
}
void Ele_K5KeyPressProcess(void)
{
#if 1
  (void)updateFlagR;
#else
  static uint32_t vibrateDruationUs = (uint32_t)(-100);
  
  vibrateDruationUs += 100;
  if(vibrateDruationUs > MAX_VIBRATE_DURATION_US)
  {
    vibrateDruationUs = MAX_VIBRATE_DURATION_US;
  }
  
  if(updateFlagR)
  {
    updateFlagR = 0;
    vibrateDruationUs = 0;
  }
  rightEle.pressVibrateDurationUs = vibrateDruationUs;
  rightEle.releaseVibrateDurationUs = vibrateDruationUs;
  
  InfoLog("us: %d", rightEle.pressVibrateDurationUs);
#endif
}



/*****static functions*********************************************************/
#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
#else
static void Ele_InitCT32B0(void)
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
  NVIC_EnableIRQ(TIMER_32_0_IRQn) ;
}
#endif
//static void Ele_InitDurationScaleArray(void)
//{
//  /* actualDurationMs = (settingDuration * (100 / (segment * 2 + 100))) */
//  for(uint8_t segment = 0; segment <= MAX_SEGMENT; segment++)
//  {
//    durationScale[segment] = ((100 << 10) / (segment * 2 + 100));
//  }
//}

static uint32_t Ele_CalculateDuration(Ele_Position_te pos, uint8_t segment, uint32_t settingDuration)
{
  /* actualDurationMs = (settingDuration * (100 / (segment * 2 + 100))) */
  uint32_t duty = settingDuration;
  uint8_t index = 0xFF;
  
  switch(pos)
  {
    case ELE_POS_LEFT:
      {
        index = 0;
      }
      break;
    case ELE_POS_RIGHT:
      {
        index = 1;
      }
      break;
    default:
      return (uint32_t)duty;
  }
  if(xCoilDebugCfg[index].bDynOnTimeEn)
  {
    if(segment <= MAX_SEGMENT)
    {
      duty = (settingDuration * durationScale[segment]) >> 10;
    }
    else
    {
      duty = (settingDuration * durationScale[MAX_SEGMENT]) >> 10;
    }
  }
  else
  {
    duty = settingDuration;
  }
  
  return (uint32_t)duty;
}

static void Ele_StatusTask(Ele_Handle_ts *pEle)
{
#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
  switch(pEle->status)
  {
    case ELE_NO_VIBRATING:
      ;
      break;
    case ELE_PRESS_VIBRATING:
    {
      if((MyTick_GetTickMs() - pEle->vibrateBeginTick) * 1000U > (pEle->pressVibrateDurationUs + 999U))
      {
        pEle->status = ELE_RELEASE_DEBOUNCE;
        pEle->debounceBeginTick = MyTick_GetTickMs();
      }
    }
    break;
    case ELE_RELEASE_VIBRATING:
    {
      if((MyTick_GetTickMs() - pEle->vibrateBeginTick) * 1000U > (pEle->releaseVibrateDurationUs + 999U))
      {
        pEle->status = ELE_RELEASE_DEBOUNCE;
        pEle->debounceBeginTick = MyTick_GetTickMs();
      }
    }
    break;
    case ELE_RELEASE_DEBOUNCE:
      if((MyTick_GetTickMs() - pEle->debounceBeginTick) > VIBRATE_DEBOUNCE_TIME_MS)
      {
        pEle->status = ELE_NO_VIBRATING;
        InfoLog("%s release debounce end: %u", pEle->pStr, MyTick_GetTickMs());
      }
      break;
    default:
      break;
  }
#else
  switch(pEle->status)
  {
    case ELE_NO_VIBRATING:
      ;
      break;
    case ELE_PRESS_VIBRATING:
      ;
      break;
    case ELE_RELEASE_VIBRATING:
      ;
      break;
    case ELE_RELEASE_DEBOUNCE:
      if((MyTick_GetTickMs() - pEle->debounceBeginTick) > VIBRATE_DEBOUNCE_TIME_MS)
      {
        pEle->status = ELE_NO_VIBRATING;
        InfoLog("%s release debounce end: %u", pEle->pStr, MyTick_GetTickMs());
      }
      break;
    default:
      break;
  }
#endif
}

#if (defined(GT_LINA_T1A_H5_PWM) || defined(HAPTIC_V4_PWM))
#else
static void Ele_Daemon(void)
{
#if (DAEMON_LIMIT_TURN_ON_TIME_MS >= 8)
  POLLING_INTERVAL_MS(DAEMON_LIMIT_TURN_ON_TIME_MS / 8);
  static uint8_t counterL = 0;
  static uint8_t counterR = 0;
  
  counterL <<= 1;
  counterR <<= 1;
  
  if(ELE_PRESS_VIBRATING == leftEle.status || ELE_RELEASE_VIBRATING == leftEle.status)
  {
    counterL |= 0x01;
  }
  
  if(ELE_PRESS_VIBRATING == rightEle.status || ELE_RELEASE_VIBRATING == rightEle.status)
  {
    counterR |= 0x01;
  }
  
  if(0xFF == counterL)
  {
    counterL = 0;
    Ele_LeftClose();
    leftEle.status = ELE_RELEASE_DEBOUNCE;
    leftEle.debounceBeginTick = MyTick_GetTickMs();
    InfoLog("Error: daemon L");
  }
  if(0xFF == counterR)
  {
    counterR = 0;
    Ele_RightClose();
    rightEle.status = ELE_RELEASE_DEBOUNCE;
    rightEle.debounceBeginTick = MyTick_GetTickMs();
    InfoLog("Error: daemon R");
  }
#endif
}
#endif

#if SINGLE_COIL_AT_A_TIME
static void Ele_OnlyOne(void)
{
  if(gxCacheVibrate.flag)
  {
    if((MyTick_GetTickMs() - gxCacheVibrate.tick) > (MAX_VIBRATE_DURATION_US/1000))
    {
      gxCacheVibrate.flag = 0;
      if(ELE_POS_LEFT == gxCacheVibrate.pos)
      {
        Ele_LeftKeyReleaseProcess();
      }
      else if(ELE_POS_RIGHT == gxCacheVibrate.pos)
      {
        Ele_RightKeyReleaseProcess();
      }
    }
  }
}
#endif


static void Ele_LeftAutoTrigger(void)
{
  if(xCoilDebugCfg[0].bAutoVibrationEn)
  {
    uint16_t periodMs = xCoilDebugCfg[0].wAutoVibTrigPeriodMs;
    if(periodMs < MIN_AUTO_VIB_TRIG_PERIOD_MS)
    {
      periodMs = MIN_AUTO_VIB_TRIG_PERIOD_MS;
    }
    POLLING_INTERVAL_MS(periodMs);
    Ele_LeftKeyPressProcess();
  }
}
static void Ele_RightAutoTrigger(void)
{
  if(xCoilDebugCfg[1].bAutoVibrationEn)
  {
    uint16_t periodMs = xCoilDebugCfg[1].wAutoVibTrigPeriodMs;
    if(periodMs < MIN_AUTO_VIB_TRIG_PERIOD_MS)
    {
      periodMs = MIN_AUTO_VIB_TRIG_PERIOD_MS;
    }
    POLLING_INTERVAL_MS(periodMs);
    Ele_RightKeyPressProcess();
  }
}

static void Ele_DebugTask(void)
{
  #if (DEBUG_STARTUP_FIX_VIB_PARAM_EN)
  static uint8_t firstFlag = 0;
  if(!firstFlag)
  {
    if(MyTick_GetTickMs() >= 1500)
    {
      firstFlag = 0xFF;
      leftEle.pressVibrateDurationUs = DEBUG_STARTUP_FIX_VIB_PARAM_US;
      leftEle.releaseVibrateDurationUs = DEBUG_STARTUP_FIX_VIB_PARAM_US;
      rightEle.pressVibrateDurationUs = DEBUG_STARTUP_FIX_VIB_PARAM_US;
      rightEle.releaseVibrateDurationUs = DEBUG_STARTUP_FIX_VIB_PARAM_US;
    }
  }
  #endif
}







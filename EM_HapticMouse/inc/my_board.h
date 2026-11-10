/*******************************************************************************
 * @file    my_board.h
 * @brief
 * @author  chengjie
 * @date    created on 2026-06-25
 * @version V1.0
 * @note
*******************************************************************************/
#ifndef __my_board_h__
#define __my_board_h__

#include "stdint.h"
#include "LPC11Uxx.h"

/**********define hardware information **************************/
//#define HAPTIC_V2
//#define HAPTIC_V3
//#define HAPTIC_V4
//#define P723_POC
//#define GEN_2_H_V1
//#define IRPT_V4
//#define P723_POC_PCB1_V07
//#define V4_HALL_HPTIC_V4
//#define V4_HALL_HPTIC_V4_G2_0923
//#define GT_LINA_T1A_H5
//#define GT_LINA_T1A_H5_PWM
#define HAPTIC_V4_PWM


#if (defined(P723_POC_PCB1_V07))
  #define P723_POC
#endif
#if (defined(V4_HALL_HPTIC_V4_G2_0923))
  #define V4_HALL_HPTIC_V4
#endif

#if (defined(GT_LINA_T1A_H5_PWM))
  #define GT_LINA_T1A_H5
  #define DISABLE_K4_K5_KEY
#endif

#if (defined(HAPTIC_V4_PWM))
  #define HAPTIC_V4
#endif



#define ENABLE_WHEEL_SCAN   0

#if (defined(HAPTIC_V2) || defined (HAPTIC_V3))
//gpio define
#define FRONT_BUTTON_PORT   0
#define FRONT_BUTTON_PIN    1
#define FRONT_BUTTON        PIO0_1
#define BACK_BUTTON_PORT    0
#define BACK_BUTTON_PIN     23
#define BACK_BUTTON         PIO0_23
#define MIDDLE_BUTTON_PORT  0
#define MIDDLE_BUTTON_PIN   12
#define MIDDLE_BUTTON       PIO0_12
#define DPI_BUTTON_PORT     0
#define DPI_BUTTON_PIN      11
#define DPI_BUTTON          PIO0_11
//Sensor
#define MOTION_PORT         0
#define MOTION_PIN          2
#define MOTION              PIO0_2
#define NCS_PORT            0
#define NCS_PIN             20
#define NCS                 PIO0_20
//Encoder
#define ZA_PORT             0
#define ZA_PIN              13
#define ZA                  TDO_PIO0_13
#define ZB_PORT             0
#define ZB_PIN              14
#define ZB                  TRST_PIO0_14
//Hall
#define HALL_EN_PORT        0
#define HALL_EN_PIN         19
#define HALL_EN             PIO0_19
//Coil
#define ETA1626_EN_PORT     0
#define ETA1626_EN_PIN      16
#define ETA1626_EN          PIO0_16
#define NMOS1_PORT          0
#define NMOS1_PIN           17
#define NMOS1               PIO0_17
#define NMOS2_PORT          0
#define NMOS2_PIN           18
#define NMOS2               PIO0_18
#define PWM1_PORT           0
#define PWM1_PIN            4
#define PWM1                PIO0_4
#define PWM2_PORT           0
#define PWM2_PIN            5
#define PWM2                PIO0_5

#elif (defined(HAPTIC_V4))
//gpio define
#define FRONT_BUTTON_PORT   0
#define FRONT_BUTTON_PIN    1
#define FRONT_BUTTON        PIO0_1
#define BACK_BUTTON_PORT    0
#define BACK_BUTTON_PIN     23
#define BACK_BUTTON         PIO0_23
#define MIDDLE_BUTTON_PORT  0
#define MIDDLE_BUTTON_PIN   12
#define MIDDLE_BUTTON       PIO0_12
#define DPI_BUTTON_PORT     0
#define DPI_BUTTON_PIN      11
#define DPI_BUTTON          PIO0_11
//Sensor
#define MOTION_PORT         0
#define MOTION_PIN          2
#define MOTION              PIO0_2
#define NCS_PORT            0
#define NCS_PIN             20
#define NCS                 PIO0_20
//Encoder
#define ZA_PORT             0
#define ZA_PIN              4
#define ZA                  PIO0_4
#define ZB_PORT             0
#define ZB_PIN              5
#define ZB                  PIO0_5
//Hall
#define HALL_EN_PORT        0
#define HALL_EN_PIN         19
#define HALL_EN             PIO0_19
//Coil
#define VDD_MOTOR_EN_PORT   0
#define VDD_MOTOR_EN_PIN    17
#define VDD_MOTOR_EN        PIO0_17
#define PWM1_PORT           0
#define PWM1_PIN            13
#define PWM1                TDO_PIO0_13
#define PWM2_PORT           0
#define PWM2_PIN            14
#define PWM2                TRST_PIO0_14

#elif (defined(P723_POC))
//gpio define
#define FRONT_BUTTON_PORT   1
#define FRONT_BUTTON_PIN    19
#define FRONT_BUTTON        PIO1_19
#define BACK_BUTTON_PORT    0
#define BACK_BUTTON_PIN     7
#define BACK_BUTTON         PIO0_7
#define MIDDLE_BUTTON_PORT  0
#define MIDDLE_BUTTON_PIN   13
#define MIDDLE_BUTTON       TDO_PIO0_13
#define DPI_BUTTON_PORT     0
#define DPI_BUTTON_PIN      14
#define DPI_BUTTON          TRST_PIO0_14
//Sensor
#define MOTION_PORT         0
#define MOTION_PIN          2
#define MOTION              PIO0_2
#define NCS_PORT            0
#define NCS_PIN             20
#define NCS                 PIO0_20
//Encoder
#define ZA_PORT             0
#define ZA_PIN              11
#define ZA                  TDI_PIO0_11
#define ZB_PORT             0
#define ZB_PIN              12
#define ZB                  TMS_PIO0_12
//Hall
#define HALL_EN_PORT        0
#define HALL_EN_PIN         19
#define HALL_EN             PIO0_19
//Coil
#define ETA1626_EN_PORT     0
#define ETA1626_EN_PIN      16
#define ETA1626_EN          PIO0_16
#define NMOS_D1_PORT        0
#define NMOS_D1_PIN         17
#define NMOS_D1             PIO0_17
#define NMOS_D2_PORT        0
#define NMOS_D2_PIN         18
#define NMOS_D2             PIO0_18
#define PWM1_PORT           0
#define PWM1_PIN            4
#define PWM1                PIO0_4
#define PWM2_PORT           0
#define PWM2_PIN            5
#define PWM2                PIO0_5

#elif (defined(GEN_2_H_V1))
//gpio define
#define FRONT_BUTTON_PORT   0
#define FRONT_BUTTON_PIN    1
#define FRONT_BUTTON        PIO0_1
#define BACK_BUTTON_PORT    1
#define BACK_BUTTON_PIN     19
#define BACK_BUTTON         PIO1_19
#define MIDDLE_BUTTON_PORT  0
#define MIDDLE_BUTTON_PIN   12
#define MIDDLE_BUTTON       PIO0_12
#define DPI_BUTTON_PORT     0
#define DPI_BUTTON_PIN      11
#define DPI_BUTTON          PIO0_11

//Sensor
#define MOTION_PORT         1
#define MOTION_PIN          0
#define MOTION              PIO1_0
#define NCS_PORT            1
#define NCS_PIN             7
#define NCS                 PIO1_7
//Encoder
#define ZA_PORT             0
#define ZA_PIN              13
#define ZA                  TDO_PIO0_13
#define ZB_PORT             0
#define ZB_PIN              14
#define ZB                  TRST_PIO0_14
//Hall
#define HALL_EN_PORT        1
#define HALL_EN_PIN         25
#define HALL_EN             PIO1_25
//Coil
#define NSLEEP_PORT         1
#define NSLEEP_PIN          13
#define NSLEEP              PIO1_13
#define PWM1_PORT           1
#define PWM1_PIN            5
#define PWM1                PIO1_5
#define PWM2_PORT           1
#define PWM2_PIN            28
#define PWM2                PIO1_28
#define PWM3_PORT           1
#define PWM3_PIN            17
#define PWM3                PIO1_17
#define PWM4_PORT           1
#define PWM4_PIN            23
#define PWM4                PIO1_23

#elif (defined(IRPT_V4))
//gpio define
#define FRONT_BUTTON_PORT   0
#define FRONT_BUTTON_PIN    1
#define FRONT_BUTTON        PIO0_1
#define BACK_BUTTON_PORT    0
#define BACK_BUTTON_PIN     23
#define BACK_BUTTON         PIO0_23
#define MIDDLE_BUTTON_PORT  0
#define MIDDLE_BUTTON_PIN   12
#define MIDDLE_BUTTON       PIO0_12
#define DPI_BUTTON_PORT     0
#define DPI_BUTTON_PIN      11
#define DPI_BUTTON          PIO0_11
//Sensor
#define MOTION_PORT         0
#define MOTION_PIN          2
#define MOTION              PIO0_2
#define NCS_PORT            0
#define NCS_PIN             20
#define NCS                 PIO0_20
//Encoder
#define ZA_PORT             0
#define ZA_PIN              17
#define ZA                  PIO0_17
#define ZB_PORT             0
#define ZB_PIN              18
#define ZB                  PIO0_18
//IRPT
#define IR_EN_PORT          0
#define IR_EN_PIN           19
#define IR_EN               PIO0_19
#define LB_ADC_PORT         0
#define LB_ADC_PIN          13
#define LB_ADC              TDO_PIO0_13
#define RB_ADC_PORT         0
#define RB_ADC_PIN          14
#define RB_ADC              TRST_PIO0_14
//Coil
#define HP_EN_PORT          0
#define HP_EN_PIN           16
#define HP_EN               PIO0_16
#define PWM1_PORT           0
#define PWM1_PIN            4
#define PWM1                PIO0_4
#define PWM2_PORT           0
#define PWM2_PIN            5
#define PWM2                PIO0_5

#elif (defined(V4_HALL_HPTIC_V4))
//gpio define
#define FRONT_BUTTON_PORT   0
#define FRONT_BUTTON_PIN    1
#define FRONT_BUTTON        PIO0_1
#define BACK_BUTTON_PORT    0
#define BACK_BUTTON_PIN     23
#define BACK_BUTTON         PIO0_23
#define MIDDLE_BUTTON_PORT  0
#define MIDDLE_BUTTON_PIN   12
#define MIDDLE_BUTTON       PIO0_12
#define DPI_BUTTON_PORT     0
#define DPI_BUTTON_PIN      11
#define DPI_BUTTON          PIO0_11
//Sensor
#define MOTION_PORT         0
#define MOTION_PIN          2
#define MOTION              PIO0_2
#define NCS_PORT            0
#define NCS_PIN             20
#define NCS                 PIO0_20
//Encoder
#define ZA_PORT             0
#define ZA_PIN              17
#define ZA                  PIO0_17
#define ZB_PORT             0
#define ZB_PIN              18
#define ZB                  PIO0_18
//Hall
#define HALL_EN_PORT        0
#define HALL_EN_PIN         19
#define HALL_EN             PIO0_19
//Coil
#define HP_EN_PORT          0
#define HP_EN_PIN           16
#define HP_EN               PIO0_16
#define PWM1_PORT           0
#define PWM1_PIN            4
#define PWM1                PIO0_4
#define PWM2_PORT           0
#define PWM2_PIN            5
#define PWM2                PIO0_5

#elif (defined(GT_LINA_T1A_H5))
//gpio define
#define FRONT_BUTTON_PORT   0
#define FRONT_BUTTON_PIN    11
#define FRONT_BUTTON        TDI_PIO0_11
#define BACK_BUTTON_PORT    0
#define BACK_BUTTON_PIN     12
#define BACK_BUTTON         TMS_PIO0_12
#define MIDDLE_BUTTON_PORT  0
#define MIDDLE_BUTTON_PIN   7
#define MIDDLE_BUTTON       PIO0_7
#define DPI_BUTTON_PORT     0
#define DPI_BUTTON_PIN      23
#define DPI_BUTTON          PIO0_23
//Sensor
#define MOTION_PORT         0
#define MOTION_PIN          2
#define MOTION              PIO0_2
#define NCS_PORT            0
#define NCS_PIN             20
#define NCS                 PIO0_20
//Encoder
#define ZA_PORT             0
#define ZA_PIN              17
#define ZA                  PIO0_17
#define ZB_PORT             0
#define ZB_PIN              18
#define ZB                  PIO0_18
//Hall
#define HALL_EN_PORT        0
#define HALL_EN_PIN         16
#define HALL_EN             PIO0_16
//Coil
#define HP_EN_PORT          1
#define HP_EN_PIN           19
#define HP_EN               PIO1_19
#define PWM1_PORT           0
#define PWM1_PIN            13
#define PWM1                TDO_PIO0_13
#define PWM2_PORT           0
#define PWM2_PIN            14
#define PWM2                TRST_PIO0_14



#else
  #error "Hardware information is undefined."
#endif


#define IOCTRL_NoPull(x)    do{ LPC_IOCON->x &= (~((0x01 << 3) | (0x01 << 4)));}while(0)
#define IOCTRL_PullUp(x)    do{ LPC_IOCON->x &= (~((0x01 << 3) | (0x01 << 4)));\
                                LPC_IOCON->x |= (0x01 << 4);}while(0)
#define IOCTRL_PullDown(x)  do{ LPC_IOCON->x &= (~((0x01 << 3) | (0x01 << 4)));\
                                LPC_IOCON->x |= (0x01 << 3);}while(0)
#define IOCTR_PushPull(x)   do{ LPC_IOCON->x &= (~(1u << 10));}while(0)
#define IOCTR_OpenDrain(x)  do{ LPC_IOCON->x |= (1u << 10);}while(0)
#define IOCTRL_CfgAsOutput(port, pin) do{LPC_GPIO->DIR[port] |= (1 << pin);}while(0)
#define IOCTRL_CfgAsInput(port, pin)  do{LPC_GPIO->DIR[port] &= ~(1 << pin);}while(0)
#define IOCTRL_HighLevel(port, pin)   do{LPC_GPIO->SET[port] = (1 << pin);}while(0)
#define IOCTRL_LowLevel(port, pin)    do{LPC_GPIO->CLR[port] = (1 << pin);}while(0)



#endif /* __my_board_h__ */


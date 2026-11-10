/*******************************************************************************
 * @file    my_protocol_common.h
 * @brief
 * @author  chengjie
 * @date    created on 2026-04-11
 * @version V0.1
 * @note
*******************************************************************************/
#ifndef __my_protocol_common_h__
#define __my_protocol_common_h__

#include "my_protocol.h"


/******************************************************************************/
#define VK_RESERVE_0X00 0x00 //Don't use
#define VK_RESERVE_0XFF 0xFF //Don't use
#define VK_LBUTTON  0x01 // mouse left button
#define VK_RBUTTON  0x02 // mouse right button
#define PACKET_PREAMBLER 0x55

typedef enum{
  ID_RESERVE_0X0F = 0xFF,
  ID_RESERVE_0X00 = 0x00,
  ID_HOST = 0x01,
  ID_MOUSE = 0x02,
  ID_KEYBOARD = 0x03,
}IdentifierList_te;

typedef enum{
  PAYLOADTYPE_RESERVE_0XFF = 0xFF,
  PAYLOADTYPE_RESERVE_0X00 = 0x00,
  PAYLOADTYPE_NOTIFICATION = 0x01, /* Don't need respond */
  PAYLOADTYPE_REQUEST = 0x02,      /* need respond */
  PAYLOADTYPE_RESPONSE = 0x03,
}PAYLOADTYPE_List_te;

typedef enum {
  CMD_RESERVE_0XFF = 0xFF,
  CMD_RESERVE_0X00 = 0x00,
  CMD_GET_INFO = 0x01,
  CMD_GET_CONFIG = 0x02,
  CMD_SET_CONFIG = 0x03,
  CMD_SAVE_CONFIG = 0x04,
  CMD_GET_DEBUG = 0x05,
  CMD_SET_DEBUG = 0x06,
  CMD_GET_MFG = 0x07,
  CMD_SET_MFG = 0x08,
}CommandList_te;

typedef enum{
  STATUS_FAIL = 0x00, 
  STATUS_SUCCESS = 0x01,
}CmdStatusList_te;

/*********************INFO SubCommand******************************************/
typedef enum {
  INFO_RESERVE_0X00 = 0x00,
  INFO_RESERVE_0XFF = 0xFF,
  INFO_HALL_RAW = 0x01,
  INFO_HALL_SEGMENT = 0x02,
  INFO_COIL = 0x03,
}InfoList_te;

typedef struct __attribute__((packed)) { /* 6bytes */
  uint16_t wCurrentRaw;
  uint16_t wBaselineBottom;
  uint16_t wBaselineTop;
}HallRawInfo_ts;

typedef struct __attribute__((packed)) { /* 4bytes */
  uint16_t wCurrentSegmentValue;
  uint16_t wMaxSegmentValue;
}HallSegmentInfo_ts;

typedef struct __attribute__((packed)) { /* 6bytes */
  uint16_t wCoilVoltageMv0;
  uint16_t wCoilVoltageMv1;
  uint16_t wCoilVoltageMv2;
}CoilInfo_ts;

/*
typedef struct __attribute__((packed)) {
  HallRawInfo_ts xRawInfo;
  HallSegmentInfo_ts xSegmentInfo;
  CoilInfo_ts xCoilInfo;
}AllInfo_ts;
*/

/*********************CONFIG SubCommand****************************************/
typedef enum {
  CFG_RESERVE_0X00 = 0x00,
  CFG_RESERVE_0XFF = 0xFF,
  CFG_RAPID_TRIGGER = 0x01,
  CFG_COIL = 0x02,
  CFG_COIL_RELEASE = 0x03,
  CFG_COIL_BRAKE = 0x04,
  CFG_COIL_BRAKE_REL = 0x05,
  CFG_COIL_DRIVE_MODE = 0x06,
  CFG_COIL_PWM = 0x07,
}ConfigList_te;

typedef struct __attribute__((packed)) {/* 8bytes */
  uint8_t bReserve;
  uint8_t bmEnableMask;
  /* bmEnableMask:
   * bit[7]: Enable allow. 
   * bit[6]: Enable ActuationPoint.
   * bit[5]: Enable RapidTriggerPress.
   * bit[4]: Enable RapidTriggerRelease.
   * bit[3:0]: Reserve.
  */
  uint16_t wSegmentActuationPoint;
  uint16_t wSegmentRapidTriggerPress;
  uint16_t wSegmentRapidTriggerRelease;
}RapidTriggerConfig_ts;

typedef struct __attribute__((packed)) {/* 4bytes*/
  uint8_t bReserve;
  uint8_t bmEnableMask;
  /* bmEnableMask:
   * bit[7]: Enable allow. 
   * bit[6:4]: Reserve.
   * bit[3:0]: Select Number of wCoilVoltageMvX,
  */
  uint16_t wCoilOnTimeUs;
}CoilConfig_ts; //CoilPressConfig_ts

typedef struct __attribute__((packed)) {/* 4bytes*/
  uint8_t bReserve;
  uint8_t bmEnableMask;
  /* bmEnableMask:
   * bit[7]: Enable allow. 
   * bit[6:4]: Reserve.
   * bit[3:0]: Select Number of wCoilVoltageMvX,
  */
  uint16_t wCoilOnTimeUs;
}CoilReleaseConfig_ts;

typedef struct __attribute__((packed)) {/* 6bytes*/
  uint8_t bReserve;
  uint8_t bmEnableMask;
  /* bmEnableMask:
   * bit[7]: Enable allow. 
   * bit[6:4]: Reserve.
   * bit[3:0]: Select Number of wCoilVoltageMvX,
  */
  uint16_t wBrakeDlyUs; //delay
  uint16_t wBrakeDurUs; //duration
}CoilBrakeConfig_ts;

typedef struct __attribute__((packed)) {/* 6bytes*/
  uint8_t bReserve;
  uint8_t bmEnableMask;
  /* bmEnableMask:
   * bit[7]: Enable allow. 
   * bit[6:4]: Reserve.
   * bit[3:0]: Select Number of wCoilVoltageMvX,
  */
  uint16_t wBrakeDlyUs; //delay
  uint16_t wBrakeDurUs; //duration
}CoilBrakeRelConfig_ts;

typedef struct __attribute__((packed)) {/* 1bytes*/
  uint8_t bDriveMode;        //0: level    1: pwm
}CoilDriveModeConfig_ts;



/*
typedef struct __attribute__((packed)) {
  RapidTriggerConfig_ts xRtCfg; 
  CoilConfig_ts xCoilCfg;
  CoilReleaseConfig_ts xCoilReleaseCfg;
  CoilBrakeConfig_ts xCoilBrakeCfg;
  CoilBrakeRelConfig_ts xCoilBrakeRelCfg;
}AllConfig_ts;*/

/*********************DEBUG SubCommand*****************************************/
typedef enum {
  DEBUG_RESERVE_0X00 = 0x00,
  DEBUG_RESERVE_0XFF = 0xFF,
  DEBUG_COIL = 0x01,
}DebugList_te;

typedef struct __attribute__((packed)) {/* 4bytes*/
  uint8_t bDynOnTimeEn;
  uint8_t bAutoVibrationEn;
  uint16_t wAutoVibTrigPeriodMs;
}CoilDebug_ts;

/*********************MFG SubCommand*****************************************/
typedef enum {
  MFG_RESERVE_0X00 = 0x00,
  MFG_RESERVE_0XFF = 0xFF,
  MFG_ALL = 0xFF,
  MFG_VIB_ZERO_POINT = 0x01,
}MfgList_te;

typedef struct __attribute__((packed)) {/* 2bytes*/
  uint16_t wZeroPoint;
}ZeroPointMfg_ts;


#endif /* __my_protocol_common_h__ */




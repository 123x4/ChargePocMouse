/*******************************************************************************
 * @file    my_protocol_mouse.c
 * @brief
 * @author  chengjie
 * @date    created on 2026-04-08
 * @version V1.x
 * @note
*******************************************************************************/
#include "my_protocol_mouse.h"
#include "IAP_CMD.h"
#include "my_misc.h"
#include "my_tick.h"
#include "my_electromagnet.h"
#include "operation_flash.h"
#include "my_debug.h"

#define my_assert(x)    do{if(!(x)){return -1;}}while(0)

//#define EEPROM_ADDR_START     0xF00 /*0xF00 ~ 0xFBF, 192Bytes*/
#define EEPROM_ADDR_START     0xE00 /*0xE00 ~ 0xFBF, 448Bytes*/
#define EEPROM_ADDR_VERSION   (EEPROM_ADDR_START + 0x00) //4bytes
#define EEPROM_ADDR_CHECKSUM  (EEPROM_ADDR_START + 0x04) //1bytes
#define EEPROM_ADDR_PARAM     (EEPROM_ADDR_START + 0x06)


typedef struct __attribute__((packed)) {
  HallRawInfo_ts xRawInfo;
  HallSegmentInfo_ts xSegmentInfo;
  CoilInfo_ts xCoilInfo;
}AllInfo_ts;

typedef struct __attribute__((packed)) {
  RapidTriggerConfig_ts xRtCfg; 
  CoilConfig_ts xCoilCfg;
  CoilReleaseConfig_ts xCoilReleaseCfg;
  CoilBrakeConfig_ts xCoilBrakeCfg;
  CoilBrakeRelConfig_ts xCoilBrakeRelCfg;
}AllConfig_ts;


typedef struct __attribute__((packed)) {
  AllInfo_ts xInfo[2];
  AllConfig_ts xConfig[2];
  CoilDriveModeConfig_ts xDriveMode;
  uint8_t gPwmWf[100];
}AllParam_ts;

AllParam_ts xgAllParam = {0};
AllInfo_ts *pxgAllInfo = &(xgAllParam.xInfo[0]);
AllConfig_ts *pxgAllConfig = &(xgAllParam.xConfig[0]);

#define INVALID_IDX   0xFF
const uint8_t aVcToIdx[] = {
  /*[0 ... 255] = INVALID_IDX,*/
  [VK_LBUTTON] = 1,
  [VK_RBUTTON] = 2,
};


static uint8_t updateCoilCfgFlagL = 0;
static uint8_t updateCoilCfgFlagR = 0;
static uint8_t updateCoilReleaseCfgFlagL = 0;
static uint8_t updateCoilReleaseCfgFlagR = 0;
static uint8_t updateCoilBrakeCfgFlagL = 0;
static uint8_t updateCoilBrakeCfgFlagR = 0;
static uint8_t updateCoilBrakeRelCfgFlagL = 0;
static uint8_t updateCoilBrakeRelCfgFlagR = 0;
static uint8_t updateRtCfgFlag = 0;
static uint8_t updateCoilDriveModeFlag = 0;
static uint8_t updateCoilPwmFlag = 0;


static uint8_t CalculateChecksum(const uint8_t *pData, uint16_t len)
{
  uint8_t sum = 0;
  for(uint16_t i = 0; i < len; i++)
  {
    sum += *(pData + i);
  }
  return sum;
}

void Hall_SyncParam(uint8_t index, HallRawInfo_ts *pxRawInfo, HallSegmentInfo_ts *pxSegInfo)
{
  uint8_t i ;
  
  if(index) // 1
  {
    i = 0;
  }
  else // 0
  {
    i = 1;
  }
  memcpy(&((pxgAllInfo + i)->xRawInfo), pxRawInfo, sizeof(HallRawInfo_ts));
  memcpy(&((pxgAllInfo + i)->xSegmentInfo), pxSegInfo, sizeof(HallSegmentInfo_ts));
  
  
}

uint8_t* Config_GetPwmTable(void)
{
  return &(xgAllParam.gPwmWf[0]);
}
uint8_t Config_GetCoilDriveMode(void)
{
  return xgAllParam.xDriveMode.bDriveMode;
}


static void Init_CfgParamToRun(void)
{
  RapidTriggerConfig_ts *pxRapidTriggerCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_LBUTTON))->xRtCfg);
  Hall_UpdateParam(VK_LBUTTON, pxRapidTriggerCfg);
  pxRapidTriggerCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_RBUTTON))->xRtCfg);
  Hall_UpdateParam(VK_RBUTTON, pxRapidTriggerCfg);
 
  CoilConfig_ts *pxCoilCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_LBUTTON))->xCoilCfg);
  Ele_UpdateParam(VK_LBUTTON, pxCoilCfg);
  pxCoilCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_RBUTTON))->xCoilCfg);
  Ele_UpdateParam(VK_RBUTTON, pxCoilCfg);
  
  CoilReleaseConfig_ts *pxCoilReleaseCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_LBUTTON))->xCoilReleaseCfg);
  Ele_UpdateReleaseParam(VK_LBUTTON, pxCoilReleaseCfg);
  pxCoilReleaseCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_RBUTTON))->xCoilReleaseCfg);
  Ele_UpdateReleaseParam(VK_RBUTTON, pxCoilReleaseCfg);
  
  CoilBrakeConfig_ts *pxBrakeCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_LBUTTON))->xCoilBrakeCfg);
  Ele_UpdateBrakeParam(VK_LBUTTON, pxBrakeCfg);
  pxBrakeCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_RBUTTON))->xCoilBrakeCfg);
  Ele_UpdateBrakeParam(VK_RBUTTON, pxBrakeCfg);
  
  CoilBrakeRelConfig_ts *pxBrakeRelCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_LBUTTON))->xCoilBrakeRelCfg);
  Ele_UpdateRelBrakeParam(VK_LBUTTON, pxBrakeRelCfg);
  pxBrakeRelCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_RBUTTON))->xCoilBrakeRelCfg);
  Ele_UpdateRelBrakeParam(VK_RBUTTON, pxBrakeRelCfg);
  
}

static void ResetDefaultConfig(void)
{
  // Debug
    for(uint8_t i = 0; i < 2; i++)
    {
  //    xgAllParam.xInfo[i].xRawInfo.wCurrentRaw = Protocol_ConvertWord(8880 + i);
  //    xgAllParam.xInfo[i].xRawInfo.wBaselineBottom = Protocol_ConvertWord(12000  + i);
  //    xgAllParam.xInfo[i].xRawInfo.wBaselineTop = Protocol_ConvertWord(6660 + i);
  //    xgAllParam.xInfo[i].xSegmentInfo.wCurrentSegmentValue = Protocol_ConvertWord(0 + i);
      xgAllParam.xInfo[i].xSegmentInfo.wMaxSegmentValue = Protocol_ConvertWord(100);
      xgAllParam.xInfo[i].xCoilInfo.wCoilVoltageMv0 = Protocol_ConvertWord(12000);
      xgAllParam.xInfo[i].xCoilInfo.wCoilVoltageMv1 = Protocol_ConvertWord(15000);
      xgAllParam.xInfo[i].xCoilInfo.wCoilVoltageMv2 = Protocol_ConvertWord(18000);
      
      xgAllParam.xConfig[i].xRtCfg.bReserve = Protocol_ConvertWord(0xAA);
      xgAllParam.xConfig[i].xRtCfg.bmEnableMask = Protocol_ConvertWord((0x01 << 7) | (0x01 << 6) | (0x01 << 5) | (0x01 << 4));
      
      xgAllParam.xConfig[i].xRtCfg.wSegmentActuationPoint = Protocol_ConvertWord(DEFAULT_AP);
      xgAllParam.xConfig[i].xRtCfg.wSegmentRapidTriggerPress = Protocol_ConvertWord(DEFAULT_RTP);
      xgAllParam.xConfig[i].xRtCfg.wSegmentRapidTriggerRelease = Protocol_ConvertWord(DEFAULT_RTR);
      
      xgAllParam.xConfig[i].xCoilCfg.bReserve = Protocol_ConvertWord(0xAA);
      xgAllParam.xConfig[i].xCoilCfg.bmEnableMask = Protocol_ConvertWord((0x01 << 7));
      xgAllParam.xConfig[i].xCoilCfg.wCoilOnTimeUs = Protocol_ConvertWord(DEFAULT_CFG_ONTIME_US); //coil press cfg
      
      xgAllParam.xConfig[i].xCoilReleaseCfg.bReserve = Protocol_ConvertWord(0xAA);
      xgAllParam.xConfig[i].xCoilReleaseCfg.bmEnableMask = Protocol_ConvertWord((0x01 << 7));
      xgAllParam.xConfig[i].xCoilReleaseCfg.wCoilOnTimeUs = Protocol_ConvertWord(DEFAULT_CFG_ONTIME_US); //coil reease cfg
      
      xgAllParam.xConfig[i].xCoilBrakeCfg.bReserve = Protocol_ConvertWord(0xAA);
      xgAllParam.xConfig[i].xCoilBrakeCfg.bmEnableMask = Protocol_ConvertWord(((DEFAULT_CFG_BRAKE_EN & 0x1) << 7)); //Default close
      xgAllParam.xConfig[i].xCoilBrakeCfg.wBrakeDlyUs = Protocol_ConvertWord(DEFAULT_CFG_BRAKE_DELAY_US);
      xgAllParam.xConfig[i].xCoilBrakeCfg.wBrakeDurUs = Protocol_ConvertWord(DEFAULT_CFG_BRAKE_DURATION_US);
      
      xgAllParam.xConfig[i].xCoilBrakeRelCfg.bReserve = Protocol_ConvertWord(0xAA);
      xgAllParam.xConfig[i].xCoilBrakeRelCfg.bmEnableMask = Protocol_ConvertWord(((DEFAULT_CFG_BRAKE_EN & 0x1) << 7)); //Default close
      xgAllParam.xConfig[i].xCoilBrakeRelCfg.wBrakeDlyUs = Protocol_ConvertWord(DEFAULT_CFG_BRAKE_DELAY_US);
      xgAllParam.xConfig[i].xCoilBrakeRelCfg.wBrakeDurUs = Protocol_ConvertWord(DEFAULT_CFG_BRAKE_DURATION_US);
      
    }
    xgAllParam.xDriveMode.bDriveMode = DEFAULT_CFG_DRIVE_MODE;
    for(uint8_t i = 0; i < 50; i++)
    {
      xgAllParam.gPwmWf[i] = i+51;
      xgAllParam.gPwmWf[99 - i] = i+51;
    }
}

void Task_ParamGuard(void)
{
  if(updateRtCfgFlag || 
    updateCoilCfgFlagL || updateCoilCfgFlagR || 
    updateCoilReleaseCfgFlagL || updateCoilReleaseCfgFlagR ||
    updateCoilBrakeCfgFlagL || updateCoilBrakeCfgFlagR ||
    updateCoilBrakeRelCfgFlagL || updateCoilBrakeRelCfgFlagR ||
    updateCoilDriveModeFlag || updateCoilPwmFlag)
  {
    uint8_t sum = 0;
    sum = CalculateChecksum((uint8_t *)(&xgAllParam), sizeof(xgAllParam));
    //disbale interrupt 
    Chip_EEPROM_Write(EEPROM_ADDR_PARAM, (uint8_t *)(&xgAllParam), sizeof(xgAllParam));
    Chip_EEPROM_Write(EEPROM_ADDR_CHECKSUM, &sum, 1);
    //enbele interrupt;
  }
  
  if(updateRtCfgFlag)
  {
    updateRtCfgFlag = 0;
    
    RapidTriggerConfig_ts *pxRapidTriggerCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_LBUTTON))->xRtCfg);
    Hall_UpdateParam(VK_LBUTTON, pxRapidTriggerCfg);
    
    pxRapidTriggerCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_RBUTTON))->xRtCfg);
    Hall_UpdateParam(VK_RBUTTON, pxRapidTriggerCfg);
  }
  
  if(updateCoilCfgFlagL)
  {
    updateCoilCfgFlagL = 0;
    CoilConfig_ts *pxCoilCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_LBUTTON))->xCoilCfg);
    Ele_UpdateParam(VK_LBUTTON, pxCoilCfg);
  }
  if(updateCoilCfgFlagR)
  {
    updateCoilCfgFlagR = 0;
    CoilConfig_ts *pxCoilCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_RBUTTON))->xCoilCfg);
    Ele_UpdateParam(VK_RBUTTON, pxCoilCfg);
  }
  
  if(updateCoilReleaseCfgFlagL)
  {
    updateCoilReleaseCfgFlagL = 0;
    CoilReleaseConfig_ts *pxCoilReleaseCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_LBUTTON))->xCoilReleaseCfg);
    Ele_UpdateReleaseParam(VK_LBUTTON, pxCoilReleaseCfg);
  }
  if(updateCoilReleaseCfgFlagR)
  {
    updateCoilReleaseCfgFlagR = 0;
    CoilReleaseConfig_ts *pxCoilReleaseCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_RBUTTON))->xCoilReleaseCfg);
    Ele_UpdateReleaseParam(VK_RBUTTON, pxCoilReleaseCfg);
  }
  
  if(updateCoilBrakeCfgFlagL)
  {
    updateCoilBrakeCfgFlagL = 0;
    CoilBrakeConfig_ts *pxCoilBrakeCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_LBUTTON))->xCoilBrakeCfg);
    Ele_UpdateBrakeParam(VK_LBUTTON, pxCoilBrakeCfg);
  }
  if(updateCoilBrakeCfgFlagR)
  {
    updateCoilBrakeCfgFlagR = 0;
    CoilBrakeConfig_ts *pxCoilBrakeCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_RBUTTON))->xCoilBrakeCfg);
    Ele_UpdateBrakeParam(VK_RBUTTON, pxCoilBrakeCfg);
  }
  
  if(updateCoilBrakeRelCfgFlagL)
  {
    updateCoilBrakeRelCfgFlagL = 0;
    CoilBrakeRelConfig_ts *pxCoilBrakeRelCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_LBUTTON))->xCoilBrakeRelCfg);
    Ele_UpdateRelBrakeParam(VK_LBUTTON, pxCoilBrakeRelCfg);
  }
  if(updateCoilBrakeRelCfgFlagR)
  {
    updateCoilBrakeRelCfgFlagR = 0;
    CoilBrakeRelConfig_ts *pxCoilBrakeRelCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(VK_RBUTTON))->xCoilBrakeRelCfg);
    Ele_UpdateRelBrakeParam(VK_RBUTTON, pxCoilBrakeRelCfg);
  }
  
  if(updateCoilDriveModeFlag)
  {
    updateCoilDriveModeFlag = 0;
  }
  if(updateCoilPwmFlag)
  {
    updateCoilPwmFlag = 0;
  }
  
  
  
  
  
  #if MYDEBUG_ENABLE
  static uint32_t tickStamp = 0;
  if((MyTick_GetTickMs() - tickStamp) >= 10000) // 10 seconds
  {
    tickStamp = MyTick_GetTickMs();
    InfoLog("sizeof(AllParam_ts): %u, startAddr: 0x%X, endAddr: 0x%X", sizeof(AllParam_ts), EEPROM_ADDR_START, EEPROM_ADDR_START + sizeof(AllParam_ts) - 1); //Do not exceed 0xFBF
    InfoLog("OPERATE_FLASH_MAX_SPACE : %u", OPERATE_FLASH_MAX_SPACE);
  
  }
  #endif
}


void Param_Init(void)
{
  uint32_t codeVer = 0, eepromVer = 0;
  uint8_t calSum = 0, eepromSum = 0;
  
  codeVer = (uint16_t)(MY_MAIN_VERSION);
  codeVer <<= 16;
  codeVer |= ((uint16_t)(DEBUG_VER));
  
  
  Chip_EEPROM_Read(EEPROM_ADDR_VERSION, (uint8_t *)(&eepromVer), 4);
  Chip_EEPROM_Read(EEPROM_ADDR_CHECKSUM, &eepromSum, 1);
  Chip_EEPROM_Read(EEPROM_ADDR_PARAM, (uint8_t *)(&xgAllParam), sizeof(xgAllParam));
  calSum = CalculateChecksum((uint8_t *)(&xgAllParam), sizeof(xgAllParam));
  
  if(eepromVer != codeVer || (eepromSum != calSum))
  {
    ResetDefaultConfig();
    
    calSum = CalculateChecksum((uint8_t *)(&xgAllParam), sizeof(xgAllParam));
    Chip_EEPROM_Write(EEPROM_ADDR_PARAM, (uint8_t *)(&xgAllParam), sizeof(xgAllParam));
    Chip_EEPROM_Write(EEPROM_ADDR_CHECKSUM, &calSum, 1);
    Chip_EEPROM_Write(EEPROM_ADDR_VERSION, (uint8_t *)(&codeVer), 4);
  }
  
  Init_CfgParamToRun();

}



uint8_t ConvertVKeyToArrayIndex(uint8_t virtualKey)
{
  uint8_t ret = 0;
  
  switch(virtualKey)
  {
    case VK_LBUTTON:
      ret = 0x00;
      break;
    case VK_RBUTTON:
      ret = 0x01;
      break;
    default:
      break;
  }
  return ret;
}

#if 0
static int8_t Protocol_Send(Packet_ts *p)
{
  uint16_t sentCount = 0;
  uint16_t remainSendLen = sizeof(PacketHeader_ts) + Protocol_ConvertWord(p->xPacketHeader.wPayLoadLength);
  
  do{
    while(!MYPROTOCOL_USB_EDP_IS_IDLE());
    
    if(remainSendLen <= MYPROTOCOL_USB_EDP_TRANSFER_LEN)
    {
      MYPROTOCOL_USB_SEND_DATA((uint8_t *)(p + sentCount), remainSendLen);
      return 0;
    }
    else
    {
      MYPROTOCOL_USB_SEND_DATA((uint8_t *)(p + sentCount), MYPROTOCOL_USB_EDP_TRANSFER_LEN);
      remainSendLen -= MYPROTOCOL_USB_EDP_TRANSFER_LEN;
      sentCount += MYPROTOCOL_USB_EDP_TRANSFER_LEN;
    }
  }while(remainSendLen);
  
  return 0;
}
#else
static int8_t Protocol_Send(Packet_ts *p)
{
  uint8_t tmpBuff[MYPROTOCOL_USB_EDP_TRANSFER_LEN] = {0};
  
  uint16_t sentCount = 0;
  uint16_t remainSendLen = sizeof(PacketHeader_ts) + Protocol_ConvertWord(p->xPacketHeader.wPayLoadLength);
  
  memset(tmpBuff, 0, MYPROTOCOL_USB_EDP_TRANSFER_LEN);
  do{
    while(!MYPROTOCOL_USB_EDP_IS_IDLE());
    
    if(remainSendLen <= MYPROTOCOL_USB_EDP_TRANSFER_LEN)
    {
      memcpy(tmpBuff, p + sentCount, remainSendLen);
      
      for(uint8_t i = 0; i < (MYPROTOCOL_USB_EDP_TRANSFER_LEN - remainSendLen); i++)
      {
        tmpBuff[remainSendLen + i] = 0;
      }
      MYPROTOCOL_USB_SEND_DATA(tmpBuff, MYPROTOCOL_USB_EDP_TRANSFER_LEN);
      return 0;
    }
    else
    {
      MYPROTOCOL_USB_SEND_DATA((uint8_t *)(p + sentCount), MYPROTOCOL_USB_EDP_TRANSFER_LEN);
      remainSendLen -= MYPROTOCOL_USB_EDP_TRANSFER_LEN;
      sentCount += MYPROTOCOL_USB_EDP_TRANSFER_LEN;
    }
  }while(remainSendLen);
  
  return 0;
}
#endif



static int8_t ProcessRequest_CMD_GET_INFO(uint8_t subCommand, uint8_t virtualKeyCode)
{
  Packet_ts *pxPack = 0;
  PacketHeader_ts *pxPackHeader = 0;
  PayloadHeader_ts *pxPayloadHeader = 0;
  
  if(virtualKeyCode != VK_LBUTTON && virtualKeyCode != VK_RBUTTON) {return 0;}
  switch(subCommand)
  {
    case INFO_HALL_RAW:
      {
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + sizeof(HallRawInfo_ts)] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + sizeof(HallRawInfo_ts));
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_GET_INFO;
        pxPayloadHeader->bSubCommand = INFO_HALL_RAW;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        HallRawInfo_ts *pxHallInfo = &((pxgAllInfo + ConvertVKeyToArrayIndex(virtualKeyCode))->xRawInfo);
        memcpy(pxPayloadHeader + 1, pxHallInfo, sizeof(HallRawInfo_ts));
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    case INFO_HALL_SEGMENT:
      {
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + sizeof(HallSegmentInfo_ts)] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + sizeof(HallSegmentInfo_ts));
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_GET_INFO;
        pxPayloadHeader->bSubCommand = INFO_HALL_SEGMENT;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        HallSegmentInfo_ts *pxHallSegmentInfo = &((pxgAllInfo + ConvertVKeyToArrayIndex(virtualKeyCode))->xSegmentInfo);
        memcpy(pxPayloadHeader + 1, pxHallSegmentInfo, sizeof(HallSegmentInfo_ts));
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    case INFO_COIL:
      {
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + sizeof(CoilInfo_ts)] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + sizeof(CoilInfo_ts));
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_GET_INFO;
        pxPayloadHeader->bSubCommand = INFO_COIL;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        CoilInfo_ts *pxCoilInfo = &((pxgAllInfo + ConvertVKeyToArrayIndex(virtualKeyCode))->xCoilInfo);
        memcpy(pxPayloadHeader + 1, pxCoilInfo, sizeof(CoilInfo_ts));
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    default:
      break;
  }
  return 0;
}

static int8_t ProcessRequest_CMD_GET_CONFIG(uint8_t subCommand, uint8_t virtualKeyCode, \
                                            uint8_t *pData, uint16_t wDataLen)
{
  Packet_ts *pxPack = 0;
  PacketHeader_ts *pxPackHeader = 0;
  PayloadHeader_ts *pxPayloadHeader = 0;
  //allow pData and wDataLen is 0.
  //if(virtualKeyCode != VK_LBUTTON && virtualKeyCode != VK_RBUTTON) {return 0;}
  switch(subCommand)
  {
    case CFG_RAPID_TRIGGER:
      {
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + sizeof(RapidTriggerConfig_ts)] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + sizeof(RapidTriggerConfig_ts));
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_GET_CONFIG;
        pxPayloadHeader->bSubCommand = CFG_RAPID_TRIGGER;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        RapidTriggerConfig_ts *pxRapidTriggerCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(virtualKeyCode))->xRtCfg);
        memcpy(pxPayloadHeader + 1, pxRapidTriggerCfg, sizeof(RapidTriggerConfig_ts));
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    case CFG_COIL:
      {
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + sizeof(CoilConfig_ts)] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + sizeof(CoilConfig_ts));
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_GET_CONFIG;
        pxPayloadHeader->bSubCommand = CFG_COIL;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        CoilConfig_ts *pxCoilCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(virtualKeyCode))->xCoilCfg);
        memcpy(pxPayloadHeader + 1, pxCoilCfg, sizeof(CoilConfig_ts));
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    case CFG_COIL_RELEASE:
      {
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + sizeof(CoilReleaseConfig_ts)] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + sizeof(CoilReleaseConfig_ts));
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_GET_CONFIG;
        pxPayloadHeader->bSubCommand = CFG_COIL_RELEASE;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        CoilReleaseConfig_ts *pxCoilReleaseCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(virtualKeyCode))->xCoilReleaseCfg);
        memcpy(pxPayloadHeader + 1, pxCoilReleaseCfg, sizeof(CoilReleaseConfig_ts));
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    case CFG_COIL_BRAKE:
      {
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + sizeof(CoilBrakeConfig_ts)] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + sizeof(CoilBrakeConfig_ts));
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_GET_CONFIG;
        pxPayloadHeader->bSubCommand = CFG_COIL_BRAKE;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        CoilBrakeConfig_ts *pxCoilBrakeCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(virtualKeyCode))->xCoilBrakeCfg);
        memcpy(pxPayloadHeader + 1, pxCoilBrakeCfg, sizeof(CoilBrakeConfig_ts));
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    case CFG_COIL_BRAKE_REL:
      {
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + sizeof(CoilBrakeRelConfig_ts)] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + sizeof(CoilBrakeRelConfig_ts));
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_GET_CONFIG;
        pxPayloadHeader->bSubCommand = CFG_COIL_BRAKE_REL;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        CoilBrakeRelConfig_ts *pxCoilBrakeRelCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(virtualKeyCode))->xCoilBrakeRelCfg);
        memcpy(pxPayloadHeader + 1, pxCoilBrakeRelCfg, sizeof(CoilBrakeRelConfig_ts));
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    case CFG_COIL_DRIVE_MODE:
      {
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + sizeof(CoilDriveModeConfig_ts)] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + sizeof(CoilDriveModeConfig_ts));
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_GET_CONFIG;
        pxPayloadHeader->bSubCommand = CFG_COIL_DRIVE_MODE;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        CoilDriveModeConfig_ts *pxCoilDriveModeCfg = &(xgAllParam.xDriveMode);
        memcpy(pxPayloadHeader + 1, pxCoilDriveModeCfg, sizeof(CoilDriveModeConfig_ts));
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    case CFG_COIL_PWM:
      {
        if(0 == wDataLen || (*(pData) > (100 - 1)))
        {
          return 0;
        }
        
        uint8_t dataLen =  (100 - *(pData)) > 50 ? 50 : (100 - *(pData));
        
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + 50] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + 1 + dataLen);
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_GET_CONFIG;
        pxPayloadHeader->bSubCommand = CFG_COIL_PWM;
        pxPayloadHeader->bValue = virtualKeyCode;
        uint8_t *pTmp = (uint8_t *)(&(pxPayloadHeader->bValue));
        *(pTmp + 1) =  *pData;
        
        memcpy(pTmp + 2, &(xgAllParam.gPwmWf[*(pData)]), dataLen);
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    default:
      break;
  }
  return 0;
}

static int8_t ProcessRequest_CMD_SET_CONFIG(uint8_t subCommand, uint8_t virtualKeyCode, \
                                uint8_t *pData, uint16_t wDataLen)
{
  uint8_t statusResponse = STATUS_FAIL;
  Packet_ts *pxPack = 0;
  PacketHeader_ts *pxPackHeader = 0;
  PayloadHeader_ts *pxPayloadHeader = 0;
  
  //if(virtualKeyCode != VK_LBUTTON && virtualKeyCode != VK_RBUTTON) {return 0;}
  
  if(!pData || !wDataLen)
  {
    return -1;
  }
  
  switch(subCommand)
  {
    case CFG_RAPID_TRIGGER:
      {
        if(wDataLen >= sizeof(RapidTriggerConfig_ts))
        {
          RapidTriggerConfig_ts *pxRapidTriggerCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(virtualKeyCode))->xRtCfg);
          memcpy(pxRapidTriggerCfg, pData, sizeof(RapidTriggerConfig_ts));
          statusResponse = STATUS_SUCCESS;
          updateRtCfgFlag = 1;
        }
        else
        {
          statusResponse = STATUS_FAIL;
        }
        
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + 1] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + 1);
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_SET_CONFIG;
        pxPayloadHeader->bSubCommand = CFG_RAPID_TRIGGER;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        *(uint8_t*)(pxPayloadHeader + 1) = statusResponse;
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    case CFG_COIL:
      {
        if(wDataLen >= sizeof(CoilConfig_ts))
        {
          CoilConfig_ts *pxCoilCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(virtualKeyCode))->xCoilCfg);
          memcpy(pxCoilCfg, pData, sizeof(CoilConfig_ts));
          statusResponse = STATUS_SUCCESS;
          if(virtualKeyCode == VK_LBUTTON) updateCoilCfgFlagL = 1;
          if(virtualKeyCode == VK_RBUTTON) updateCoilCfgFlagR = 1;
        }
        else
        {
          statusResponse = STATUS_FAIL;
        }
        
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + 1] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + 1);
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_SET_CONFIG;
        pxPayloadHeader->bSubCommand = CFG_COIL;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        *(uint8_t*)(pxPayloadHeader + 1) = statusResponse;
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    case CFG_COIL_RELEASE:
      {
        if(wDataLen >= sizeof(CoilReleaseConfig_ts))
        {
          CoilReleaseConfig_ts *pxCoilReleaseCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(virtualKeyCode))->xCoilReleaseCfg);
          memcpy(pxCoilReleaseCfg, pData, sizeof(CoilReleaseConfig_ts));
          statusResponse = STATUS_SUCCESS;
          if(virtualKeyCode == VK_LBUTTON) updateCoilReleaseCfgFlagL = 1;
          if(virtualKeyCode == VK_RBUTTON) updateCoilReleaseCfgFlagR = 1;
        }
        else
        {
          statusResponse = STATUS_FAIL;
        }
        
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + 1] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + 1);
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_SET_CONFIG;
        pxPayloadHeader->bSubCommand = CFG_COIL_RELEASE;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        *(uint8_t*)(pxPayloadHeader + 1) = statusResponse;
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    case CFG_COIL_BRAKE:
      {
        if(wDataLen >= sizeof(CoilBrakeConfig_ts))
        {
          CoilBrakeConfig_ts *pxCoilBrakeCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(virtualKeyCode))->xCoilBrakeCfg);
          memcpy(pxCoilBrakeCfg, pData, sizeof(CoilBrakeConfig_ts));
          statusResponse = STATUS_SUCCESS;
          if(virtualKeyCode == VK_LBUTTON) updateCoilBrakeCfgFlagL = 1;
          if(virtualKeyCode == VK_RBUTTON) updateCoilBrakeCfgFlagR = 1;
        }
        else
        {
          statusResponse = STATUS_FAIL;
        }
        
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + 1] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + 1);
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_SET_CONFIG;
        pxPayloadHeader->bSubCommand = CFG_COIL_BRAKE;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        *(uint8_t*)(pxPayloadHeader + 1) = statusResponse;
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    case CFG_COIL_BRAKE_REL:
      {
        if(wDataLen >= sizeof(CoilBrakeRelConfig_ts))
        {
          CoilBrakeRelConfig_ts *pxCoilBrakeRelCfg = &((pxgAllConfig + ConvertVKeyToArrayIndex(virtualKeyCode))->xCoilBrakeRelCfg);
          memcpy(pxCoilBrakeRelCfg, pData, sizeof(CoilBrakeRelConfig_ts));
          statusResponse = STATUS_SUCCESS;
          if(virtualKeyCode == VK_LBUTTON) updateCoilBrakeRelCfgFlagL = 1;
          if(virtualKeyCode == VK_RBUTTON) updateCoilBrakeRelCfgFlagR = 1;
        }
        else
        {
          statusResponse = STATUS_FAIL;
        }
        
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + 1] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + 1);
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_SET_CONFIG;
        pxPayloadHeader->bSubCommand = CFG_COIL_BRAKE_REL;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        *(uint8_t*)(pxPayloadHeader + 1) = statusResponse;
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    case CFG_COIL_DRIVE_MODE:
      {
        if(wDataLen >= sizeof(CoilDriveModeConfig_ts))
        {
          CoilDriveModeConfig_ts *pxCoilDriveModeCfg = &(xgAllParam.xDriveMode);
          memcpy(pxCoilDriveModeCfg, pData, sizeof(CoilDriveModeConfig_ts));
          statusResponse = STATUS_SUCCESS;
          updateCoilDriveModeFlag = 1;
          //if(virtualKeyCode == VK_LBUTTON) updateCoilBrakeRelCfgFlagL = 1;
          //if(virtualKeyCode == VK_RBUTTON) updateCoilBrakeRelCfgFlagR = 1;
        }
        else
        {
          statusResponse = STATUS_FAIL;
        }
        
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + 1] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + 1);
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_SET_CONFIG;
        pxPayloadHeader->bSubCommand = CFG_COIL_DRIVE_MODE;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        *(uint8_t*)(pxPayloadHeader + 1) = statusResponse;
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    case CFG_COIL_PWM:
      {
        uint8_t index = *(pData);
        uint8_t validLen = wDataLen - 1;
        uint8_t *pValid = pData + 1;
        if((wDataLen >= 2) && (validLen <= 50) && (index < 100) && ((index + validLen) <= 100))
        {
          memcpy(&(xgAllParam.gPwmWf[index]), pValid, validLen);
          statusResponse = STATUS_SUCCESS;
          updateCoilPwmFlag = 1;
        }
        else
        {
          statusResponse = STATUS_FAIL;
        }
        
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + 1 + 1] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + 1 + 1);
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_SET_CONFIG;
        pxPayloadHeader->bSubCommand = CFG_COIL_PWM;
        pxPayloadHeader->bValue = virtualKeyCode;
        uint8_t *pTmp = (uint8_t *)(&(pxPayloadHeader->bValue));
        *(pTmp + 1) =  *pData;
        
        *(pTmp + 2) = statusResponse;
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    default:
      break;
  }
  return 0;
}

static int8_t ProcessRequest_CMD_GET_DEBUG(uint8_t subCommand, uint8_t virtualKeyCode)
{
  Packet_ts *pxPack = 0;
  PacketHeader_ts *pxPackHeader = 0;
  PayloadHeader_ts *pxPayloadHeader = 0;
  
  if(virtualKeyCode != VK_LBUTTON && virtualKeyCode != VK_RBUTTON) {return 0;}
  switch(subCommand)
  {
    case DEBUG_COIL:
      {
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + sizeof(CoilDebug_ts)] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + sizeof(CoilDebug_ts));
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_GET_DEBUG;
        pxPayloadHeader->bSubCommand = DEBUG_COIL;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        CoilDebug_ts xCoilDebug;
        Ele_GetDebugParam(virtualKeyCode, &xCoilDebug);
        memcpy(pxPayloadHeader + 1, &xCoilDebug, sizeof(CoilDebug_ts));
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    default:
      break;
  }
  return 0;
}

static int8_t ProcessRequest_CMD_SET_DEBUG(uint8_t subCommand, uint8_t virtualKeyCode, \
                                uint8_t *pData, uint16_t wDataLen)
{
  uint8_t statusResponse = STATUS_FAIL;
  Packet_ts *pxPack = 0;
  PacketHeader_ts *pxPackHeader = 0;
  PayloadHeader_ts *pxPayloadHeader = 0;
  
  if(virtualKeyCode != VK_LBUTTON && virtualKeyCode != VK_RBUTTON) {return 0;}
  
  if(!pData || !wDataLen)
  {
    return -1;
  }
  
  switch(subCommand)
  {
    case DEBUG_COIL:
      {
        if(wDataLen >= sizeof(CoilDebug_ts))
        {
          CoilDebug_ts xCoilDebug;
          memcpy(&xCoilDebug, pData, sizeof(CoilDebug_ts));
          Ele_UpdateDebugParam(virtualKeyCode, &xCoilDebug);
          statusResponse = STATUS_SUCCESS;
        }
        else
        {
          statusResponse = STATUS_FAIL;
        }
        
        uint8_t tmpBuff[sizeof(PacketHeader_ts) + sizeof(PayloadHeader_ts) + 1] = {0};
        pxPack =  (Packet_ts *)tmpBuff;
        pxPackHeader = &(pxPack->xPacketHeader);
        pxPayloadHeader = &(pxPack->xPayload.xPayloadHeader);
        
        pxPackHeader->bPreamble = PACKET_PREAMBLER;
        pxPackHeader->bDestination = ID_HOST;      // Destination(receiver).
        pxPackHeader->bSource = ID_MOUSE;          // Source(Sender).
        //pxPackHeader->bChecksum = 0;
        pxPackHeader->wPayLoadLength = Protocol_ConvertWord(sizeof(PayloadHeader_ts) + 1);
        
        pxPayloadHeader->bPayloadType = PAYLOADTYPE_RESPONSE;
        pxPayloadHeader->bCommand = CMD_SET_DEBUG;
        pxPayloadHeader->bSubCommand = DEBUG_COIL;
        pxPayloadHeader->bValue = virtualKeyCode;
        
        *(uint8_t*)(pxPayloadHeader + 1) = statusResponse;
        pxPackHeader->bChecksum = Protocol_CalculateChecksum(pxPack);
        Protocol_Send(pxPack);
      }
      break;
    default:
      break;
  }
  return 0;
}

static int8_t Protocol_ProcessReceivePayload(Payload_ts *pxPayload, uint16_t payloadLen)
{
  PayloadHeader_ts *pxPayloadHeader = 0;
  
  if(pxPayload == 0 || payloadLen < sizeof(PayloadHeader_ts))
  {
    return -1;
  }
  
  pxPayloadHeader = &(pxPayload->xPayloadHeader);
  
  switch(pxPayloadHeader->bPayloadType)
  {
      case PAYLOADTYPE_NOTIFICATION:
          ;
          break;
      case PAYLOADTYPE_REQUEST:
          switch(pxPayloadHeader->bCommand)
          {
              case CMD_GET_INFO:
                  ProcessRequest_CMD_GET_INFO(pxPayloadHeader->bSubCommand, pxPayloadHeader->bValue);
                  break;
              case CMD_GET_CONFIG:
                  ProcessRequest_CMD_GET_CONFIG(pxPayloadHeader->bSubCommand, pxPayloadHeader->bValue, \
                                            (uint8_t*)(pxPayloadHeader + 1), payloadLen - sizeof(PayloadHeader_ts));
                  break;
              case CMD_SET_CONFIG:
                  ProcessRequest_CMD_SET_CONFIG(pxPayloadHeader->bSubCommand, \
                                pxPayloadHeader->bValue, (uint8_t*)(pxPayloadHeader + 1), \
                                payloadLen - sizeof(PayloadHeader_ts));
                  break;
              case CMD_GET_DEBUG:
                  ProcessRequest_CMD_GET_DEBUG(pxPayloadHeader->bSubCommand, pxPayloadHeader->bValue);
                  break;
              case CMD_SET_DEBUG:
                  ProcessRequest_CMD_SET_DEBUG(pxPayloadHeader->bSubCommand, \
                                pxPayloadHeader->bValue, (uint8_t*)(pxPayloadHeader + 1), \
                                payloadLen - sizeof(PayloadHeader_ts));
                  break;
              default:
                  break;
          }
          break;
      case PAYLOADTYPE_RESPONSE:
          ;
          break;
      default:
          break;
  }
  
  return 0;
}


static int8_t Protocol_ProcessReceivePacket(Packet_ts *pxPacket)
{
  uint16_t payloadLen = 0;
  
  if(!pxPacket)
  {
      return -1;
  }
  
  payloadLen = Protocol_ConvertWord(pxPacket->xPacketHeader.wPayLoadLength);
  
  if(payloadLen)
  { // Not Empty Payload.
    Protocol_ProcessReceivePayload(&(pxPacket->xPayload), payloadLen);
  }
  
  return 0;
}


int8_t Protocol_ProcessReceiveData(uint8_t *pData, uint16_t length)
{
    uint8_t *p = pData;
    uint16_t remainLen = length;
    uint16_t payloadLen = 0;
    
    if(!pData || length < sizeof(PacketHeader_ts))
    {
        return -1;
    }
    while(remainLen >= sizeof(PacketHeader_ts))
    {
        if(*p == PACKET_PREAMBLER)
        {
            if(*(p + 1) == ID_MOUSE)
            {
                payloadLen = *(p + 5);
                payloadLen <<= 8;
                payloadLen |= *(p + 4);
                
                if((remainLen - sizeof(PacketHeader_ts)) >= payloadLen)
                {
                    if(*(p + 3) == Protocol_CalculateChecksum((Packet_ts *)p))
                    {
                        Protocol_ProcessReceivePacket((Packet_ts *)p);
                        
                        remainLen -= (payloadLen + sizeof(PacketHeader_ts));
                        p += (payloadLen + sizeof(PacketHeader_ts));
                        continue;
                    }
                }
            }
        }
        remainLen -= 1;
        p += 1;
    }
    
    return 0;
}


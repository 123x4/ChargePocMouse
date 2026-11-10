/*******************************************************************************
 * @file    my_protocol_mouse.h
 * @brief
 * @author  chengjie
 * @date    created on 2026-04-08
 * @version V1.x
 * @note
*******************************************************************************/
#ifndef __my_protocol_mouse_h__
#define __my_protocol_mouse_h__

#include "my_protocol_common.h"
#include "string.h"
#include "usbhw.h"
#include "my_debug.h"


#define MYPROTOCOL_USB_EDP_NUM                 EP_3_CUSTION_OUT
#define MYPROTOCOL_USB_EDP_TRANSFER_LEN        TRANSFER_LENGTH
#define MYPROTOCOL_USB_EDP_IS_IDLE()   nsAPI_USB_EP_Status(MYPROTOCOL_USB_EDP_NUM)
#define MYPROTOCOL_USB_SEND_DATA(pdata, len) \
                              nsAPI_USB_WriteEP(MYPROTOCOL_USB_EDP_NUM, pdata, len)


//extern AllInfo_ts *pxgAllInfo;
//extern AllConfig_ts *pxgAllConfig;

/**************Function Declaration********************************************/
uint8_t* Config_GetPwmTable(void);
uint8_t Config_GetCoilDriveMode(void);

void Hall_SyncParam(uint8_t index, HallRawInfo_ts *pxRawInfo, HallSegmentInfo_ts *pxSegInfo);
void Task_ParamGuard(void);
void Param_Init(void);
uint8_t ConvertVKeyToArrayIndex(uint8_t virtualKey);
int8_t Protocol_ProcessReceiveData(uint8_t *pData, uint16_t length);

#endif /* __my_protocol_mouse_h__ */



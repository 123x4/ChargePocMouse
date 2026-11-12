/*
 * pc_communication.h
 *
 *  Created on: 2023年7月30日
 *  Author: Hao.Luo
 */

#ifndef TESTAPP_COMMUNICATION_H_
#define TESTAPP_COMMUNICATION_H_

#include "usb.h"
/*******************************************************************************
 * define
 ******************************************************************************/
/************************************************************************************************/
/* Test APP Protocol */
#define TestAPP_Byte0_TriggerID 		(0x00)
#define TestAPP_Byte0_ID 				(0x04)
//------------------------------------------------------------------------------------------------
#define TestAPP_CRCErr_ID 				(0x06)
//------------------------------------------------------------------------------------------------
#define TestAPP_Byte1_EnterTest 		(0x04)
#define TestAPP_Byte1_EnterNormal		(0x05)
#define TestAPP_Byte1_ReportType 		(0x17)
#define TestAPP_Byte1_SETTrigger 		(0x40)
#define TestAPP_Byte1_GETTrigger 		(0x41)
//------------------------------------------------------------------------------------------------
#define TestAPP_Byte10_RawData 			(0x00)
#define TestAPP_Byte10_Segment 			(0x01)
#define TestAPP_Byte10_SegmentAnalove 	(0x03)
#define TestAPP_Byte10_Gain 			(0x05)
#define TestAPP_Byte10_BaselineTop 		(0x0d)
#define TestAPP_Byte10_BaselineBottom 	(0x0e)
#define TestAPP_Byte10_Checkself 		(0x0c)
//------------------------------------------------------------------------------------------------
#define Test_EnterISP			 		(0xFE)
//------------------------------------------------------------------------------------------------
/* 因为协议是一个按键带三个byte的数据, 5528一条链最多有21个按键,一个包放不下,所以定下每一个包最多放20个按键,超过的另加一个包发. */
#define TestAPP_OnePacket_LEN 			(0x14)
#define TestAPP_TotalPacket_NUM  	 	5
/************************************************************************************************/
extern uint8_t Flag_Report_Trigger;
extern uint8_t Flag_SET_Trigger;
/*******************************************************************************
 * FUNCTION
 ******************************************************************************/
extern usb_status_t USB_DeviceHidCustomReportOutCallBack(void);
extern void TestAPP_COMMUNICATION(void);
extern void Report_RawData_Baseline(void);
extern void Report_SegmentData(void);
extern void Report_Triggerdata(void);
extern void Set_Triggerdata(void);
extern void Report_SelfCheck(void);
extern void Clear_buffer(uint8_t *buffer, uint8_t len);
extern void InitParameter_FOR_TestMode(void);
extern void TestAPP_CheckReportType(void);

#endif /* PC_COMMUNICATION_H */

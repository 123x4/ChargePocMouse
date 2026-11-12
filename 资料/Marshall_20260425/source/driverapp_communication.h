/*
 * pc_communication.h
 *
 *  Created on: 2023年7月30日
 *  Author: Hao.Luo
 */

#ifndef DRIVERAPP_COMMUNICATION_H_
#define DRIVERAPP_COMMUNICATION_H_

/**************************************************************************************************/
/* VGN的驱动协议 */
/* Driver APP Protocol */
#define DriverAPP_Byte0_CMD_GET_Info 					(0x12)
#define DriverAPP_Byte0_CMD_GET_Online 					(0x13)
#define DriverAPP_Byte0_CMD_SET_NKeyRollover 			(0x14)
#define DriverAPP_Byte0_CMD_GET_Keycode 				(0x15)
#define DriverAPP_Byte0_CMD_SET_Keycode 				(0x16)
#define DriverAPP_Byte0_CMD_GET_ALLKeycode 				(0x17)
#define DriverAPP_Byte0_CMD_SET_ALLKeycode 				(0x18)
#define DriverAPP_Byte0_CMD_GET_WASD 					(0x19)
#define DriverAPP_Byte0_CMD_SET_WASD 					(0x1A)
#define DriverAPP_Byte0_CMD_GET_Mac 					(0x1B)
#define DriverAPP_Byte0_CMD_SET_Mac 					(0x1C)
#define DriverAPP_Byte0_CMD_GET_WinLock 				(0x1D)
#define DriverAPP_Byte0_CMD_SET_WinLock 				(0x1E)
#define DriverAPP_Byte0_CMD_SaveFlash 					(0x21)
#define DriverAPP_Byte0_CMD_Reset 						(0x22)
#define DriverAPP_Byte0_CMD_GET_Polling 				(0x23)
#define DriverAPP_Byte0_CMD_SET_Polling 				(0x24)
#define DriverAPP_Byte0_CMD_GET_Debounce 				(0x25)
#define DriverAPP_Byte0_CMD_SET_Debounce 				(0x26)
#define DriverAPP_Byte0_CMD_GET_LowLatency 				(0x27)
#define DriverAPP_Byte0_CMD_SET_LowLatency 				(0x28)
#define DriverAPP_Byte0_CMD_GET_BottomTriggerState 		(0x29)
#define DriverAPP_Byte0_CMD_SET_BottomTriggerState 		(0x2A)
#define DriverAPP_Byte0_CMD_GET_ProfileIndex 			(0x2B)
#define DriverAPP_Byte0_CMD_SET_ProfileIndex 			(0x2C)
#define DriverAPP_Byte0_CMD_GET_Battery 				(0x2D)
#define DriverAPP_Byte0_CMD_GET_SingleTrigger 			(0x40)
#define DriverAPP_Byte0_CMD_SET_SingleTrigger 			(0x41)
#define DriverAPP_Byte0_CMD_SET_ALLTrigger 				(0x42)
#define DriverAPP_Byte0_CMD_GET_ALLTrigger 				(0x43)
#define DriverAPP_Byte0_CMD_SET_LEDData 				(0x44)
#define DriverAPP_Byte0_CMD_GET_LEDData 				(0x45)
#define DriverAPP_Byte0_CMD_SET_Macro 					(0x46)
#define DriverAPP_Byte0_CMD_GET_Macro 					(0x47)
#define DriverAPP_Byte0_CMD_GET_CustomModeLEDData 		(0x49)
#define DriverAPP_Byte0_CMD_SET_CustomModeLEDData 		(0x4A)
#define DriverAPP_Byte0_CMD_GET_AllKeyTrigger 			(0x4B)
#define DriverAPP_Byte0_CMD_SET_AllKeyTrigger 			(0x5C)
#define DriverAPP_Byte0_CMD_GET_SingleTriggerStatus 	(0x54)
#define DriverAPP_Byte0_CMD_SET_SingleTriggerStatus 	(0x55)
#define DriverAPP_Byte0_CMD_GET_ContinueTriggerMode 	(0x4C)
#define DriverAPP_Byte0_CMD_SET_ContinueTriggerMode 	(0x4D)
#define DriverAPP_Byte0_CMD_GET_DKS 					(0x4E)
#define DriverAPP_Byte0_CMD_SET_DKS 					(0x4F)
#define DriverAPP_Byte0_CMD_GET_MT 						(0x50)
#define DriverAPP_Byte0_CMD_SET_MT 						(0x51)
#define DriverAPP_Byte0_CMD_GET_TGL 					(0x52)
#define DriverAPP_Byte0_CMD_SET_TGL 					(0x53)
#define DriverAPP_Byte0_CMD_GET_ALLRapidTrigger 		(0x56)
#define DriverAPP_Byte0_CMD_SET_ALLRapidTrigger 		(0x57)
#define DriverAPP_Byte0_CMD_GET_ALLContinueRapidTrigger (0x5A)
#define DriverAPP_Byte0_CMD_SET_ALLContinueRapidTrigger (0x5B)
#define DriverAPP_Byte0_CMD_GET_FULLRapidTrigger 		(0x5D)
#define DriverAPP_Byte0_CMD_SET_FULLRapidTrigger 		(0x5E)

#define DriverAPP_Byte0_CMD_Update 						(0xFE)

#define DriverAPP_Byte2_CMD_Success 					(0x00)
#define DriverAPP_Byte2_CMD_Fail 						(0xFF)

#define DriverAPP_Byte3_Online							(0x01)
#define DriverAPP_Byte3_Offline							(0x00)

#define DriverAPP_Byte7_ReportRate_1KHZ					(0x01)
#define DriverAPP_Byte7_ReportRate_500HZ			(0x02)
#define DriverAPP_Byte7_ReportRate_250HZ			(0x03)
#define DriverAPP_Byte7_ReportRate_125HZ			(0x04)
#define DriverAPP_Byte7_ReportRate_4KHZ				(0x05)
#define DriverAPP_Byte7_ReportRate_8KHZ				(0x06)
//--------------------------------------------------------------------------------------------------
/* VGN的驱动相关参数 */
extern uint8_t DriverAPP_ReceiveData;
extern uint8_t DriverAPP_ReportData_Type;
extern uint8_t DriverAPP_WASD_Switch;
extern uint8_t DriverAPP_Nkey_Rollover_Switch;
extern uint8_t DriverAPP_WinLock_Switch;			//00:未锁定，01:锁定
extern uint8_t DriverAPP_ButtonDelay;
extern uint8_t DriverAPP_WinMacMode;
extern uint8_t DriverAPP_LowLatencyMode;
extern uint8_t DriverAPP_BottomTriggerState;
extern uint8_t DriverAPP_ProfileIndex;
extern uint8_t Tick_DriverAPP_CNT;
//--------------------------------------------------------------------------------------------------
extern void DriverAPP_Process(void);
extern void DriverAPP_ReportInfo(void);
extern void DriverAPP_ReportOnline(void);
extern void DriverAPP_ReportNKeyRollover(void);
extern void DriverAPP_GetKeycode(void);
extern void DriverAPP_SetKeycode(void);
extern void DriverAPP_GetAllKeycode(void);
extern void DriverAPP_SetAllKeycode(void);
extern void DriverAPP_GetWASD(void);
extern void DriverAPP_SetWASD(void);
extern void DriverAPP_GetMAC(void);
extern void DriverAPP_SetMac(void);
extern void DriverAPP_GetWinlock(void);
extern void DriverAPP_SetWinlock(void);
extern void DriverAPP_SaveFlash(void);
extern void DriverAPP_Restore(void);
extern void DriverAPP_GetPolling(void);
extern void DriverAPP_SetPolling(void);
extern void DriverAPP_GetDebounce(void);
extern void DriverAPP_SetDebounce(void);
extern void DriverAPP_GetLowLatency(void);
extern void DriverAPP_SetLowLatency(void);
extern void DriverAPP_GetBottomTriggerState(void);
extern void DriverAPP_SetBottomTriggerState(void);
extern void DriverAPP_SaveFlash(void);
extern void DriverAPP_GetProfileIndex(void);
extern void DriverAPP_SetProfileIndex(void);
extern void DriverAPP_GetBattery(void);
extern void DriverAPP_GetSingleTrigger(void);
extern void DriverAPP_SetSingleTrigger(void);
extern void DriverAPP_SetALLTrigger(void);
extern void DriverAPP_GetALLTrigger(void);
extern void DriverAPP_SetLEDData(void);
extern void DriverAPP_GetLEDData(void);
extern void DriverAPP_GetTriggerStatus(void);
extern void DriverAPP_SetTriggerStatus(void);
extern void DriverAPP_SetTriggerStatus(void);
extern void DriverAPP_GetAllKeyTrigger(void);
extern void DriverAPP_SetAllKeyTrigger(void);
extern void DriverAPP_GetContinueTrigger(void);
extern void DriverAPP_SetContinueTrigger(void);
extern void DriverAPP_GetDKS(void);
extern void DriverAPP_SetDKS(void);
extern void DriverAPP_GetMT(void);
extern void DriverAPP_SetMT(void);
extern void DriverAPP_GetTGL(void);
extern void DriverAPP_SetTGL(void);
extern void DriverAPP_GetCustomModeLEDData(void);
extern void DriverAPP_SetCustomModeLEDData(void);
extern void DriverAPP_GetMacro(void);
extern void DriverAPP_SetMacro(void);
extern void DriverAPP_GETALLRapidTrigger(void);
extern void DriverAPP_SETALLRapidTrigger(void);
extern void DriverAPP_GETALLContinueRapidTrigger(void);
extern void DriverAPP_SETALLContinueRapidTrigger(void);
extern void DriverAPP_GETFULLRapidTrigger(void);
extern void DriverAPP_SETFULLRapidTrigger(void);
/**************************************************************************************************/
#endif /* DRIVERAPP_COMMUNICATION_H */

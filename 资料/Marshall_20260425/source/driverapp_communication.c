/*
 * driverapp_communication.c
 */
#include <usb.h>
#include <composite.h>
#include <hid_keyboard.h>
#include <fsl_debug_console.h>
#include <board.h>
#include <usb_device_descriptor.h>
#include <testapp_communication.h>
#include <driverapp_communication.h>
#include <Flash_iap.h>
#include <flash_zb25v.h>
#include <led.h>
#include <pin_mux.h>
#include <macro.h>
#include <hall_spi.h>

/*******************************************************************************
 * Driver APP相关参数
 ******************************************************************************/
uint8_t DriverAPP_ReceiveData = 0;
uint8_t DriverAPP_ReportData_Type = 0;
uint8_t DriverAPP_WASD_Switch = 0;
uint8_t DriverAPP_WinMacMode = 0;
uint8_t DriverAPP_Nkey_Rollover_Switch = 0;
uint8_t DriverAPP_WinLock_Switch = 0;			//00:未锁定，01:锁定
uint8_t DriverAPP_LowLatencyMode = 0;
uint8_t DriverAPP_ButtonDelay = 0;
uint8_t DriverAPP_BottomTriggerState = 0;
uint8_t DriverAPP_ProfileIndex = 0;
uint8_t Tick_DriverAPP_CNT = 0;
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: DriverAPP_Process
 * Description  	: 处理驱动的数据，包括上报或者是设置功能.
 *
 * END ****************************************************************************************************************/
void DriverAPP_Process(void)
{
	if(++Tick_DriverAPP_CNT > 8)
	{
		Tick_DriverAPP_CNT = 0;
		if(DriverAPP_ReceiveData)
		{
			//------------------------------------------------------------------------------------------
			switch(DriverAPP_ReportData_Type)
			{
				//----------------------------------------------------------------------------------------------------------
				/* 上报设备信息 */
				case DriverAPP_Byte0_CMD_GET_Info:
					DriverAPP_ReceiveData = 0;
					DriverAPP_ReportInfo();
					break;
				//----------------------------------------------------------------------------------------------------------
				/* 上报是否在线 */
				case DriverAPP_Byte0_CMD_GET_Online:
					DriverAPP_ReceiveData = 0;
					DriverAPP_ReportOnline();
					break;
				//----------------------------------------------------------------------------------------------------------
				/* 上报全键or六键无冲 */
				case DriverAPP_Byte0_CMD_SET_NKeyRollover:
					DriverAPP_ReceiveData = 0;
					DriverAPP_ReportNKeyRollover();
					break;
				//----------------------------------------------------------------------------------------------------------
				/* 获取单个按键键值 */
				case DriverAPP_Byte0_CMD_GET_Keycode:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetKeycode();
				break;
				/* 设置单个按键键值 */
				case DriverAPP_Byte0_CMD_SET_Keycode:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetKeycode();
				break;
				//----------------------------------------------------------------------------------------------------------、
				/* 获取全部按键键值 */
				case DriverAPP_Byte0_CMD_GET_ALLKeycode:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetAllKeycode();
				break;
				/* 设置全部按键键值 */
				case DriverAPP_Byte0_CMD_SET_ALLKeycode:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetAllKeycode();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取WASD切换 */
				case DriverAPP_Byte0_CMD_GET_WASD:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetWASD();
				break;
				/* 设置WASD切换 */
				case DriverAPP_Byte0_CMD_SET_WASD:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetWASD();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取Mac模式 */
				case DriverAPP_Byte0_CMD_GET_Mac:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetMAC();
				break;
				/* 设置Mac模式 */
				case DriverAPP_Byte0_CMD_SET_Mac:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetMac();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取Winlock模式 */
				case DriverAPP_Byte0_CMD_GET_WinLock:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetWinlock();
				break;
				/* 设置Winlock模式 */
				case DriverAPP_Byte0_CMD_SET_WinLock:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetWinlock();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 保存flash */
				case DriverAPP_Byte0_CMD_SaveFlash:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SaveFlash();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 重置键盘 */
				case DriverAPP_Byte0_CMD_Reset:
					DriverAPP_ReceiveData = 0;
					DriverAPP_Restore();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取上报率 */
				case DriverAPP_Byte0_CMD_GET_Polling:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetPolling();
				break;
				/* 设置上报率 */
				case DriverAPP_Byte0_CMD_SET_Polling:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetPolling();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取按键延时 */
				case DriverAPP_Byte0_CMD_GET_Debounce:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetDebounce();
				break;
				/* 设置按键延时 */
				case DriverAPP_Byte0_CMD_SET_Debounce:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetDebounce();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取按键低延时状态 */
				case DriverAPP_Byte0_CMD_GET_LowLatency:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetLowLatency();
				break;
				/* 设置按键低延时状态 */
				case DriverAPP_Byte0_CMD_SET_LowLatency:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetLowLatency();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取触底触发状态 */
				case DriverAPP_Byte0_CMD_GET_BottomTriggerState:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetBottomTriggerState();
				break;
				/* 设置触底触发状态 */
				case DriverAPP_Byte0_CMD_SET_BottomTriggerState:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetBottomTriggerState();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取Profile配置文件 */
				case DriverAPP_Byte0_CMD_GET_ProfileIndex:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetProfileIndex();
				break;
				/* 设置Profile配置文件 */
				case DriverAPP_Byte0_CMD_SET_ProfileIndex:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetProfileIndex();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取电量 */
				case DriverAPP_Byte0_CMD_GET_Battery:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetBattery();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取单个触发值 */
				case DriverAPP_Byte0_CMD_GET_SingleTrigger:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetSingleTrigger();
				break;
				/* 设置单个触发值 */
				case DriverAPP_Byte0_CMD_SET_SingleTrigger:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetSingleTrigger();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 设置全部按键行程 */
				case DriverAPP_Byte0_CMD_SET_ALLTrigger:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetALLTrigger();
				break;
				/* 获取全部按键行程 */
				case DriverAPP_Byte0_CMD_GET_ALLTrigger:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetALLTrigger();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 设置灯光信息 */
				case DriverAPP_Byte0_CMD_SET_LEDData:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetLEDData();
				break;
				/* 获取灯光信息 */
				case DriverAPP_Byte0_CMD_GET_LEDData:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetLEDData();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取自定义按键灯 */
				case DriverAPP_Byte0_CMD_GET_CustomModeLEDData:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetCustomModeLEDData();
				break;
				/* 设置自定义按键灯 */
				case DriverAPP_Byte0_CMD_SET_CustomModeLEDData:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetCustomModeLEDData();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取单个快速触发状态 */
				case DriverAPP_Byte0_CMD_GET_SingleTriggerStatus:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetTriggerStatus();
				break;
				/* 设置单个快速触发状态 */
				case DriverAPP_Byte0_CMD_SET_SingleTriggerStatus:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetTriggerStatus();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取所有按键行程 */
				case DriverAPP_Byte0_CMD_GET_AllKeyTrigger:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetAllKeyTrigger();
				break;
				/* 设置所有按键行程 */
				case DriverAPP_Byte0_CMD_SET_AllKeyTrigger:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetAllKeyTrigger();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取连续快速触发模式 */
				case DriverAPP_Byte0_CMD_GET_ContinueTriggerMode:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetContinueTrigger();
				break;
				/* 设置连续快速触发模式 */
				case DriverAPP_Byte0_CMD_SET_ContinueTriggerMode:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetContinueTrigger();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取动态键程数据 */
				case DriverAPP_Byte0_CMD_GET_DKS:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetDKS();
				break;
				/* 设置动态键程数据 */
				case DriverAPP_Byte0_CMD_SET_DKS:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetDKS();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取按住/单击切换数据 */
				case DriverAPP_Byte0_CMD_GET_MT:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetMT();
				break;
				/* 设置按住/单击切换数据 */
				case DriverAPP_Byte0_CMD_SET_MT:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetMT();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取按键切换开关数据 */
				case DriverAPP_Byte0_CMD_GET_TGL:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetTGL();
				break;
				/* 设置按键切换开关数据 */
				case DriverAPP_Byte0_CMD_SET_TGL:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetTGL();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取macro */
				case DriverAPP_Byte0_CMD_GET_Macro:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GetMacro();
				break;
				/* 设置macro */
				case DriverAPP_Byte0_CMD_SET_Macro:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SetMacro();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取全部快速触发状态 */
				case DriverAPP_Byte0_CMD_GET_ALLRapidTrigger:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GETALLRapidTrigger();
				break;
				/* 设置全部快速触发状态 */
				case DriverAPP_Byte0_CMD_SET_ALLRapidTrigger:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SETALLRapidTrigger();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取全部连续快速触发状态 */
				case DriverAPP_Byte0_CMD_GET_ALLContinueRapidTrigger:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GETALLContinueRapidTrigger();
				break;
				/* 设置全部连续快速触发状态 */
				case DriverAPP_Byte0_CMD_SET_ALLContinueRapidTrigger:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SETALLContinueRapidTrigger();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 获取全局快速触发模式 */
				case DriverAPP_Byte0_CMD_GET_FULLRapidTrigger:
					DriverAPP_ReceiveData = 0;
					DriverAPP_GETFULLRapidTrigger();
				break;
				/* 设置全局快速触发模式 */
				case DriverAPP_Byte0_CMD_SET_FULLRapidTrigger:
					DriverAPP_ReceiveData = 0;
					DriverAPP_SETFULLRapidTrigger();
				break;
				//-------------------------------------------------------------------------------------------------------------
				/* 进ISP模式 */
				case DriverAPP_Byte0_CMD_Update:
					Hall_PWMOff_Suspend();
					PMC->AOREG1 |=(0x0A<< 16);
					NVIC_SystemReset();
				break;
				//-------------------------------------------------------------------------------------------------------------
				default:
					break;
			}
			//------------------------------------------------------------------------------------------
		}
	}
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_ReportInfo
 * Description  	: 上报设备信息.
 * Byte：0		1		2			3/4			5/6			7/8						9/10					11		12
 * 		CMD[1] 	LEN[1] 	RESULT[1] 	VENDOR[2] 	PRODUCT[2] 	MCU1 FIRMWARE_VER[2] 	MCU2 FIRMWARE_VER[2]	设备回报率	设备打击率
 * Byte：13		14		15			16			17			18						19
 * 		击打延时	WASD切换	6键/全键切换	锁WIN键		按键延迟		Win/Mac模式				低延时模式
 * END *******************************************************************************************************************/
void DriverAPP_ReportInfo(void)
{
	uint8_t i = 0;
	/*********************************************************************************************************************/
	Clear_buffer(s_CustomBufferIn,64);
	/*********************************************************************************************************************/
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_Info;					//命令类型
	s_CustomBufferIn[1]  = 0x12;											//命令长度
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;						//命令状态
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[3]  = ((USB_DEVICE_VID & 0xFF00) >> 8);				//VID高位
	s_CustomBufferIn[4]  = (USB_DEVICE_VID & 0x00FF);						//VID低位
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[5]  = ((USB_DEVICE_PID & 0xFF00) >> 8);				//PID高位
	s_CustomBufferIn[6]  = (USB_DEVICE_PID & 0x00FF);						//PID低位
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[7]  = ((USB_DEVICE_DEMO_BCD_VERSION & 0xFF00) >> 8);   //版本号高位
	s_CustomBufferIn[8]  = (USB_DEVICE_DEMO_BCD_VERSION & 0x00FF);			//版本号低位
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[9]  = (0x01);											//加密狗固件版本号高位
	s_CustomBufferIn[10]  = (0x05);											//加密狗固件版本号低位
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[11] = DriverAPP_Byte7_ReportRate_1KHZ;					//报告率
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[14] = DriverAPP_WASD_Switch;							//WASD切换:WASD和方向键切换
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[15] = DriverAPP_Nkey_Rollover_Switch;					//全键和六键切换
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[16] = DriverAPP_WinLock_Switch;						//锁win
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[17] = DriverAPP_ButtonDelay;							//按键延迟
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[18] = DriverAPP_WinMacMode;							//Win/Mac
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[19] = DriverAPP_LowLatencyMode;						//低延时模式
	//---------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//----------------------------------------------------------------------------------------------------------------------
	//FlashZB25V_Sector_Erase(FLASHZB25V_7_BLOCK_LED,0);
	//FlashZB25V_Write_Data(FLASHZB25V_Block07_Sector112_LED1_Addr, (uint8_t *)&g_DrivetData_LED, 256);
	//FlashZB25V_Write_Data(FLASHZB25V_Block07_Sector113_LED2_Addr, (uint8_t *)&g_DrivetData_LED.User_Define_RGB[151], 256);
	//----------------------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_ReportOnline
 * Description  	: 上报设备是否在线.
 * END *******************************************************************************************************************/
void DriverAPP_ReportOnline(void)
{
	uint8_t i = 0;
	//---------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_Online;					//命令类型
	s_CustomBufferIn[1]  = 0x02;											//命令长度
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;						//命令状态
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[3]  = DriverAPP_Byte3_Online;							//设备是否在线
	//---------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//---------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_ReportNKeyRollover
 * Description  	: 上报全键or六键无冲.
 * END *******************************************************************************************************************/
void DriverAPP_ReportNKeyRollover(void)
{
	uint8_t i = 0;
	//---------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//---------------------------------------------------------------------------------------------------------------------
	/* OUT */
	//---------------------------------------------------------------------------------------------------------------------
	DriverAPP_Nkey_Rollover_Switch = s_CustomBufferOut[2];
	g_variableData.DriverAPP_Nkey_Rollover_Switch = DriverAPP_Nkey_Rollover_Switch;
	//---------------------------------------------------------------------------------------------------------------------
	Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
	Tick_UpdateMCUFlash = 0;
	//---------------------------------------------------------------------------------------------------------------------
	/* IN */
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_NKeyRollover;			//命令类型
	s_CustomBufferIn[1]  = 0x02;											//命令长度
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;						//命令状态
	//---------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[3]  = DriverAPP_Nkey_Rollover_Switch;					//上报全键or六键无冲
	//---------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//---------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetKeycode
 * Description  	: 获取单个按键键值.
 * END *******************************************************************************************************************/
void DriverAPP_GetKeycode(void)
{
	uint8_t i = 0;
	uint8_t DriverAPP_Keycode_Row = 0;
	uint8_t DriverAPP_Keycode_Col = 0;
	uint8_t DriverAPP_Keycode_FnLayout = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* 对应按键的行和列 */
	DriverAPP_Keycode_FnLayout = s_CustomBufferOut[2];
	DriverAPP_Keycode_Row = s_CustomBufferOut[3];
	DriverAPP_Keycode_Col = s_CustomBufferOut[4];
	//-----------------------------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_Keycode;																	//命令类型
	s_CustomBufferIn[1]  = 0x03;																							//命令长度
	//-----------------------------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[2]  = DriverAPP_Keycode_FnLayout;																		//FN层or普通按键层
	//-----------------------------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[3]  = g_variableData.Driver_KeyMatrix_Table[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col];				//上报码值
	s_CustomBufferIn[4]  = 0;																								//上报码值
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetKeycode
 * Description  	: 设置单个按键键值.
 * END *******************************************************************************************************************/
void DriverAPP_SetKeycode(void)
{
	uint8_t i = 0;
	uint8_t DriverAPP_Keycode_Row = 0;
	uint8_t DriverAPP_Keycode_Col = 0;
	uint8_t DriverAPP_Keycode_FnLayout = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* 对应按键的行和列 */
	DriverAPP_Keycode_FnLayout = s_CustomBufferOut[2];
	DriverAPP_Keycode_Row = s_CustomBufferOut[3];																					//行
	DriverAPP_Keycode_Col = s_CustomBufferOut[4];
	//FN层
	if(DriverAPP_Keycode_FnLayout)
	{
		g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] = 0;//列
		g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] |= s_CustomBufferOut[5]<<8;		//设置码值INDEX
		g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] |= s_CustomBufferOut[6];			//设置码值ID

		g_variableData.Driver_KeyMatrix_Table[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] = 0;//列
		g_variableData.Driver_KeyMatrix_Table[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] |= s_CustomBufferOut[5]<<8;		//设置码值INDEX
		g_variableData.Driver_KeyMatrix_Table[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] |= s_CustomBufferOut[6];			//设置码值ID

		s_CustomBufferIn[6]  = (g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] & 0xFF00) >> 8;
		s_CustomBufferIn[7]  = (g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] & 0x00FF);
	}
	else
	{
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] = 0;//列
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] |= s_CustomBufferOut[5]<<8;			//设置码值INDEX
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] |= s_CustomBufferOut[6];				//设置码值ID

		g_variableData.Driver_KeyMatrix_Table[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] = 0;//列
		g_variableData.Driver_KeyMatrix_Table[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] |= s_CustomBufferOut[5]<<8;		//设置码值INDEX
		g_variableData.Driver_KeyMatrix_Table[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] |= s_CustomBufferOut[6];			//设置码值ID

		s_CustomBufferIn[6]  = (g_DrivetData_KeyMatrix.KeyMatrix_Buffer[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] & 0xFF00) >> 8;
		s_CustomBufferIn[7]  = (g_DrivetData_KeyMatrix.KeyMatrix_Buffer[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col] & 0x00FF);
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Flag_NeedUpdate_FlashZB25V |= FLASHZB25V_KeyMatrix;
	Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
	Tick_UpdateMCUFlash = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_Keycode;																	//命令类型
	s_CustomBufferIn[1]  = 0x06;																							//命令长度
	//-----------------------------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[3]  = DriverAPP_Keycode_FnLayout;																		//FN层or普通按键层
	//-----------------------------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[4]  = DriverAPP_Keycode_Row;																			//行
	//-----------------------------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[5]  = DriverAPP_Keycode_Row;																			//列
	//-----------------------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------------------------
	//usb_echo("MT kecode = %x.\r\n",g_variableData.Driver_KeyMatrix_Table[DriverAPP_Keycode_Row][DriverAPP_Keycode_Col]);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetAllKeycode
 * Description  	: 获取全部按键键值.
 * END *******************************************************************************************************************/
void DriverAPP_GetAllKeycode(void)
{
	uint8_t i = 0;
	uint8_t DriverAPP_Keycode_FnLayout = 0;
	uint8_t DriverAPP_Keycode_Index = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	DriverAPP_Keycode_FnLayout = s_CustomBufferOut[2];
	DriverAPP_Keycode_Index = s_CustomBufferOut[3];
	//-----------------------------------------------------------------------------------------------------------------------------------------\
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_ALLKeycode;																//命令类型
	s_CustomBufferIn[1]  = 0x21;																							//命令长度
	//-----------------------------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[3]  = DriverAPP_Keycode_FnLayout;																		//FN层or普通按键层
	//-----------------------------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[4]  = DriverAPP_Keycode_Index;																			//第几包
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* 普通按键层 */
	if(DriverAPP_Keycode_FnLayout == 0)
	{
		switch(DriverAPP_Keycode_Index)
		{
			case 1:
				/* 根据键盘的每一行按键个数来 */
				/* KeyBoardLayout_Table[0][i] */
				for(i = 0; i<15; i++)
				{
					//-----------------------------------------------------------------------------------------------------------------------------------------
					s_CustomBufferIn[i*2+5]  = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[0][i] >> 8;
					s_CustomBufferIn[i*2+6]  = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[0][i];
					//-----------------------------------------------------------------------------------------------------------------------------------------
				};
				break;
			case 2:
				/* 根据键盘的每一行按键个数来 */
				/* KeyBoardLayout_Table[1][i] */
				for(i = 0; i<15; i++)
				{
					//-----------------------------------------------------------------------------------------------------------------------------------------
					s_CustomBufferIn[i*2+5]  = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[1][i] >> 8;
					s_CustomBufferIn[i*2+6]  = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[1][i];
					//-----------------------------------------------------------------------------------------------------------------------------------------
				};
				break;
			case 3:
				/* 根据键盘的每一行按键个数来 */
				/* KeyBoardLayout_Table[2][i] */
				for(i = 0; i<15; i++)
				{
					//-----------------------------------------------------------------------------------------------------------------------------------------
					s_CustomBufferIn[i*2+5]  = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[2][i] >> 8;
					s_CustomBufferIn[i*2+6]  = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[2][i];
					//-----------------------------------------------------------------------------------------------------------------------------------------
				};
				break;
			case 4:
				/* 根据键盘的每一行按键个数来 */
				/* KeyBoardLayout_Table[3][i] */
				for(i = 0; i<15; i++)
				{
					//-----------------------------------------------------------------------------------------------------------------------------------------
					s_CustomBufferIn[i*2+5]  = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[3][i] >> 8;
					s_CustomBufferIn[i*2+6]  = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[3][i];
					//-----------------------------------------------------------------------------------------------------------------------------------------
				};
				break;
			case 5:
				/* 根据键盘的每一行按键个数来 */
				/* KeyBoardLayout_Table[4][i] */
				for(i = 0; i<15; i++)
				{
					//-----------------------------------------------------------------------------------------------------------------------------------------
					s_CustomBufferIn[i*2+5]  = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][i] >> 8;
					s_CustomBufferIn[i*2+6]  = g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][i];
					//-----------------------------------------------------------------------------------------------------------------------------------------
				};
				break;
			default:
				break;
		}
	}
	/* FN组合键对应的码值 */
	else
	{
		switch(DriverAPP_Keycode_Index)
		{
			case 1:
				/* 根据键盘的每一行按键个数来 */
				/* KeyBoardLayout_Table[0][i] */
				for(i = 0; i<15; i++)
				{
					//-----------------------------------------------------------------------------------------------------------------------------------------
					s_CustomBufferIn[i*2+5]  = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[0][i] >> 8;
					s_CustomBufferIn[i*2+6]  = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[0][i];
					//-----------------------------------------------------------------------------------------------------------------------------------------
				};
				break;
			case 2:
				/* 根据键盘的每一行按键个数来 */
				/* KeyBoardLayout_Table[1][i] */
				for(i = 0; i<15; i++)
				{
					//-----------------------------------------------------------------------------------------------------------------------------------------
					s_CustomBufferIn[i*2+5]  = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[1][i] >> 8;
					s_CustomBufferIn[i*2+6]  = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[1][i];
					//-----------------------------------------------------------------------------------------------------------------------------------------
				};
				break;
			case 3:
				/* 根据键盘的每一行按键个数来 */
				/* KeyBoardLayout_Table[2][i] */
				for(i = 0; i<15; i++)
				{
					//-----------------------------------------------------------------------------------------------------------------------------------------
					s_CustomBufferIn[i*2+5]  = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[2][i] >> 8;
					s_CustomBufferIn[i*2+6]  = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[2][i];
					//-----------------------------------------------------------------------------------------------------------------------------------------
				};
				break;
			case 4:
				/* 根据键盘的每一行按键个数来 */
				/* KeyBoardLayout_Table[3][i] */
				for(i = 0; i<15; i++)
				{
					//-----------------------------------------------------------------------------------------------------------------------------------------
					s_CustomBufferIn[i*2+5]  = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[3][i] >> 8;
					s_CustomBufferIn[i*2+6]  = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[3][i];
					//-----------------------------------------------------------------------------------------------------------------------------------------
				};
				break;
			case 5:
				/* 根据键盘的每一行按键个数来 */
				/* KeyBoardLayout_Table[4][i] */
				for(i = 0; i<15; i++)
				{
					//-----------------------------------------------------------------------------------------------------------------------------------------
					s_CustomBufferIn[i*2+5]  = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[4][i] >> 8;
					s_CustomBufferIn[i*2+6]  = g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[4][i];
					//-----------------------------------------------------------------------------------------------------------------------------------------
				};
				break;
			default:
				break;
		}
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetAllKeycode
 * Description  	: 设置全部按键键值.
 * END *******************************************************************************************************************/
void DriverAPP_SetAllKeycode(void)
{
	uint8_t i = 0;
	uint8_t DriverAPP_Keycode_FnLayout = 0;
	uint8_t DriverAPP_Keycode_Index = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	DriverAPP_Keycode_FnLayout = s_CustomBufferOut[2];
	DriverAPP_Keycode_Index = s_CustomBufferOut[3];
	//-----------------------------------------------------------------------------------------------------------------------------------------\
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_ALLKeycode;																//命令类型
	s_CustomBufferIn[1]  = 0x01;																							//命令长度
	//-----------------------------------------------------------------------------------------------------------------------------------------
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	switch(DriverAPP_Keycode_Index)
	{
		case 1:
			/* 根据键盘的每一行按键个数来 */
			/* KeyBoardLayout_Table[0][i] */
			for(i = 0; i<14; i++)
			{
				//-----------------------------------------------------------------------------------------------------------------------------------------
				g_variableData.Driver_KeyMatrix_Table[0][i] = s_CustomBufferOut[i*2+4];
				//-----------------------------------------------------------------------------------------------------------------------------------------
			};
			break;
		case 2:
			/* 根据键盘的每一行按键个数来 */
			/* KeyBoardLayout_Table[1][i] */
			for(i = 0; i<15; i++)
			{
				//-----------------------------------------------------------------------------------------------------------------------------------------
				g_variableData.Driver_KeyMatrix_Table[1][i] = s_CustomBufferOut[i*2+4];
				//-----------------------------------------------------------------------------------------------------------------------------------------
			};
			break;
		case 3:
			/* 根据键盘的每一行按键个数来 */
			/* KeyBoardLayout_Table[2][i] */
			for(i = 0; i<15; i++)
			{
				//-----------------------------------------------------------------------------------------------------------------------------------------
				g_variableData.Driver_KeyMatrix_Table[2][i] = s_CustomBufferOut[i*2+4];
				//-----------------------------------------------------------------------------------------------------------------------------------------
			};
			break;
		case 4:
			/* 根据键盘的每一行按键个数来 */
			/* KeyBoardLayout_Table[3][i] */
			for(i = 0; i<14; i++)
			{
				//-----------------------------------------------------------------------------------------------------------------------------------------
				g_variableData.Driver_KeyMatrix_Table[3][i] = s_CustomBufferOut[i*2+4];
				//-----------------------------------------------------------------------------------------------------------------------------------------
			};
			break;
		case 5:
			/* 根据键盘的每一行按键个数来 */
			/* KeyBoardLayout_Table[4][i] */
			for(i = 0; i<14; i++)
			{
				//-----------------------------------------------------------------------------------------------------------------------------------------
				g_variableData.Driver_KeyMatrix_Table[4][i] = s_CustomBufferOut[i*2+4];
				//-----------------------------------------------------------------------------------------------------------------------------------------
			};
			break;
		case 6:
			/* 根据键盘的每一行按键个数来 */
			/* KeyBoardLayout_Table[5][i] */
			for(i = 0; i<10; i++)
			{
				//-----------------------------------------------------------------------------------------------------------------------------------------
				g_variableData.Driver_KeyMatrix_Table[5][i] = s_CustomBufferOut[i*2+4];
				//-----------------------------------------------------------------------------------------------------------------------------------------
			};
			break;
		default:
			break;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}

/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetWASD
 * Description  	: 获取WASD.
 * END *******************************************************************************************************************/
void DriverAPP_GetWASD(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------\
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_WASD;																	//命令类型
	s_CustomBufferIn[1]  = 0x01;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = DriverAPP_WASD_Switch;																			//WASD是否切换
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetWASD
 * Description  	: 设置WASD.
 * END *******************************************************************************************************************/
void DriverAPP_SetWASD(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------\
	/* out */
	DriverAPP_WinMacMode = s_CustomBufferOut[2];																			//WASD是否切换
	if(DriverAPP_WinMacMode)
	{
		g_variableData.DriverAPP_WASD_Switch = DriverAPP_WASD_Switch;
		//------------------------------------------------------------------------------------------
		/* W变成向上 */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[1][2] = KeyBoardLayout_NormalCode[3][12];
		/* S变成向下 */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[2][2] = KeyBoardLayout_NormalCode[4][8];
		/* A变成向左 */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[2][1] = KeyBoardLayout_NormalCode[4][7];
		/* D变成向右 */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[2][3] = KeyBoardLayout_NormalCode[4][9];
		//------------------------------------------------------------------------------------------
		/* 向上变成W */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[3][12] = KeyBoardLayout_NormalCode[1][2];
		/* 向下变成S */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][8] = KeyBoardLayout_NormalCode[2][2];
		/* 向左变成A */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][7] = KeyBoardLayout_NormalCode[2][1];
		/* 向右变成D */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][9] = KeyBoardLayout_NormalCode[2][3];
		//------------------------------------------------------------------------------------------
		/* 保存flash */
		//Tick_UpdateMCUFlash = 0;
		//Flag_NeedUpdate_VariableFlash = 1;
		//------------------------------------------------------------------------------------------
	}
	else if(DriverAPP_WASD_Switch == 0)
	{
		g_variableData.DriverAPP_WASD_Switch = DriverAPP_WASD_Switch;
		//------------------------------------------------------------------------------------------
		/* W变成向上 */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[1][2] = KeyBoardLayout_NormalCode[1][2];
		/* S变成向下 */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[2][2] = KeyBoardLayout_NormalCode[2][2];
		/* A变成向左 */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[2][1] = KeyBoardLayout_NormalCode[2][1];
		/* D变成向右 */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[2][3] = KeyBoardLayout_NormalCode[2][3];
		//------------------------------------------------------------------------------------------
		/* 向上变成W */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[3][12] = KeyBoardLayout_NormalCode[3][12];
		/* 向下变成S */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][8] = KeyBoardLayout_NormalCode[4][8];
		/* 向左变成A */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][7] = KeyBoardLayout_NormalCode[4][7];
		/* 向右变成D */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][9] = KeyBoardLayout_NormalCode[4][9];
		//------------------------------------------------------------------------------------------
		/* 保存flash */
		//Tick_UpdateMCUFlash = 0;
		//Flag_NeedUpdate_VariableFlash = 1;
		//------------------------------------------------------------------------------------------
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Flag_NeedUpdate_FlashZB25V |= FLASHZB25V_KeyMatrix;
	Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
	Tick_UpdateMCUFlash = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_WASD;																	//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = DriverAPP_WASD_Switch;																			//WASD是否切换
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetMAC
 * Description  	: 获取Mac模式.
 * END *******************************************************************************************************************/
void DriverAPP_GetMAC(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------\
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_Mac;																		//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = DriverAPP_WinMacMode;																			//Mac是否切换
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetMac
 * Description  	: 设置Mac.
 * END *******************************************************************************************************************/
void DriverAPP_SetMac(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------\
	/* out */
	DriverAPP_WinMacMode = s_CustomBufferOut[2];	//Windows和Mac系统切换
	if(DriverAPP_WinMacMode)
	{
		g_variableData.DriverAPP_WinMacMode = DriverAPP_WinMacMode;
		//------------------------------------------------------------------------------------------
		/* Win_L变成Alt_L */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][1] = KeyBoardLayout_NormalCode[4][2];
		/* Alt_L变成Win_L */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][2] = KeyBoardLayout_NormalCode[4][1];
		//------------------------------------------------------------------------------------------
		/* 保存flash */
		//Tick_UpdateMCUFlash = 0;
		//Flag_NeedUpdate_VariableFlash = 1;
		//------------------------------------------------------------------------------------------
	}
	else if(DriverAPP_WinMacMode == 0)
	{
		g_variableData.DriverAPP_WinMacMode = DriverAPP_WinMacMode;
		//------------------------------------------------------------------------------------------
		/* Win_L恢复成Win_L */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][1] = KeyBoardLayout_NormalCode[4][1];
		/* Alt_L恢复成Alt_L */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][2] = KeyBoardLayout_NormalCode[4][2];
		//------------------------------------------------------------------------------------------
		/* 保存flash */
		//Tick_UpdateMCUFlash = 0;
		//Flag_NeedUpdate_VariableFlash = 1;
		//------------------------------------------------------------------------------------------
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Flag_NeedUpdate_FlashZB25V |= FLASHZB25V_KeyMatrix;
	Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
	Tick_UpdateMCUFlash = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_Mac;																	//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = DriverAPP_WASD_Switch;																			//WASD是否切换
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetWinlock
 * Description  	: 获取Winlock状态.
 * END *******************************************************************************************************************/
void DriverAPP_GetWinlock(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------\
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_WinLock;																	//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = DriverAPP_WinLock_Switch;																		//winlock是否切换
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetWinlock
 * Description  	: 设置Winlock状态.
 * END *******************************************************************************************************************/
void DriverAPP_SetWinlock(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	DriverAPP_WinLock_Switch = s_CustomBufferOut[2];
	g_variableData.DriverAPP_WinLock_Switch = DriverAPP_WinLock_Switch;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
	Tick_UpdateMCUFlash = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_WinLock;																	//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = DriverAPP_WinLock_Switch;																		//winlock是否切换
	//-----------------------------------------------------------------------------------------------------------------------------------------
	if(DriverAPP_WinLock_Switch)
	{
		Set_HighLight_Buffer(0,1,SPI1_GROUP,1);
		Hall_set_led_current_max();
	}
	else
	{
		Set_HighLight_Buffer(0,0,SPI1_GROUP,1);
		Hall_set_led_current_max();
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SaveFlash
 * Description  	: 保存flash.
 * END *******************************************************************************************************************/
void DriverAPP_SaveFlash(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SaveFlash;																	//命令类型
	s_CustomBufferIn[1]  = 0x01;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* 保存flash */
	FlashZB25V_SaveAPPData();
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_Restore
 * Description  	: 重置键盘
 * END *******************************************************************************************************************/
void DriverAPP_Restore(void)
{
	uint8_t i = 0;
	uint8_t j = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_Reset;																		//命令类型
	s_CustomBufferIn[1]  = 0x01;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Hall_PWMOff_Suspend();
	/* 参数恢复默认 */
	Variable_Reset();
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* 补抬起 */
	for(i = 0; i < TRANSFER_Group; i++)
	{
		for(j = 0; j < TRANSFER_SIZE; j++)
		{
			LastCurrent_key_state[i][j] = Current_key_state[i][j];
		}
	}

	for(i = 0; i < USB_HID_KEYBOARD_REPORT_LENGTH; i++)
	{
		s_KeyboardBuffer1_DKS[i]=0;
		s_KeyboardBuffer2_DKS[i] = 0;
	}
	Has_Key_Report_Interface1_DKS = 1;
	Has_Key_Report_Interface2_DKS = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetPolling
 * Description  	: 获取上报率
 * END *******************************************************************************************************************/
void DriverAPP_GetPolling(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_Polling;																	//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = DriverAPP_Byte7_ReportRate_8KHZ;																	//上报率
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetPolling
 * Description  	: 设置上报率
 * END *******************************************************************************************************************/
void DriverAPP_SetPolling(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_Polling;																	//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Fail;																		//命令状态
	s_CustomBufferIn[3]  = DriverAPP_Byte7_ReportRate_8KHZ;																	//上报率
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetDebounce
 * Description  	: 获取按键延时
 * END *******************************************************************************************************************/
void DriverAPP_GetDebounce(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_Debounce;																//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = 0x00;																							//按键Debounce
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetDebounce
 * Description  	: 设置按键延时
 * END *******************************************************************************************************************/
void DriverAPP_SetDebounce(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_Debounce;																//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Fail;																		//命令状态
	s_CustomBufferIn[3]  = 0x00;																							//按键Debounce
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetLowLatency
 * Description  	: 获取按键低延时状态
 * END *******************************************************************************************************************/
void DriverAPP_GetLowLatency(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_LowLatency;																//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = 0x00;																							//低延时状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetLowLatency
 * Description  	: 设置按键低延时状态
 * END *******************************************************************************************************************/
void DriverAPP_SetLowLatency(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_LowLatency;																//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Fail;																		//命令状态
	s_CustomBufferIn[3]  = 0x00;																							//低延时状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetBottomTriggerState
 * Description  	: 获取触底触发状态
 * END *******************************************************************************************************************/
void DriverAPP_GetBottomTriggerState(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_BottomTriggerState;														//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = g_variableData.DriverAPP_BottomTriggerState;														//低延时状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetLowLatency
 * Description  	: 设置触底触发状态
 * END *******************************************************************************************************************/
void DriverAPP_SetBottomTriggerState(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	g_variableData.DriverAPP_BottomTriggerState = s_CustomBufferOut[2];
	DriverAPP_BottomTriggerState = g_variableData.DriverAPP_BottomTriggerState;
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_BottomTriggerState;														//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = g_variableData.DriverAPP_BottomTriggerState;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetProfileIndex
 * Description  	: 获取Profile配置文件
 * END *******************************************************************************************************************/
void DriverAPP_GetProfileIndex(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_ProfileIndex;															//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = g_variableData.Profile_Index;																	//哪一个profile
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetProfileIndex
 * Description  	: 设置Profile配置文件
 * END *******************************************************************************************************************/
void DriverAPP_SetProfileIndex(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	if(s_CustomBufferOut[2] < 3)
	{
		g_variableData.Profile_Index = s_CustomBufferOut[2];
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_ProfileIndex;															//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = DriverAPP_BottomTriggerState;																	//哪一个profile
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
	Tick_UpdateMCUFlash = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetBattery
 * Description  	: 获取电量
 * END *******************************************************************************************************************/
void DriverAPP_GetBattery(void)
{
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_Battery;																	//命令类型
	s_CustomBufferIn[1]  = 0x03;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Fail;																		//命令状态
	s_CustomBufferIn[3]  = 0x00;																							//是否充电
	s_CustomBufferIn[4]  = DriverAPP_BottomTriggerState;																	//电量
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetSingleTrigger
 * Description  	: 获取单个按键的触发值
 * END *******************************************************************************************************************/
void DriverAPP_GetSingleTrigger(void)
{
	uint8_t Chain_index;
	uint8_t key_index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	Chain_index = s_CustomBufferOut[3];
	key_index = s_CustomBufferOut[4];
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_SingleTrigger;															//命令类型
	s_CustomBufferIn[1]  = 0x04;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	switch(Chain_index)
	{
		case 0:
			s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint[0][key_index];							//触发点
			s_CustomBufferIn[4]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][key_index];												//按下行程
			s_CustomBufferIn[5]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][key_index];												//抬起行程
			break;
		case 1:
			s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint[1][key_index];							//触发点
			s_CustomBufferIn[4]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][key_index];												//按下行程
			s_CustomBufferIn[5]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[1][key_index];												//抬起行程
			break;
		case 2:
			s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint[2][key_index];							//触发点
			s_CustomBufferIn[4]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][key_index];												//按下行程
			s_CustomBufferIn[5]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[2][key_index];												//抬起行程
			break;
		case 3:
			s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint[3][key_index];							//触发点
			s_CustomBufferIn[4]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][key_index];												//按下行程
			s_CustomBufferIn[5]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[3][key_index];												//抬起行程
			break;
		case 4:
			s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint[4][key_index];							//触发点
			s_CustomBufferIn[4]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][key_index];												//按下行程
			s_CustomBufferIn[5]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[4][key_index];												//抬起行程
			break;
		default:
			break;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetSingleTrigger
 * Description  	: 设置单个按键的触发值
 * END *******************************************************************************************************************/
void DriverAPP_SetSingleTrigger(void)
{
	uint8_t Chain_index;
	uint8_t key_index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	Chain_index = s_CustomBufferOut[3];
	key_index = s_CustomBufferOut[4];
	//-----------------------------------------------------------------------------------------------------------------------------------------
	switch(Chain_index)
	{
		case 0:
			g_DrivetData_TriggerPoint.ActualTriggerPoint[0][key_index] = s_CustomBufferOut[5];							//触发点
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][key_index] = s_CustomBufferOut[6];												//按下行程
			g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][key_index] = s_CustomBufferOut[7];												//抬起行程
			break;
		case 1:
			g_DrivetData_TriggerPoint.ActualTriggerPoint[1][key_index] = s_CustomBufferOut[5];							//触发点
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][key_index] = s_CustomBufferOut[6];												//按下行程
			g_DrivetData_PBPoint.Segment_SET_Trigger_Break[1][key_index] = s_CustomBufferOut[7];												//抬起行程
			break;
		case 2:
			g_DrivetData_TriggerPoint.ActualTriggerPoint[2][key_index] = s_CustomBufferOut[5];							//触发点
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][key_index] = s_CustomBufferOut[6];												//按下行程
			g_DrivetData_PBPoint.Segment_SET_Trigger_Break[2][key_index] = s_CustomBufferOut[7];												//抬起行程
			break;
		case 3:
			g_DrivetData_TriggerPoint.ActualTriggerPoint[3][key_index] = s_CustomBufferOut[5];							//触发点
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][key_index] = s_CustomBufferOut[6];												//按下行程
			g_DrivetData_PBPoint.Segment_SET_Trigger_Break[3][key_index] = s_CustomBufferOut[7];												//抬起行程
			break;
		case 4:
			g_DrivetData_TriggerPoint.ActualTriggerPoint[4][key_index] = s_CustomBufferOut[5];							//触发点
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][key_index] = s_CustomBufferOut[6];												//按下行程
			g_DrivetData_PBPoint.Segment_SET_Trigger_Break[4][key_index] = s_CustomBufferOut[7];												//抬起行程
			break;
		default:
			break;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_SingleTrigger;															//命令类型
	s_CustomBufferIn[1]  = 0x04;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	switch(Chain_index)
	{
		case 0:
			s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint[0][key_index];							//触发点
			s_CustomBufferIn[4]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][key_index];												//按下行程
			s_CustomBufferIn[5]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][key_index];												//抬起行程
			break;
		case 1:
			s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint[1][key_index];							//触发点
			s_CustomBufferIn[4]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][key_index];												//按下行程
			s_CustomBufferIn[5]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[1][key_index];												//抬起行程
			break;
		case 2:
			s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint[2][key_index];							//触发点
			s_CustomBufferIn[4]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][key_index];												//按下行程
			s_CustomBufferIn[5]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[2][key_index];												//抬起行程
			break;
		case 3:
			s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint[3][key_index];							//触发点
			s_CustomBufferIn[4]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][key_index];												//按下行程
			s_CustomBufferIn[5]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[3][key_index];												//抬起行程
			break;
		case 4:
			s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint[4][key_index];							//触发点
			s_CustomBufferIn[4]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][key_index];												//按下行程
			s_CustomBufferIn[5]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[4][key_index];												//抬起行程
			break;
		default:
			break;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetSingleTrigger
 * Description  	: 设置全部按键的触发值
 * END *******************************************************************************************************************/
void DriverAPP_SetALLTrigger(void)
{
	uint8_t i;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	for(i = 0; i<TRANSFER_SIZE; i++)
	{
		g_DrivetData_TriggerPoint.ActualTriggerPoint[0][i] = s_CustomBufferOut[2];											//触发点
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][i] = s_CustomBufferOut[3];										//按下行程
		g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][i] = s_CustomBufferOut[4];										//抬起行程

		g_DrivetData_TriggerPoint.ActualTriggerPoint[1][i] = s_CustomBufferOut[2];											//触发点
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][i] = s_CustomBufferOut[3];										//按下行程
		g_DrivetData_PBPoint.Segment_SET_Trigger_Break[1][i] = s_CustomBufferOut[4];										//抬起行程

		g_DrivetData_TriggerPoint.ActualTriggerPoint[2][i] = s_CustomBufferOut[2];											//触发点
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][i] = s_CustomBufferOut[3];										//按下行程
		g_DrivetData_PBPoint.Segment_SET_Trigger_Break[2][i] = s_CustomBufferOut[4];										//抬起行程

		g_DrivetData_TriggerPoint.ActualTriggerPoint[3][i] = s_CustomBufferOut[2];											//触发点
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][i] = s_CustomBufferOut[3];										//按下行程
		g_DrivetData_PBPoint.Segment_SET_Trigger_Break[3][i] = s_CustomBufferOut[4];										//抬起行程

		g_DrivetData_TriggerPoint.ActualTriggerPoint[4][i] = s_CustomBufferOut[2];											//触发点
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][i] = s_CustomBufferOut[3];										//按下行程
		g_DrivetData_PBPoint.Segment_SET_Trigger_Break[4][i] = s_CustomBufferOut[4];										//抬起行程
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Flag_NeedUpdate_FlashZB25V |= FLASHZB25V_PBPoint;
	Flag_NeedUpdate_FlashZB25V |= FLASHZB25V_TrigerPoint;
	Tick_UpdateMCUFlash = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_ALLTrigger;																//命令类型
	s_CustomBufferIn[1]  = 0x04;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint[0][0];												//触发点
	s_CustomBufferIn[4]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][0];											//按下行程
	s_CustomBufferIn[5]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][0];											//抬起行程
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetALLTrigger
 * Description  	: 获取全部按键的触发值
 * END *******************************************************************************************************************/
void DriverAPP_GetALLTrigger(void)
{
	uint8_t i;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_ALLTrigger;																//命令类型
	s_CustomBufferIn[1]  = 0x04;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint[0][0];												//触发点
	s_CustomBufferIn[4]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][0];											//按下行程
	s_CustomBufferIn[5]  = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][0];											//抬起行程
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetLEDData
 * Description  	: 设置灯光信息
 * END *******************************************************************************************************************/
void DriverAPP_SetLEDData(void)
{
	uint8_t i;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	/* 切模式,如果已经是相同的模式了，就不用重新开始做初始化了 */
	if(g_variableData.LED_ONOFF != s_CustomBufferOut[2])
	{
		Flag_LedMode_ModeInit = 1;
		g_variableData.LED_ONOFF = s_CustomBufferOut[2];																		//灯效总开关
		//-------------------------------------------------------------------------------------------------------------------------------------
		Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
		Tick_UpdateMCUFlash = 0;
		//-------------------------------------------------------------------------------------------------------------------------------------
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* 切模式,如果已经是相同的模式了，就不用重新开始做初始化了 */
	if(g_variableData.LED_MODE != s_CustomBufferOut[3])
	{
		Flag_LedMode_ModeInit = 1;
		g_variableData.LED_MODE = s_CustomBufferOut[3];																			//灯效模式
		//-------------------------------------------------------------------------------------------------------------------------------------
		Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
		Tick_UpdateMCUFlash = 0;
		//-------------------------------------------------------------------------------------------------------------------------------------
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* 切亮度,如果已经是相同的亮度了，就不用重新开始做初始化了 */
	if(g_variableData.LED_BRIGHTNESS != s_CustomBufferOut[4])
	{
		Flag_LedMode_ModeInit = 1;
		g_variableData.LED_BRIGHTNESS = s_CustomBufferOut[4];																	//灯效亮度
		//-------------------------------------------------------------------------------------------------------------------------------------
		Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
		Tick_UpdateMCUFlash = 0;
		//-------------------------------------------------------------------------------------------------------------------------------------
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* 切速度,如果已经是相同的速度了，就不用重新开始做初始化了 */
	if(g_variableData.LED_SPEED != s_CustomBufferOut[5])
	{
		Flag_LedMode_ModeInit = 1;
		g_variableData.LED_SPEED = s_CustomBufferOut[5];																		//灯效速度
		//-------------------------------------------------------------------------------------------------------------------------------------
		Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
		Tick_UpdateMCUFlash = 0;
		//-------------------------------------------------------------------------------------------------------------------------------------
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* 切颜色,如果已经是相同的颜色了，就不用重新开始做初始化了 */
	if((g_variableData.LED_COLOR[0] != s_CustomBufferOut[6]) || (g_variableData.LED_COLOR[1] != s_CustomBufferOut[7]) || (g_variableData.LED_COLOR[2] != s_CustomBufferOut[8]))
	{
		g_variableData.LED_RGB_MODE = s_CustomBufferOut[9];
		/* 1的时候键盘做本体自带的颜色，不用驱动的颜色 */
		if(g_variableData.LED_RGB_MODE == 0)
		{
			Flag_LedMode_ModeInit = 1;
			g_variableData.LED_COLOR[0] = s_CustomBufferOut[6];																		//RGB
			g_variableData.LED_COLOR[1] = s_CustomBufferOut[7];																		//RGB
			g_variableData.LED_COLOR[2] = s_CustomBufferOut[8];																		//RGB
		}
		//-------------------------------------------------------------------------------------------------------------------------------------
		Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
		Tick_UpdateMCUFlash = 0;
		//-------------------------------------------------------------------------------------------------------------------------------------
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* 切RGB模式,如果已经是相同的RGB模式了，就不用重新开始做初始化了 */
	if(g_variableData.LED_RGB_MODE != s_CustomBufferOut[9])
	{
		g_variableData.LED_RGB_MODE = s_CustomBufferOut[9];																		//RGB模式
		/* 1的时候键盘做本体自带的颜色，不用驱动的颜色 */
		if(g_variableData.LED_RGB_MODE == 0)
		{
			Flag_LedMode_ModeInit = 1;
		}
		//-------------------------------------------------------------------------------------------------------------------------------------
		Flag_NeedUpdate_FlashZB25V |= FLASH5528_Data;
		Tick_UpdateMCUFlash = 0;
		//-------------------------------------------------------------------------------------------------------------------------------------
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	g_variableData.LIGHT_BEACON = s_CustomBufferOut[10];																	//灯标灯效
	g_variableData.LIGHT_BEACON_COLOR[0] = s_CustomBufferOut[11];															//灯标主色调
	g_variableData.LIGHT_BEACON_COLOR[1] = s_CustomBufferOut[12];															//灯标主色调
	g_variableData.LIGHT_BEACON_COLOR[2] = s_CustomBufferOut[13];															//灯标主色调
	g_variableData.LIGHT_BEACON_RGB_MODE = s_CustomBufferOut[14];															//灯标RGB模式
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_LEDData;																	//命令类型
	s_CustomBufferIn[1]  = 0x09;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = g_variableData.LED_ONOFF;																		//灯效总开关
	s_CustomBufferIn[4]  = g_variableData.LED_MODE;																			//灯效模式
	s_CustomBufferIn[5]  = g_variableData.LED_BRIGHTNESS;																	//灯效亮度
	s_CustomBufferIn[6]  = g_variableData.LED_SPEED;																		//灯效速度
	s_CustomBufferIn[7]  = g_variableData.LED_COLOR[0];																		//主色调RGB
	s_CustomBufferIn[8]  = g_variableData.LED_COLOR[1];																		//主色调RGB
	s_CustomBufferIn[9]  = g_variableData.LED_COLOR[2];																		//主色调RGB
	s_CustomBufferIn[10]  = g_variableData.LED_RGB_MODE;																	//RGB模式
	s_CustomBufferIn[11]  = g_variableData.LIGHT_BEACON;																	//灯标灯效
	s_CustomBufferIn[12]  = g_variableData.LIGHT_BEACON_COLOR[0];															//灯标主色调
	s_CustomBufferIn[13]  = g_variableData.LIGHT_BEACON_COLOR[1];															//灯标主色调
	s_CustomBufferIn[14]  = g_variableData.LIGHT_BEACON_COLOR[2];															//灯标主色调
	s_CustomBufferIn[15]  = g_variableData.LIGHT_BEACON_RGB_MODE;															//灯标RGB模式
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------

}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetLEDData
 * Description  	: 获取灯光信息
 * END *******************************************************************************************************************/
void DriverAPP_GetLEDData(void)
{
	uint8_t i;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_LEDData;																	//命令类型
	s_CustomBufferIn[1]  = 0x09;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = g_variableData.LED_ONOFF;																		//灯效总开关
	s_CustomBufferIn[4]  = g_variableData.LED_MODE;																			//灯效模式
	s_CustomBufferIn[5]  = g_variableData.LED_BRIGHTNESS;																	//灯效亮度
	s_CustomBufferIn[6]  = g_variableData.LED_SPEED;																		//灯效速度
	s_CustomBufferIn[7]  = g_variableData.LED_COLOR[0];																		//主色调RGB
	s_CustomBufferIn[8]  = g_variableData.LED_COLOR[1];																		//主色调RGB
	s_CustomBufferIn[9]  = g_variableData.LED_COLOR[2];																		//主色调RGB
	s_CustomBufferIn[10]  = g_variableData.LED_RGB_MODE;																	//RGB模式
	s_CustomBufferIn[11]  = g_variableData.LIGHT_BEACON;																	//灯标灯效
	/* 协议没写要上报这个logo灯的RGB，先把它放在最后看看 */
	s_CustomBufferIn[12]  = g_variableData.LIGHT_BEACON_COLOR[0];															//灯标主色调
	s_CustomBufferIn[13]  = g_variableData.LIGHT_BEACON_COLOR[1];															//灯标主色调
	s_CustomBufferIn[14]  = g_variableData.LIGHT_BEACON_COLOR[2];															//灯标主色调
	s_CustomBufferIn[15]  = g_variableData.LIGHT_BEACON_RGB_MODE;															//灯标RGB模式
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetCustomModeLEDData
 * Description  	: 获取自定义按键灯
 * END *******************************************************************************************************************/
void DriverAPP_GetCustomModeLEDData(void)
{
	uint8_t i;
	uint8_t DriverAPP_LED_Row = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* OUT */
	DriverAPP_LED_Row = s_CustomBufferOut[2];
	if(DriverAPP_LED_Row > 0 && DriverAPP_LED_Row < 6 )
	{
		DriverAPP_LED_Row = DriverAPP_LED_Row - 1;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_CustomModeLEDData;														//命令类型
	s_CustomBufferIn[1]  = 0x2F;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = DriverAPP_LED_Row+1;																				//Packet ID,根据键盘的行行列来
	switch(DriverAPP_LED_Row)
	{
		case 0:
			for(i = 0; i<20; i++)
			{
				s_CustomBufferIn[3*i+4] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0];
				s_CustomBufferIn[3*i+5] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1];
				s_CustomBufferIn[3*i+6] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2];
			}
			break;
		case 1:
			for(i = 0; i<20; i++)
			{
				s_CustomBufferIn[3*i+4] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0];
				s_CustomBufferIn[3*i+5] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1];
				s_CustomBufferIn[3*i+6] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2];
			}
			break;
		case 2:
			for(i = 0; i<20; i++)
			{
				s_CustomBufferIn[3*i+4] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0];
				s_CustomBufferIn[3*i+5] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1];
				s_CustomBufferIn[3*i+6] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2];
			}
			break;
		case 3:
			for(i = 0; i<20; i++)
			{
				s_CustomBufferIn[3*i+4] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0];
				s_CustomBufferIn[3*i+5] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1];
				s_CustomBufferIn[3*i+6] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2];
			}
			break;
		case 4:
			for(i = 0; i<20; i++)
			{
				s_CustomBufferIn[3*i+4] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0];
				s_CustomBufferIn[3*i+5] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1];
				s_CustomBufferIn[3*i+6] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2];
			}
			break;
		default:
			break;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetCustomModeLEDData
 * Description  	: 设置自定义按键灯
 * END *******************************************************************************************************************/
void DriverAPP_SetCustomModeLEDData(void)
{
	uint8_t i;
	uint8_t DriverAPP_LED_Row = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	DriverAPP_LED_Row = s_CustomBufferOut[2];
	if(DriverAPP_LED_Row > 0 && DriverAPP_LED_Row < 6 )
	{
		DriverAPP_LED_Row -= 1;

		switch(DriverAPP_LED_Row)
		{
			case 0:
				for(i = 0; i<20; i++)
				{
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0] = s_CustomBufferOut[3*i+3];
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1] = s_CustomBufferOut[3*i+4];
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2] = s_CustomBufferOut[3*i+5];
				}
				break;
			case 1:
				for(i = 0; i<20; i++)
				{
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0] = s_CustomBufferOut[3*i+3];
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1] = s_CustomBufferOut[3*i+4];
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2] = s_CustomBufferOut[3*i+5];
				}
				break;
			case 2:
				for(i = 0; i<20; i++)
				{
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0] = s_CustomBufferOut[3*i+3];
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1] = s_CustomBufferOut[3*i+4];
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2] = s_CustomBufferOut[3*i+5];
				}
				break;
			case 3:
				for(i = 0; i<20; i++)
				{
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0] = s_CustomBufferOut[3*i+3];
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1] = s_CustomBufferOut[3*i+4];
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2] = s_CustomBufferOut[3*i+5];
				}
				break;
			case 4:
				for(i = 0; i<20; i++)
				{
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0] = s_CustomBufferOut[3*i+3];
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1] = s_CustomBufferOut[3*i+4];
					g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2] = s_CustomBufferOut[3*i+5];
				}
				break;
			default:
				break;
		}
		//-----------------------------------------------------------------------------------------------------------------------------------------
		/* IN */
		s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_CustomModeLEDData;														//命令类型
		s_CustomBufferIn[1]  = 0x00;																							//命令长度
		s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
		switch(DriverAPP_LED_Row)
		{
			case 0:
				for(i = 0; i<20; i++)
				{
					s_CustomBufferIn[3*i+3] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0];
					s_CustomBufferIn[3*i+4] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1];
					s_CustomBufferIn[3*i+5] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2];
				}
				break;
			case 1:
				for(i = 0; i<20; i++)
				{
					s_CustomBufferIn[3*i+3] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0];
					s_CustomBufferIn[3*i+4] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1];
					s_CustomBufferIn[3*i+5] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2];
				}
				break;
			case 2:
				for(i = 0; i<20; i++)
				{
					s_CustomBufferIn[3*i+3] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0];
					s_CustomBufferIn[3*i+4] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1];
					s_CustomBufferIn[3*i+5] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2];
				}
				break;
			case 3:
				for(i = 0; i<20; i++)
				{
					s_CustomBufferIn[3*i+3] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0];
					s_CustomBufferIn[3*i+4] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1];
					s_CustomBufferIn[3*i+5] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2];
				}
				break;
			case 4:
				for(i = 0; i<20; i++)
				{
					s_CustomBufferIn[3*i+3] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+0];
					s_CustomBufferIn[3*i+4] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+1];
					s_CustomBufferIn[3*i+5] = g_DrivetData_LED.User_Define_RGB[DriverAPP_LED_Row*63+3*i+2];
				}
				break;
			default:
				break;
		}
	}
	else
	{
		//-----------------------------------------------------------------------------------------------------------------------------------------
		/* IN */
		s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_CustomModeLEDData;														//命令类型
		s_CustomBufferIn[1]  = 0x00;																							//命令长度
		s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Fail;																		//命令状态
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetTriggerStatus
 * Description  	: 获取单个快速触发状态
 * END *******************************************************************************************************************/
void DriverAPP_GetTriggerStatus(void)
{
	uint8_t Chain_index;
	uint8_t key_index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	Chain_index = s_CustomBufferOut[2];
	key_index = s_CustomBufferOut[3];
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_SingleTriggerStatus;														//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[Chain_index][key_index];						//抬起行程
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetTriggerStatus
 * Description  	: 设置单个快速触发状态
 * END *******************************************************************************************************************/
void DriverAPP_SetTriggerStatus(void)
{
	uint8_t Chain_index;
	uint8_t key_index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	Chain_index = s_CustomBufferOut[2];
	key_index = s_CustomBufferOut[3];
	g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[Chain_index][key_index] = s_CustomBufferOut[4];
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Flag_NeedUpdate_FlashZB25V |= FLASHZB25V_TrigerPoint;
	Tick_UpdateMCUFlash = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_SingleTriggerStatus;														//命令类型
	s_CustomBufferIn[1]  = 0x04;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = 0;
	s_CustomBufferIn[4]  = 0;
	s_CustomBufferIn[5]  = g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[Chain_index][key_index];						//抬起行程
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetAllKeyTrigger
 * Description  	: 获取所有按键行程
 * END *******************************************************************************************************************/
void DriverAPP_GetAllKeyTrigger(void)
{
	uint8_t Chain_index;
	uint8_t key_index;
	 uint8_t Packet_Index = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	Packet_Index = s_CustomBufferOut[2];
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_AllKeyTrigger;															//命令类型
	s_CustomBufferIn[1]  = 0x2F;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = Packet_Index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	switch(Packet_Index)
	{
		case 1:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			//for(key_index = 0; key_index < SPI0_TRANSFER_SIZE; key_index++)
			for(key_index = 0; key_index < 15; key_index++)
			{
				s_CustomBufferIn[4+key_index*3] = g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI0_GROUP][key_index];
				s_CustomBufferIn[5+key_index*3] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI0_GROUP][key_index];
				s_CustomBufferIn[6+key_index*3] = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI0_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 2:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < 15; key_index++)
			{
				s_CustomBufferIn[4+key_index*3] = g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI1_GROUP][key_index];
				s_CustomBufferIn[5+key_index*3] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI1_GROUP][key_index];
				s_CustomBufferIn[6+key_index*3] = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI1_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 3:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < 15; key_index++)
			{
				s_CustomBufferIn[4+key_index*3] = g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI3_GROUP][key_index];
				s_CustomBufferIn[5+key_index*3] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI3_GROUP][key_index];
				s_CustomBufferIn[6+key_index*3] = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI3_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 4:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < 15; key_index++)
			{
				s_CustomBufferIn[4+key_index*3] = g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI4_GROUP][key_index];
				s_CustomBufferIn[5+key_index*3] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI4_GROUP][key_index];
				s_CustomBufferIn[6+key_index*3] = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI4_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 5:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < 15; key_index++)
			{
				s_CustomBufferIn[4+key_index*3] = g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI6_GROUP][key_index];
				s_CustomBufferIn[5+key_index*3] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI6_GROUP][key_index];
				s_CustomBufferIn[6+key_index*3] = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI6_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		default:
			break;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetAllKeyTrigger
 * Description  	: 获取所有按键行程
 * END *******************************************************************************************************************/
void DriverAPP_SetAllKeyTrigger(void)
{
	uint8_t Chain_index;
	uint8_t key_index;
	 uint8_t Packet_Index = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	Packet_Index = s_CustomBufferOut[2];
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_AllKeyTrigger;															//命令类型
	s_CustomBufferIn[1]  = 0x01;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = Packet_Index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	switch(Packet_Index)
	{
		case 1:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI0_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI0_GROUP][key_index] = s_CustomBufferOut[3+key_index*3];
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI0_GROUP][key_index] = s_CustomBufferOut[4+key_index*3];
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI0_GROUP][key_index] = s_CustomBufferOut[5+key_index*3];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 2:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI1_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI1_GROUP][key_index] = s_CustomBufferOut[3+key_index*3];
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI1_GROUP][key_index] = s_CustomBufferOut[4+key_index*3];
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI1_GROUP][key_index] = s_CustomBufferOut[5+key_index*3];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 3:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI3_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI3_GROUP][key_index] = s_CustomBufferOut[3+key_index*3];
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI3_GROUP][key_index] = s_CustomBufferOut[4+key_index*3];
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI3_GROUP][key_index] = s_CustomBufferOut[5+key_index*3];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 4:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI4_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI4_GROUP][key_index] = s_CustomBufferOut[3+key_index*3];
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI4_GROUP][key_index] = s_CustomBufferOut[4+key_index*3];
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI4_GROUP][key_index] = s_CustomBufferOut[5+key_index*3];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 5:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI7_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_TriggerPoint.ActualTriggerPoint[SPI6_GROUP][key_index] = s_CustomBufferOut[3+key_index*3];
				g_DrivetData_PBPoint.Segment_SET_Trigger_Press[SPI6_GROUP][key_index] = s_CustomBufferOut[4+key_index*3];
				g_DrivetData_PBPoint.Segment_SET_Trigger_Break[SPI6_GROUP][key_index] = s_CustomBufferOut[5+key_index*3];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		default:
			break;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Flag_NeedUpdate_FlashZB25V |=( FLASHZB25V_TrigerPoint | FLASHZB25V_PBPoint);
	Tick_UpdateMCUFlash = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetContinueTrigger
 * Description  	: 获取连续快速触发模式
 * END *******************************************************************************************************************/
void DriverAPP_GetContinueTrigger(void)
{
	uint8_t Chain_index;
	uint8_t key_index;
	 uint8_t Packet_Index = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_ContinueTriggerMode;														//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = g_DrivetData_ContinuePoint.ContinueTriggerPoint_OnOff;											//是否打开连续快速触发模式
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetContinueTrigger
 * Description  	: 设置连续快速触发模式
 * END *******************************************************************************************************************/
void DriverAPP_SetContinueTrigger(void)
{
	uint8_t Chain_index;
	uint8_t key_index;
	 uint8_t Packet_Index = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_DKS;																		//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = g_DrivetData_ContinuePoint.ContinueTriggerPoint_OnOff;											//是否打开连续快速触发模式
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetDKS
 * Description  	: 设置动态键程数据(DKS)
 * END *******************************************************************************************************************/
void DriverAPP_GetDKS(void)
{
	uint8_t DKS_id;
	uint8_t i;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	DKS_id = s_CustomBufferOut[2];
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_DKS;										//命令类型
	s_CustomBufferIn[1]  = 0x12;															//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;										//命令状态																						//命令长度
	s_CustomBufferIn[3] = DKS_id;
	for(i = 0; i<4; i++)
	{
		s_CustomBufferOut[i*4+4] = g_DrivetData_DKS.DKS_Buffer[DKS_id*17+1+i*4];			//status_low
		s_CustomBufferOut[i*4+5] = g_DrivetData_DKS.DKS_Buffer[DKS_id*17+2+i*4];			//status_high
		s_CustomBufferOut[i*4+6] = g_DrivetData_DKS.DKS_Buffer[DKS_id*17+3+i*4];			//码值High
		s_CustomBufferOut[i*4+7] = g_DrivetData_DKS.DKS_Buffer[DKS_id*17+4+i*4];			//码值Low
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetDKS  111
 * Description  	: 获取动态键程数据(DKS)
 * END *******************************************************************************************************************/
void DriverAPP_SetDKS(void)
{
	uint8_t DKS_id;
	uint8_t i;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	DKS_id = s_CustomBufferOut[2];
	g_DrivetData_DKS.DKS_Buffer[DKS_id*17] = s_CustomBufferOut[2];
	for(i = 0; i<4; i++)
	{
		g_DrivetData_DKS.DKS_Buffer[DKS_id*17+1+i*4] = s_CustomBufferOut[i*4+3];			//status_low
		g_DrivetData_DKS.DKS_Buffer[DKS_id*17+2+i*4] = s_CustomBufferOut[i*4+4];			//status_high
		g_DrivetData_DKS.DKS_Buffer[DKS_id*17+3+i*4] = s_CustomBufferOut[i*4+5];			//码值High
		g_DrivetData_DKS.DKS_Buffer[DKS_id*17+4+i*4] = s_CustomBufferOut[i*4+6];			//码值Low
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Flag_NeedUpdate_FlashZB25V |= FLASHZB25V_DKS;
	Tick_UpdateMCUFlash = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_DKS;																		//命令类型
	s_CustomBufferIn[1]  = 0x01;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetTGL
 * Description  	: 获取按键切换开关数据(TGL)
 * END *******************************************************************************************************************/
void DriverAPP_GetTGL(void)
{
	uint8_t TGL_id;
	uint8_t i;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	TGL_id = s_CustomBufferOut[2];
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_TGL;																		//命令类型
	s_CustomBufferIn[1]  = 0x04;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = TGL_id;
	s_CustomBufferIn[4]  = g_DrivetData_TGL.TGL_Buffer[TGL_id][1];
	s_CustomBufferIn[5]  = g_DrivetData_TGL.TGL_Buffer[TGL_id][2];
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetTGL
 * Description  	: 设置按键切换开关数据(TGL)
 * END *******************************************************************************************************************/
void DriverAPP_SetTGL(void)
{
	uint8_t TGL_id;
	uint8_t i;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	TGL_id = s_CustomBufferOut[2];
	g_DrivetData_TGL.TGL_Buffer[TGL_id][0] = s_CustomBufferOut[2];			//TGL ID
	g_DrivetData_TGL.TGL_Buffer[TGL_id][1] = s_CustomBufferOut[3];			//Keycode_High
	g_DrivetData_TGL.TGL_Buffer[TGL_id][2] = s_CustomBufferOut[4];			//Keycode_Low
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Flag_NeedUpdate_FlashZB25V |= FLASHZB25V_TGL;
	Tick_UpdateMCUFlash = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_TGL;																		//命令类型
	s_CustomBufferIn[1]  = 0x01;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetMT
 * Description  	: 获取按住/单击切换数据(MT)
 * END *******************************************************************************************************************/
void DriverAPP_GetMT(void)
{
	uint8_t MT_id;
	uint8_t i;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	MT_id = s_CustomBufferOut[2];
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_MT;																		//命令类型
	s_CustomBufferIn[1]  = 0x08;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = MT_id;
	s_CustomBufferIn[4]  = g_DrivetData_MT.MT_Buffer[MT_id][1];
	s_CustomBufferIn[5]  = g_DrivetData_MT.MT_Buffer[MT_id][2];
	s_CustomBufferIn[6]  = g_DrivetData_MT.MT_Buffer[MT_id][3];
	s_CustomBufferIn[7]  = g_DrivetData_MT.MT_Buffer[MT_id][4];
	s_CustomBufferIn[8]  = g_DrivetData_MT.MT_Buffer[MT_id][5];
	s_CustomBufferIn[9]  = g_DrivetData_MT.MT_Buffer[MT_id][6];
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetMT
 * Description  	: 设置按住/单击切换数据(MT)
 * END *******************************************************************************************************************/
void DriverAPP_SetMT(void)
{
	uint8_t MT_id;
	uint8_t i;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	MT_id = s_CustomBufferOut[2];
	g_DrivetData_MT.MT_Buffer[MT_id][0] = s_CustomBufferOut[2];			//TGL ID
	g_DrivetData_MT.MT_Buffer[MT_id][1] = s_CustomBufferOut[3];			//TIME_Low
	g_DrivetData_MT.MT_Buffer[MT_id][2] = s_CustomBufferOut[4];			//TIME_High
	g_DrivetData_MT.MT_Buffer[MT_id][3] = s_CustomBufferOut[5];			//长按的码值/Keycode_Low
	g_DrivetData_MT.MT_Buffer[MT_id][4] = s_CustomBufferOut[6];			//长按的码值/Keycode_High
	g_DrivetData_MT.MT_Buffer[MT_id][5] = s_CustomBufferOut[7];			//单击的码值/Keycode_Low
	g_DrivetData_MT.MT_Buffer[MT_id][6] = s_CustomBufferOut[8];			//单击的码值/Keycode_High
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Flag_NeedUpdate_FlashZB25V |= FLASHZB25V_MT;
	Tick_UpdateMCUFlash = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_MT;																		//命令类型
	s_CustomBufferIn[1]  = 0x01;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GetMacro
 * Description  	: 获取Macro
 * END *******************************************************************************************************************/
void DriverAPP_GetMacro(void)
{
	uint8_t Macro_id;
	uint8_t Packet_index;
	uint8_t i;
	uint32_t Macro_Address;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	Macro_id = s_CustomBufferOut[2];
	if(Macro_id == 0)
	{
		/* IN */
		s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_Macro;																	//命令类型
		s_CustomBufferIn[1]  = 0x11;																							//命令长度
		s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Fail;																		//命令状态
		s_CustomBufferIn[3]  = Macro_id;
		s_CustomBufferIn[4]  = Packet_index;
		return;
	}

	Packet_index = s_CustomBufferOut[3];
	if(Packet_index == 0)
	{
		/* IN */
		s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_Macro;																	//命令类型
		s_CustomBufferIn[1]  = 0x11;																							//命令长度
		s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Fail;																		//命令状态
		s_CustomBufferIn[3]  = Macro_id;
		s_CustomBufferIn[4]  = Packet_index;
		return;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_Macro;																	//命令类型
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	/* 从block05开始，每一个page存一个宏，一共50个宏，虽然浪费空间，但是好算. */
	Macro_Address = FLASHZB25V_Block01_Sector23_MACRO_Addr + (Macro_id-1)*FLASHZB25V_PageSize;
	FlashZB25V_Read_Data(Macro_Address, (uint8_t *)&g_DrivetData_MACRO, 64);
	if(g_DrivetData_MACRO.Buffer_Macro[0] == 0xFF)
	{
		s_CustomBufferIn[1]  = 0x03;																						//命令长度
		for(i=0;i<61;i++)
		{
			s_CustomBufferIn[i+3] = 0;
		}
	}
	else
	{
		s_CustomBufferIn[1]  = 0x11;																						//命令长度
		for(i=0;i<61;i++)
		{
			s_CustomBufferIn[i+3] = g_DrivetData_MACRO.Buffer_Macro[(Packet_index-1)*64+i];
		}
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SetMacro
 * Description  	: 设置Macro
 * END *******************************************************************************************************************/
void DriverAPP_SetMacro(void)
{
	uint8_t Macro_id;
	uint8_t Packet_index;
	uint16_t i = 0;
	uint32_t Macro_Address;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Macro_id = s_CustomBufferOut[2];
	if(Macro_id == 0)
	{
		/* IN */
		s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_Macro;																	//命令类型
		s_CustomBufferIn[1]  = 0x03;																							//命令长度
		s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Fail;																		//命令状态
		s_CustomBufferIn[3]  = Macro_id;
		s_CustomBufferIn[4]  = Packet_index;
		return;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Packet_index = s_CustomBufferOut[3];
	if(Packet_index == 0)
	{
		/* IN */
		s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_Macro;																	//命令类型
		s_CustomBufferIn[1]  = 0x03;																							//命令长度
		s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Fail;																		//命令状态
		s_CustomBufferIn[3]  = Macro_id;
		s_CustomBufferIn[4]  = Packet_index;
		return;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	for(i=0;i<62;i++)
	{
		g_DrivetData_MACRO.Buffer_Macro[(Packet_index-1)*64+i] = s_CustomBufferOut[i+2];
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Flag_NeedUpdate_FlashZB25V |= FLASHZB25V_Macro;
	Tick_UpdateMCUFlash = 0;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_Macro;																		//命令类型
	s_CustomBufferIn[1]  = 0x03;																								//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																			//命令状态
	s_CustomBufferIn[3]  = Macro_id;
	s_CustomBufferIn[4]  = Packet_index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GETALLRapidTrigger
 * Description  	: 获取全部快速触发状态
 * END *******************************************************************************************************************/
void DriverAPP_GETALLRapidTrigger(void)
{
	uint8_t Row_index;
	uint8_t key_index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	Row_index = s_CustomBufferOut[2];
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_ALLRapidTrigger;															//命令类型
	s_CustomBufferIn[1]  = 0x11;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = Row_index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	switch(Row_index)
	{
		case 0:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI7_TRANSFER_SIZE; key_index++)
			{
				s_CustomBufferIn[4+key_index] = g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI6_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 1:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI3_TRANSFER_SIZE; key_index++)
			{
				s_CustomBufferIn[4+key_index] = g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI3_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 2:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI4_TRANSFER_SIZE; key_index++)
			{
				s_CustomBufferIn[4+key_index] = g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI4_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 3:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI0_TRANSFER_SIZE; key_index++)
			{
				s_CustomBufferIn[4+key_index] = g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI0_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 4:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI1_TRANSFER_SIZE; key_index++)
			{
				s_CustomBufferIn[4+key_index] = g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI1_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		default:
			break;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SETALLRapidTrigger
 * Description  	: 设置全部快速触发状态
 * END *******************************************************************************************************************/
void DriverAPP_SETALLRapidTrigger(void)
{
	uint8_t Row_index;
	uint8_t key_index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	Row_index = s_CustomBufferOut[2];

	switch(Row_index)
	{
		case 0:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI7_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI6_GROUP][key_index] = s_CustomBufferIn[4+key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 1:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI3_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI3_GROUP][key_index] = s_CustomBufferIn[4+key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 2:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI4_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI4_GROUP][key_index] = s_CustomBufferIn[4+key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 3:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI0_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI0_GROUP][key_index] = s_CustomBufferIn[4+key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 4:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI1_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI1_GROUP][key_index] = s_CustomBufferIn[4+key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		default:
			break;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_ALLRapidTrigger;															//命令类型
	s_CustomBufferIn[1]  = 0x01;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GETALLContinueRapidTrigger
 * Description  	: 获取全部连续快速触发状态
 * END *******************************************************************************************************************/
void DriverAPP_GETALLContinueRapidTrigger(void)
{
	uint8_t Row_index;
	uint8_t key_index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	Row_index = s_CustomBufferOut[2];
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_ALLContinueRapidTrigger;													//命令类型
	s_CustomBufferIn[1]  = 0x11;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = Row_index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	switch(Row_index)
	{
		case 0:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI7_TRANSFER_SIZE; key_index++)
			{
				s_CustomBufferIn[4+key_index] = g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI6_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 1:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI3_TRANSFER_SIZE; key_index++)
			{
				s_CustomBufferIn[4+key_index] = g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI3_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 2:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI4_TRANSFER_SIZE; key_index++)
			{
				s_CustomBufferIn[4+key_index] = g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI4_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 3:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI0_TRANSFER_SIZE; key_index++)
			{
				s_CustomBufferIn[4+key_index] = g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI0_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 4:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI1_TRANSFER_SIZE; key_index++)
			{
				s_CustomBufferIn[4+key_index] = g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI1_GROUP][key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		default:
			break;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SETALLContinueRapidTrigger
 * Description  	: 设置全部连续快速触发状态
 * END *******************************************************************************************************************/
void DriverAPP_SETALLContinueRapidTrigger(void)
{
	uint8_t Row_index;
	uint8_t key_index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	Row_index = s_CustomBufferOut[2];

	switch(Row_index)
	{
		case 0:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI7_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI6_GROUP][key_index] = s_CustomBufferIn[4+key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 1:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI3_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI3_GROUP][key_index] = s_CustomBufferIn[4+key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 2:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI4_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI4_GROUP][key_index] = s_CustomBufferIn[4+key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 3:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI0_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI0_GROUP][key_index] = s_CustomBufferIn[4+key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		case 4:
			//-----------------------------------------------------------------------------------------------------------------------------------------
			for(key_index = 0; key_index < SPI1_TRANSFER_SIZE; key_index++)
			{
				g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[SPI1_GROUP][key_index] = s_CustomBufferIn[4+key_index];
			}
			//-----------------------------------------------------------------------------------------------------------------------------------------
		break;
		default:
			break;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_SET_ALLContinueRapidTrigger;													//命令类型
	s_CustomBufferIn[1]  = 0x01;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_GETFULLRapidTrigger
 * Description  	: 获取全局快速触发模式
 * END *******************************************************************************************************************/
void DriverAPP_GETFULLRapidTrigger(void)
{
	uint8_t Row_index;
	uint8_t key_index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	Row_index = s_CustomBufferOut[1];
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_FULLRapidTrigger;														//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint_OnOff;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}
/* ************************************************************************************************************************
 * Function Name	: DriverAPP_SETFULLRapidTrigger
 * Description  	:   连续快速触发模式。
 * END *******************************************************************************************************************/
void DriverAPP_SETFULLRapidTrigger(void)
{
	uint8_t Row_index;
	uint8_t key_index;
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Clear_buffer(s_CustomBufferIn,64);
	//-----------------------------------------------------------------------------------------------------------------------------------------
	/* out */
	g_DrivetData_TriggerPoint.ActualTriggerPoint_OnOff = s_CustomBufferOut[2];
	/* IN */
	s_CustomBufferIn[0]  = DriverAPP_Byte0_CMD_GET_FULLRapidTrigger;														//命令类型
	s_CustomBufferIn[1]  = 0x02;																							//命令长度
	s_CustomBufferIn[2]  = DriverAPP_Byte2_CMD_Success;																		//命令状态
	s_CustomBufferIn[3]  = g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[SPI0_GROUP][0];
	//-----------------------------------------------------------------------------------------------------------------------------------------
	Has_Key_Report_Interface3 = 1;
	//-----------------------------------------------------------------------------------------------------------------------------------------
}






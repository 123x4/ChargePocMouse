/*
 * testapp_communication.c
 *
 */
#include "usb.h"
#include "composite.h"
#include "hid_keyboard.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "dma_config.h"
#include "hall_spi.h"
#include "Flash_iap.h"
#include <testapp_communication.h>
#include <driverapp_communication.h>
#include <flash_zb25v.h>
/*******************************************************************************
 * define
 ******************************************************************************/
uint8_t Flag_Report_Trigger = 0;
uint8_t Flag_SET_Trigger = 0;
static uint8_t Flag_PacketSendNum_Segment = 0;
static uint8_t Packet_send_num = 0;
uint8_t Flag_TestAPP_PacketIndex = 0;
/*******************************************************************************
 * FUNCTION
 ******************************************************************************/
uint8_t TestApp_Check_Buffer_CRC(uint8_t* buff, uint8_t start_index, uint8_t end_index);
void Report_CRC_Error(void);
void TestAPP_Report_RawData(void);
void TestAPP_Report_SegmentAnalog(void);
void TestAPP_Report_SegmentData(void);
void TestAPP_Report_BaselineTop(void);
void TestAPP_Report_BaselineBottom(void);
void TestAPP_Report_Gain(void);
void TestAPP_Report_SelfCheck(void);
void TestAPP_Report_Triggerdata(void);
void TestAPP_Set_Triggerdata(void);
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: USB_DeviceHidCustomReportOutCallBack
 * Description  	:
 *
 * END ****************************************************************************************************************/
usb_status_t USB_DeviceHidCustomReportOutCallBack(void)
{
	uint8_t i, j, matrix_index, crc;

	unsigned char* Custom_buff_out = USB_DeviceHidCustomGetReportBuffOut();
	unsigned char* Custom_buff_in = USB_DeviceHidCustomGetReportBuffIn();
	//-----------------------------------------------------------------------------------------------------
	crc = TestApp_Check_Buffer_CRC(Custom_buff_out, 0, 62);
	//-----------------------------------------------------------------------------------------------------
	//if(crc != Custom_buf_out[63])
	//{
	//	Report_CRC_Error();
	//}
	//else
	{
		if((device_mode == MODE_NORMAL) || (device_mode ==  MODE_TEST) || (device_mode == MODE_FNTAB))
		{
			/*************************************************************************************************************/
			/* 测试软件功能 */
			if(Custom_buf_out[0] == TestAPP_Byte0_ID)
			{
				switch(Custom_buf_out[1])
				{
					//-----------------------------------------------------------------------------------------------------
					/* 设置成测试模式:04 04 */
					case TestAPP_Byte1_EnterTest:
						device_mode = MODE_TEST;
						InitParameter_FOR_TestMode();
						//-----------------------------------------------------------------------------------------------------
						Key_AllRelease();
						//-----------------------------------------------------------------------------------------------------
						break;
					//-----------------------------------------------------------------------------------------------------
					/* 从测试模式退出到正常模式:04 05 */
					case TestAPP_Byte1_EnterNormal:
						device_mode = MODE_NORMAL;
						InitParameter_FOR_TestMode();
						break;
					//-----------------------------------------------------------------------------------------------------
					/* 设置上报数据格式:04 17 Custom_buf_out[2] */
					case TestAPP_Byte1_ReportType:
						InitParameter_FOR_TestMode();
						TestAPP_ReportData_Type = Custom_buf_out[10];
						Flag_TestAPP_PacketIndex = 0;
						break;
					//-----------------------------------------------------------------------------------------------------
					/* 驱动进ISP */
					case Test_EnterISP:
						if(Flag_SET_Trigger)
						{
							Flag_SET_Trigger = 0;
							TestAPP_Set_Triggerdata();
						}
					break;
					//-----------------------------------------------------------------------------------------------------
					default:
						break;
					//-----------------------------------------------------------------------------------------------------
				}
			}
			else if(Custom_buf_out[0] == TestAPP_Byte0_TriggerID)
			{
				switch(Custom_buf_out[1])
				{
					//-----------------------------------------------------------------------------------------------------
					/* 获取触发值：00 41 */
					case TestAPP_Byte1_GETTrigger:
						Packet_send_num = Custom_buf_out[4] & 0x0F;
						Flag_Report_Trigger = 1;
						break;
					//-----------------------------------------------------------------------------------------------------
					/* 设置触发值：00 40 */
					case TestAPP_Byte1_SETTrigger:
						Packet_send_num = Custom_buf_out[4] & 0x0F;
						Flag_SET_Trigger = 1;
						break;
					//-----------------------------------------------------------------------------------------------------
					default:
						break;
					//-----------------------------------------------------------------------------------------------------
				}
			}
			/*************************************************************************************************************/
			/* VGN的驱动功能 */
			/* 获取设备信息 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_Info)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_Info;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_Info;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取设备是否在线 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_Online)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_Online;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_Online;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 切换六键和全键 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_NKeyRollover)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_NKeyRollover;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_NKeyRollover;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取单个按键键值 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_Keycode)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_Keycode;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_Keycode;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 设置单个按键键值 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_Keycode)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_Keycode;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_Keycode;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取全部按键键值 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_ALLKeycode)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_ALLKeycode;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_ALLKeycode;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 设置全部按键键值 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_ALLKeycode)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_ALLKeycode;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_ALLKeycode;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取WASD切换 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_WASD)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_WASD;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_WASD;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 设置WASD切换 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_WASD)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_WASD;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_WASD;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取Mac模式 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_Mac)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_Mac;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_Mac;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 设置Mac模式 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_Mac)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_Mac;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_Mac;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取win lock */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_WinLock)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_WinLock;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_WinLock;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 设置win lock */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_WinLock)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_WinLock;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_WinLock;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 保存flash */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SaveFlash)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SaveFlash;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SaveFlash;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 重置键盘 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_Reset)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_Reset;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_Reset;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取上报率 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_Polling)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_Polling;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_Polling;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置上报率 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_Polling)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_Polling;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_Polling;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取按键延时 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_Debounce)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_Debounce;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_Debounce;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置按键延时 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_Debounce)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_Debounce;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_Debounce;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取按键低延时状态 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_LowLatency)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_LowLatency;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_LowLatency;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置按键低延时状态 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_LowLatency)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_LowLatency;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_LowLatency;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取触底触发状态 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_BottomTriggerState)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_BottomTriggerState;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_BottomTriggerState;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置触底触发状态 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_BottomTriggerState)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_BottomTriggerState;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_BottomTriggerState;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取Profile配置文件 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_ProfileIndex)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_ProfileIndex;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_ProfileIndex;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置Profile配置文件 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_ProfileIndex)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_ProfileIndex;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_ProfileIndex;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取电量 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_Battery)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_Battery;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_Battery;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取单个按键的行程 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_SingleTrigger)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_SingleTrigger;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_SingleTrigger;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置单个按键的行程 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_SingleTrigger)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_SingleTrigger;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_SingleTrigger;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 设置全部按键的行程 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_ALLTrigger)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_ALLTrigger;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_ALLTrigger;
				Tick_DriverAPP_CNT = 0;
			}
			/* 获取全部按键的行程 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_ALLTrigger)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_ALLTrigger;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_ALLTrigger;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取单个快速触发状态 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_SingleTriggerStatus)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_SingleTriggerStatus;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_SingleTriggerStatus;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置单个快速触发状态 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_SingleTriggerStatus)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_SingleTriggerStatus;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_SingleTriggerStatus;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 设置灯光信息 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_LEDData)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_LEDData;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_LEDData;
				Tick_DriverAPP_CNT = 0;
			}
			/* 获取灯光信息 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_LEDData)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_LEDData;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_LEDData;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取自定义按键灯 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_CustomModeLEDData)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_CustomModeLEDData;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_CustomModeLEDData;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置自定义按键灯 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_CustomModeLEDData)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_CustomModeLEDData;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_CustomModeLEDData;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取所有按键行程 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_AllKeyTrigger)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_AllKeyTrigger;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_AllKeyTrigger;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置所有按键行程 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_AllKeyTrigger)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_AllKeyTrigger;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_AllKeyTrigger;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取连续快速触发模式 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_ContinueTriggerMode)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_ContinueTriggerMode;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_ContinueTriggerMode;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置连续快速触发模式 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_ContinueTriggerMode)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_ContinueTriggerMode;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_ContinueTriggerMode;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取DKS按键 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_DKS)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_DKS;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_DKS;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置DKS按键 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_DKS)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_DKS;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_DKS;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取MT按键 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_MT)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_MT;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_MT;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置MT按键 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_MT)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_MT;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_MT;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取TGL按键 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_TGL)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_TGL;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_TGL;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置TGL按键 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_TGL)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_TGL;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_TGL;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取macro按键 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_Macro)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_Macro;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_Macro;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置macro按键 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_Macro)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_Macro;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_Macro;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取全部快速触发状态 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_ALLRapidTrigger)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_ALLRapidTrigger;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_ALLRapidTrigger;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置全部快速触发状态 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_ALLRapidTrigger)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_ALLRapidTrigger;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_ALLRapidTrigger;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取全部连续快速触发状态 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_ALLContinueRapidTrigger)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_ALLContinueRapidTrigger;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_ALLContinueRapidTrigger;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置全部连续快速触发状态 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_ALLContinueRapidTrigger)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_ALLContinueRapidTrigger;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_ALLContinueRapidTrigger;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------
			/* 获取全局快速触发模式 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_GET_FULLRapidTrigger)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_GET_FULLRapidTrigger;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_GET_FULLRapidTrigger;
				Tick_DriverAPP_CNT = 0;
			}
			/* 设置全局快速触发模式 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_SET_FULLRapidTrigger)
			{
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_SET_FULLRapidTrigger;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_SET_FULLRapidTrigger;
				Tick_DriverAPP_CNT = 0;
			}
			//-------------------------------------------------------------------------------------------------------------

			//-------------------------------------------------------------------------------------------------------------
			/* 进入ISP模式 */
			else if(Custom_buf_out[0] == DriverAPP_Byte0_CMD_Update)
			{
				usb_echo("A\r\n");
				DriverAPP_ReceiveData = DriverAPP_Byte0_CMD_Update;
				DriverAPP_ReportData_Type = DriverAPP_Byte0_CMD_Update;
			}

			/*************************************************************************************************************/
		}
	}

}
/* FUNCTION **************************************************************************************************************
 *
 * Function Name	: Hall_set_led_current_max
 * Description  	: Configure LED0/LED1/LED2 max current.
 * Led_Max_Current_Level	:It is divided into for levels.
 *
 * END ****************************************************************************************************************/
void TestAPP_COMMUNICATION(void)
{
	//------------------------------------------------------------------------------------------
	//DMA_StartTransfer_Once_TESTAPP();
	//------------------------------------------------------------------------------------------
	switch(Custom_buf_out[1])
	{
		//----------------------------------------------------------------------------------------------------------
		/* 设置上报数据类型 */
		case TestAPP_Byte1_ReportType:
			TestAPP_CheckReportType();
			break;
		//----------------------------------------------------------------------------------------------------------
		/* 上报按下和抬起的触发值 */
		case TestAPP_Byte1_GETTrigger:
			if(Flag_Report_Trigger)
			{
				Flag_Report_Trigger = 0;
				TestAPP_Report_Triggerdata();
			}
		break;
		//----------------------------------------------------------------------------------------------------------
		/* 设置按下和抬起的触发值 */
		case TestAPP_Byte1_SETTrigger:
			if(Flag_SET_Trigger)
			{
				Flag_SET_Trigger = 0;
				TestAPP_Set_Triggerdata();
			}
		break;
		//----------------------------------------------------------------------------------------------------------
		default:
			break;
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_CheckReportType
 * Description  	: 判断需要上报什么数据格式.
 *
 * END ****************************************************************************************************************/
void TestAPP_CheckReportType(void)
{
	uint8_t i = 0;
	switch(TestAPP_ReportData_Type)
	{
		//----------------------------------------------------------------------------------------------------------
		case TestAPP_Byte10_RawData:
			TestAPP_Report_RawData();
		break;
		//----------------------------------------------------------------------------------------------------------
		case TestAPP_Byte10_Segment:
			TestAPP_Report_SegmentData();
		break;
		//----------------------------------------------------------------------------------------------------------
		case TestAPP_Byte10_SegmentAnalove:
			TestAPP_Report_SegmentAnalog();
		break;
		//----------------------------------------------------------------------------------------------------------
		case TestAPP_Byte10_Gain:
			TestAPP_Report_Gain();
		break;
		//----------------------------------------------------------------------------------------------------------
		case TestAPP_Byte10_BaselineTop:
			TestAPP_Report_BaselineTop();
		break;
		//----------------------------------------------------------------------------------------------------------
		case TestAPP_Byte10_BaselineBottom:
			TestAPP_Report_BaselineBottom();
		break;
		//----------------------------------------------------------------------------------------------------------
		case TestAPP_Byte10_Checkself:
			TestAPP_Report_SelfCheck();
		break;
		//----------------------------------------------------------------------------------------------------------
		default:
			break;
		//----------------------------------------------------------------------------------------------------------
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Report_RawData
 * Description  	: 上报采样到的原始值给测试软件.
 *
 * END ****************************************************************************************************************/
void TestAPP_Report_RawData(void)
{
	uint8_t i = 0;

	if(Packet_send_num < TestAPP_TotalPacket_NUM)
	{
		Clear_buffer(s_CustomBufferIn,64);
		s_CustomBufferIn[0] = TestAPP_Byte0_ID;
		s_CustomBufferIn[1] = 0x20;
		s_CustomBufferIn[3] = TestAPP_Byte10_RawData;
		switch(Packet_send_num)
		{
			//----------------------------------------------------------------------------------------------------------
			/* KeyBoardLayout_NormalCode[0][i]矩阵和SPI的组数不是一一对应，要自己对应修改，比如这里矩阵里面第0行对应的是SPI7这一组 */
			/* 需要注意的是一条上报给测试软件的hall数量是20个，21个会超过64byte,所以有21长度的链，分两包发,即加一个case和Packet_send_num相应增加 */
			case 0:
				for(i = 0; i < SPI6_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[0][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((DMA_SPI_RX_6_Buffer[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (DMA_SPI_RX_6_Buffer[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 1:
				for(i = 0; i < SPI3_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[1][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((DMA_SPI_RX_4_Buffer[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (DMA_SPI_RX_4_Buffer[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 2:
				for(i = 0; i < SPI4_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[2][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((DMA_SPI_RX_5_Buffer[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (DMA_SPI_RX_5_Buffer[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 3:
				for(i = 0; i < SPI0_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[3][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((DMA_SPI_RX_1_Buffer[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (DMA_SPI_RX_1_Buffer[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 4:
				for(i = 0; i < SPI1_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[4][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((DMA_SPI_RX_2_Buffer[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (DMA_SPI_RX_2_Buffer[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			default:
			break;
			//----------------------------------------------------------------------------------------------------------
		}
		//--------------------------------------------------------------------------------------------------------------
		s_CustomBufferIn[63] = TestApp_Check_Buffer_CRC(s_CustomBufferIn, 0, 62);
		//--------------------------------------------------------------------------------------------------------------
		/* 每一包都先发码值，然后再发数据，码值和数据的包的Index保持一样，用来判断是不是同一组数据。 */
		Packet_send_num++;
		Has_Key_Report_Interface3 = 1;
		//--------------------------------------------------------------------------------------------------------------
	}
	else
	{
		Packet_send_num = 0;
		Sample_Hall_Once = 1;
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Report_SegmentData
 * Description  	: 上报分段值.
 *
 * END ****************************************************************************************************************/
void TestAPP_Report_SegmentData(void)
{
	uint8_t i = 0;

	if(Packet_send_num < TestAPP_TotalPacket_NUM)
	{
		Clear_buffer(s_CustomBufferIn,64);
		s_CustomBufferIn[0] = TestAPP_Byte0_ID;
		s_CustomBufferIn[1] = 0x20;
		s_CustomBufferIn[3] = TestAPP_Byte10_Segment;
		switch(Packet_send_num)
		{
			//----------------------------------------------------------------------------------------------------------
			/* KeyBoardLayout_NormalCode[0][i]矩阵和SPI的组数不是一一对应，要自己对应修改，比如这里矩阵里面第0行对应的是SPI7这一组 */
			/* 需要注意的是一条上报给测试软件的hall数量是20个，21个会超过64byte,所以有21长度的链，分两包发,即加一个case和Packet_send_num相应增加 */
			case 0:
				for(i = 0; i < SPI7_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[0][i] >> 8;
					s_CustomBufferIn[i*2+4] = Hall_Node_SPI7[i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 1:
				for(i = 0; i < SPI3_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[1][i] >> 8;
					s_CustomBufferIn[i*2+4] = Hall_Node_SPI3[i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 2:
				for(i = 0; i < SPI4_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[2][i] >> 8;
					s_CustomBufferIn[i*2+4] = Hall_Node_SPI4[i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 3:
				for(i = 0; i < SPI0_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[3][i] >> 8;
					s_CustomBufferIn[i*2+4] = Hall_Node_SPI0[i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 4:
				for(i = 0; i < SPI1_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[4][i] >> 8;
					s_CustomBufferIn[i*2+4] = Hall_Node_SPI1[i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			default:
			break;
			//----------------------------------------------------------------------------------------------------------
		}
		//--------------------------------------------------------------------------------------------------------------
		/* 每一包都先发码值，然后再发数据，码值和数据的包的Index保持一样，用来判断是不是同一组数据。 */
		Packet_send_num++;
		Has_Key_Report_Interface3 = 1;
		//--------------------------------------------------------------------------------------------------------------
	}
	else
	{
		Packet_send_num = 0;
		Sample_Hall_Once = 1;
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Report_SegmentAnalog
 * Description  	: 上报255分段值.
 *
 * END ****************************************************************************************************************/
void TestAPP_Report_SegmentAnalog(void)
{
	uint8_t i = 0;

	if(Packet_send_num < TestAPP_TotalPacket_NUM)
	{
		Clear_buffer(s_CustomBufferIn,64);
		s_CustomBufferIn[0] = TestAPP_Byte0_ID;
		s_CustomBufferIn[1] = 0x20;
		s_CustomBufferIn[3] = TestAPP_Byte10_SegmentAnalove;
		switch(Packet_send_num)
		{
			//----------------------------------------------------------------------------------------------------------
			/* KeyBoardLayout_NormalCode[0][i]矩阵和SPI的组数不是一一对应，要自己对应修改，比如这里矩阵里面第0行对应的是SPI7这一组 */
			/* 需要注意的是一条上报给测试软件的hall数量是20个，21个会超过64byte,所以有21长度的链，分两包发,即加一个case和Packet_send_num相应增加 */
			case 0:
				for(i = 0; i < SPI7_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[0][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Hall_Node_SPI7[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Hall_Node_SPI7[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 1:
				for(i = 0; i < SPI3_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[1][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Hall_Node_SPI3[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Hall_Node_SPI3[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 2:
				for(i = 0; i < SPI4_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[2][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Hall_Node_SPI4[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Hall_Node_SPI4[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 3:
				for(i = 0; i < SPI0_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[3][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Hall_Node_SPI0[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Hall_Node_SPI0[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 4:
				for(i = 0; i < SPI1_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[4][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Hall_Node_SPI1[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Hall_Node_SPI1[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			default:
			break;
			//----------------------------------------------------------------------------------------------------------
		}
		//--------------------------------------------------------------------------------------------------------------
		/* 每一包都先发码值，然后再发数据，码值和数据的包的Index保持一样，用来判断是不是同一组数据。 */
		Packet_send_num++;
		Has_Key_Report_Interface3 = 1;
		//--------------------------------------------------------------------------------------------------------------
	}
	else
	{
		Packet_send_num = 0;
		Sample_Hall_Once = 1;
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Report_Triggerdata
 * Description  	: Send Press and break trigger to test APP.
 *
 * END ****************************************************************************************************************/
void TestAPP_Report_Triggerdata(void)
{
	uint8_t i = 0;

	if(Packet_send_num < 6)
	{
		//-------------------------------------------------------------------------------------------------------------
		Packet_send_num = Custom_buf_out[4] & 0x0F;
		//-------------------------------------------------------------------------------------------------------------
		Clear_buffer(s_CustomBufferIn,64);
		s_CustomBufferIn[0] = TestAPP_Byte0_TriggerID;
		s_CustomBufferIn[1] = TestAPP_Byte1_GETTrigger;
		s_CustomBufferIn[3] = 0x3B;
		s_CustomBufferIn[4] = (0x50) | (Packet_send_num);
		//-------------------------------------------------------------------------------------------------------------
		switch(Packet_send_num)
		{
			//----------------------------------------------------------------------------------------------------------
			/* KeyBoardLayout_NormalCode[0][i]矩阵和SPI的组数不是一一对应，要自己对应修改，比如这里矩阵里面第0行对应的是SPI7这一组 */
			/* 需要注意的是一条上报给测试软件的hall数量是20个，21个会超过64byte,所以有21长度的链，分两包发,即加一个case和Packet_send_num相应增加 */
			case 0:
				for(i = 0; i < SPI7_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+5] = KeyBoardLayout_NormalCode[0][i] >> 8;
					s_CustomBufferIn[i*3+6] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][i];
					s_CustomBufferIn[i*3+7] = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 1:
				for(i = 0; i < SPI3_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+5] = KeyBoardLayout_NormalCode[1][i] >> 8;
					s_CustomBufferIn[i*3+6] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][i];
					s_CustomBufferIn[i*3+7] = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[1][i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 2:
				for(i = 0; i < SPI4_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+5] = KeyBoardLayout_NormalCode[2][i] >> 8;
					s_CustomBufferIn[i*3+6] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][i];
					s_CustomBufferIn[i*3+7] = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[2][i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 3:
				for(i = 0; i < SPI0_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+5] = KeyBoardLayout_NormalCode[3][i] >> 8;
					s_CustomBufferIn[i*3+6] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][i];
					s_CustomBufferIn[i*3+7] = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[3][i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 4:
				for(i = 0; i < SPI1_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+5] = KeyBoardLayout_NormalCode[4][i] >> 8;
					s_CustomBufferIn[i*3+6] = g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][i];
					s_CustomBufferIn[i*3+7] = g_DrivetData_PBPoint.Segment_SET_Trigger_Break[4][i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			default:
			break;
			//----------------------------------------------------------------------------------------------------------
		}
		//--------------------------------------------------------------------------------------------------------------
		s_CustomBufferIn[63] = TestApp_Check_Buffer_CRC(s_CustomBufferIn, 0, 62);
		//--------------------------------------------------------------------------------------------------------------
		/* 每一包都先发码值，然后再发数据，码值和数据的包的Index保持一样，用来判断是不是同一组数据。 */
		Has_Key_Report_Interface3 = 1;
		//--------------------------------------------------------------------------------------------------------------
	}
	else
	{
		Packet_send_num = 0;
		Sample_Hall_Once = 1;
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Set_Triggerdata
 * Description  	: Set Press and break trigger from test APP.
 *
 * END ****************************************************************************************************************/
void TestAPP_Set_Triggerdata(void)
{
	uint8_t i = 0;

	if(Packet_send_num < 6)
	{
		//-------------------------------------------------------------------------------------------------------------
		Packet_send_num = Custom_buf_out[4] & 0x0F;
		//-------------------------------------------------------------------------------------------------------------
		Clear_buffer(s_CustomBufferIn,64);
		s_CustomBufferIn[0] = TestAPP_Byte0_TriggerID;
		s_CustomBufferIn[1] = TestAPP_Byte1_SETTrigger;
		s_CustomBufferIn[3] = 0;
		s_CustomBufferIn[4] = (0x50) | (Packet_send_num);
		//-------------------------------------------------------------------------------------------------------------
		switch(Packet_send_num)
		{
			//----------------------------------------------------------------------------------------------------------
			/* 第一条链的trigger */
			/* 设置触发值最长只能放19个sensor，多的再加一包 */
			case 0:
				for(i = 0; i < SPI7_TRANSFER_SIZE; i++)
				{
					g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][i] = Custom_buf_out[i*3+6];
					g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][i] = Custom_buf_out[i*3+7];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			/* 第二条链的trigger */
			/* 设置触发值最长只能放19个sensor，多的再加一包 */
			case 1:
				for(i = 0; i < SPI3_TRANSFER_SIZE; i++)
				{
					g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][i] = Custom_buf_out[i*3+6];
					g_DrivetData_PBPoint.Segment_SET_Trigger_Break[1][i] = Custom_buf_out[i*3+7];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 2:
				for(i = 0; i < SPI4_TRANSFER_SIZE; i++)
				{
					g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][i] = Custom_buf_out[i*3+6];
					g_DrivetData_PBPoint.Segment_SET_Trigger_Break[2][i] = Custom_buf_out[i*3+7];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 3:
				for(i = 0; i < SPI0_TRANSFER_SIZE; i++)
				{
					g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][i] = Custom_buf_out[i*3+6];
					g_DrivetData_PBPoint.Segment_SET_Trigger_Break[3][i] = Custom_buf_out[i*3+7];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			/* 第三条链的trigger */
			/* 设置触发值最长只能放19个sensor，多的再加一包 */
			case 4:
				for(i = 0; i < SPI1_TRANSFER_SIZE; i++)
				{
					g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][i] = Custom_buf_out[i*3+6];
					g_DrivetData_PBPoint.Segment_SET_Trigger_Break[4][i] = Custom_buf_out[i*3+7];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			default:
			break;
			//----------------------------------------------------------------------------------------------------------
		}
		//--------------------------------------------------------------------------------------------------------------
		s_CustomBufferIn[63] = TestApp_Check_Buffer_CRC(s_CustomBufferIn, 0, 62);
		//--------------------------------------------------------------------------------------------------------------
		/* 每一包都先发码值，然后再发数据，码值和数据的包的Index保持一样，用来判断是不是同一组数据。 */
		Has_Key_Report_Interface3 = 1;
		//--------------------------------------------------------------------------------------------------------------
		/* 最后一包保存数据 */
		if(Packet_send_num == 4)
		{
			/* 设置完触发值后自动退出测试模式 */
			device_mode = MODE_NORMAL;
			InitParameter_FOR_TestMode();
			//--------------------------------------------------------------------------------------------------------------
			/* wait 10ms and save Flash */
			Flag_NeedUpdate_FlashZB25V |= FLASHZB25V_PBPoint;
			Tick_UpdateMCUFlash = 0;
			//--------------------------------------------------------------------------------------------------------------
		}
	}
	else
	{
		Packet_send_num = 0;
		Sample_Hall_Once = 1;
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Report_BaselineTop
 * Description  	: 上报顶部Baseline
 *
 * END ****************************************************************************************************************/
void TestAPP_Report_BaselineTop(void)
{
	uint8_t i = 0;

	if(Packet_send_num < TestAPP_TotalPacket_NUM)
	{
		Clear_buffer(s_CustomBufferIn,64);
		s_CustomBufferIn[0] = TestAPP_Byte0_ID;
		s_CustomBufferIn[1] = 0x20;
		s_CustomBufferIn[3] = TestAPP_Byte10_BaselineTop;
		switch(Packet_send_num)
		{
			//----------------------------------------------------------------------------------------------------------
			/* KeyBoardLayout_NormalCode[0][i]矩阵和SPI的组数不是一一对应，要自己对应修改，比如这里矩阵里面第0行对应的是SPI7这一组 */
			/* 需要注意的是一条上报给测试软件的hall数量是20个，21个会超过64byte,所以有21长度的链，分两包发,即加一个case和Packet_send_num相应增加 */
			case 0:
				for(i = 0; i < SPI7_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[0][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Baseline_Top_SPI7[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Baseline_Top_SPI7[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 1:
				for(i = 0; i < SPI3_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[1][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Baseline_Top_SPI3[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Baseline_Top_SPI3[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 2:
				for(i = 0; i < SPI4_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[2][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Baseline_Top_SPI4[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Baseline_Top_SPI4[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 3:
				for(i = 0; i < SPI0_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[3][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Baseline_Top_SPI0[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Baseline_Top_SPI0[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 4:
				for(i = 0; i < SPI1_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[4][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Baseline_Top_SPI1[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Baseline_Top_SPI1[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			default:
			break;
			//----------------------------------------------------------------------------------------------------------
		}
		//--------------------------------------------------------------------------------------------------------------
		/* 每一包都先发码值，然后再发数据，码值和数据的包的Index保持一样，用来判断是不是同一组数据。 */
		Packet_send_num++;
		Has_Key_Report_Interface3 = 1;
		//--------------------------------------------------------------------------------------------------------------
	}
	else
	{
		Packet_send_num = 0;
		Sample_Hall_Once = 1;
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Report_BaselineBottom
 * Description  	: 上报底部Baseline
 *
 * END ****************************************************************************************************************/
void TestAPP_Report_BaselineBottom(void)
{

	uint8_t i = 0;

	if(Packet_send_num < TestAPP_TotalPacket_NUM)
	{
		Clear_buffer(s_CustomBufferIn,64);
		s_CustomBufferIn[0] = TestAPP_Byte0_ID;
		s_CustomBufferIn[1] = 0x20;
		s_CustomBufferIn[3] = TestAPP_Byte10_BaselineBottom;
		switch(Packet_send_num)
		{
			//----------------------------------------------------------------------------------------------------------
			/* KeyBoardLayout_NormalCode[0][i]矩阵和SPI的组数不是一一对应，要自己对应修改，比如这里矩阵里面第0行对应的是SPI7这一组 */
			/* 需要注意的是一条上报给测试软件的hall数量是20个，21个会超过64byte,所以有21长度的链，分两包发,即加一个case和Packet_send_num相应增加 */
			case 0:
				for(i = 0; i < SPI7_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[0][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Baseline_Bottom_SPI7[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Baseline_Bottom_SPI7[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 1:
				for(i = 0; i < SPI3_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[1][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Baseline_Bottom_SPI3[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Baseline_Bottom_SPI3[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 2:
				for(i = 0; i < SPI4_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[2][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Baseline_Bottom_SPI4[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Baseline_Bottom_SPI4[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 3:
				for(i = 0; i < SPI0_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[3][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Baseline_Bottom_SPI0[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Baseline_Bottom_SPI0[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 4:
				for(i = 0; i < SPI1_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*3+3] = KeyBoardLayout_NormalCode[4][i] >> 8;
					s_CustomBufferIn[i*3+4] = ((Baseline_Bottom_SPI1[i] & 0x3F00) >> 8);
					s_CustomBufferIn[i*3+5] = (Baseline_Bottom_SPI1[i] & 0x00FF);
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			default:
			break;
			//----------------------------------------------------------------------------------------------------------
		}
		//--------------------------------------------------------------------------------------------------------------
		/* 每一包都先发码值，然后再发数据，码值和数据的包的Index保持一样，用来判断是不是同一组数据。 */
		Packet_send_num++;
		Has_Key_Report_Interface3 = 1;
		//--------------------------------------------------------------------------------------------------------------
	}
	else
	{
		Packet_send_num = 0;
		Sample_Hall_Once = 1;
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Report_Gain
 * Description  	: 上报增益.
 *
 * END ****************************************************************************************************************/
void TestAPP_Report_Gain(void)
{
	uint8_t i = 0;

	if(Packet_send_num < TestAPP_TotalPacket_NUM)
	{
		Clear_buffer(s_CustomBufferIn,64);
		s_CustomBufferIn[0] = TestAPP_Byte0_ID;
		s_CustomBufferIn[1] = 0x20;
		s_CustomBufferIn[3] = TestAPP_Byte10_Gain;
		switch(Packet_send_num)
		{
			//----------------------------------------------------------------------------------------------------------
			/* KeyBoardLayout_NormalCode[0][i]矩阵和SPI的组数不是一一对应，要自己对应修改，比如这里矩阵里面第0行对应的是SPI7这一组 */
			/* 需要注意的是一条上报给测试软件的hall数量是20个，21个会超过64byte,所以有21长度的链，分两包发,即加一个case和Packet_send_num相应增加 */
			case 0:
				for(i = 0; i < SPI7_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[0][i] >> 8;
					s_CustomBufferIn[i*2+4] = g_variableData.Gain_Buffer[SPI6_GROUP][i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 1:
				for(i = 0; i < SPI3_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[1][i] >> 8;
					s_CustomBufferIn[i*2+4] = g_variableData.Gain_Buffer[SPI3_GROUP][i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 2:
				for(i = 0; i < SPI4_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[2][i] >> 8;
					s_CustomBufferIn[i*2+4] = g_variableData.Gain_Buffer[SPI4_GROUP][i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 3:
				for(i = 0; i < SPI0_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[3][i] >> 8;
					s_CustomBufferIn[i*2+4] = g_variableData.Gain_Buffer[SPI0_GROUP][i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 4:
				for(i = 0; i < SPI1_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[4][i] >> 8;
					s_CustomBufferIn[i*2+4] = g_variableData.Gain_Buffer[SPI1_GROUP][i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			default:
			break;
			//----------------------------------------------------------------------------------------------------------
		}
		//--------------------------------------------------------------------------------------------------------------
		/* 每一包都先发码值，然后再发数据，码值和数据的包的Index保持一样，用来判断是不是同一组数据。 */
		Packet_send_num++;
		Has_Key_Report_Interface3 = 1;
		//--------------------------------------------------------------------------------------------------------------
	}
	else
	{
		Packet_send_num = 0;
		Sample_Hall_Once = 1;
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: TestAPP_Report_SelfCheck
 * Description  	: 上报自检.
 *
 * END ****************************************************************************************************************/
void TestAPP_Report_SelfCheck(void)
{
	uint8_t i = 0;

	if(Packet_send_num < TestAPP_TotalPacket_NUM)
	{
		Clear_buffer(s_CustomBufferIn,64);
		s_CustomBufferIn[0] = TestAPP_Byte0_ID;
		s_CustomBufferIn[1] = 0x20;
		s_CustomBufferIn[3] = TestAPP_Byte10_Checkself;
		switch(Packet_send_num)
		{
			//----------------------------------------------------------------------------------------------------------
			/* KeyBoardLayout_NormalCode[0][i]矩阵和SPI的组数不是一一对应，要自己对应修改，比如这里矩阵里面第0行对应的是SPI7这一组 */
			/* 需要注意的是一条上报给测试软件的hall数量是20个，21个会超过64byte,所以有21长度的链，分两包发,即加一个case和Packet_send_num相应增加 */
			case 0:
				for(i = 0; i < SPI7_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[0][i] >> 8;
					s_CustomBufferIn[i*2+4] = DMA_SPI_RX_5_Buffer[i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 1:
				for(i = 0; i < SPI3_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[1][i] >> 8;
					s_CustomBufferIn[i*2+4] = DMA_SPI_RX_3_Buffer[i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 2:
				for(i = 0; i < SPI4_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[2][i] >> 8;
					s_CustomBufferIn[i*2+4] = DMA_SPI_RX_4_Buffer[i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 3:
				for(i = 0; i < SPI0_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[3][i] >> 8;
					s_CustomBufferIn[i*2+4] = DMA_SPI_RX_1_Buffer[i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			case 4:
				for(i = 0; i < SPI1_TRANSFER_SIZE; i++)
				{
					s_CustomBufferIn[i*2+3] = KeyBoardLayout_NormalCode[4][i] >> 8;
					s_CustomBufferIn[i*2+4] = DMA_SPI_RX_2_Buffer[i];
				}
			break;
			//----------------------------------------------------------------------------------------------------------
			default:
			break;
			//----------------------------------------------------------------------------------------------------------
		}
		//--------------------------------------------------------------------------------------------------------------
		/* 每一包都先发码值，然后再发数据，码值和数据的包的Index保持一样，用来判断是不是同一组数据。 */
		Packet_send_num++;
		Has_Key_Report_Interface3 = 1;
		//--------------------------------------------------------------------------------------------------------------
	}
	else
	{
		Packet_send_num = 0;
		Sample_Hall_Once = 1;
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Clear_buffer
 * Description  	: Clear buffer data.
 *
 * END ****************************************************************************************************************/
void Clear_buffer(uint8_t *buffer, uint8_t len)
{
	uint8_t i = 0;
	for(i = 0; i < len; i++)
	{
		buffer[i] = 0;
	}
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Clear_buffer
 * Description  	: Clear buffer data.
 *
 * END ****************************************************************************************************************/
uint8_t TestApp_Check_Buffer_CRC(uint8_t* buff, uint8_t start_index, uint8_t end_index)
{
	uint8_t ii;
	uint8_t crc = 0;

	for(ii = start_index; ii <= end_index; ii++)
	{
		crc += buff[ii];
	}

	crc = 0x55 - crc;

	return crc;
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Report_CRC_Error
 * Description  	:
 *
 * END ****************************************************************************************************************/
void Report_CRC_Error(void)
{
	Clear_buffer(s_CustomBufferIn,64);
	s_CustomBufferIn[0] = TestAPP_CRCErr_ID;
	s_CustomBufferIn[2] = s_CustomBufferOut[1];
	s_CustomBufferIn[63] = TestApp_Check_Buffer_CRC(s_CustomBufferOut, 0, 62);
	Clear_buffer(s_CustomBufferOut,64);
	Has_Key_Report_Interface3 = 1;
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: InitParameter_FOR_TestMode_
 * Description  	: Before enter test mode, some parameters should be initiated.
 *
 * END ****************************************************************************************************************/
void InitParameter_FOR_TestMode(void)
{
	//-----------------------------------------------------------------------------------------------------
	Tick_10ms = 0;
	Sample_Hall_Once = 1;
	Packet_send_num = 0;
	Flag_PacketSendNum_Segment = 0;
	TestAPP_ReportData_Type = TestAPP_Byte10_Segment;
}


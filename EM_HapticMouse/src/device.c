#include "device.h"
#include "../inc/GPIO.h"
#include "hiduser.h"
#include "usbcore.h"
#include "../inc/OSC.h"
#include "usbhw.h"
#include "../inc/optical_3325.h"
#include "../inc/scroll.h"
#include "../inc/Timer.h"
#include "../inc/ADC.h"
#include "../inc/UART.h"
#include "../inc/SPI.h"
#include "../inc/I2C.h"
#include "../inc/led_driver.h"
#include "led1.h"
#include "led2.h"
#include "color_cycle.h"
#include "led_effect.h"
#include "led_effect_group_code.h"
#include "custom.h"
#include "gloab.h"
#include "button.h"
#include "gtech_flash.h"
#include "operation_flash.h"
#include "pc_communicate.h"
#include "suspend.h"
#include "wnDrv_Pmu.h"
#include "eeprom.h"
#include "WWDT.h"
#include "Production_line_test.h"
//#include "usbcfg.h"
//#include "my_i2c.h"
#include "usbdesc.h"
#include "my_debug.h"
#include "IAP_CMD.h"
#include "my_electromagnet.h"
#include "my_protocol_mouse.h"
#include "my_board.h"
#include "PAW3950.h"
#include "my_adc.h"
#include "my_charger.h"


//static MS_Key_statet Chk_Button_state = No_Button;
uint8_t Debounce_cnt;
//extern int8_t mouse_buttons_packet[MOUSE_IN_REPORT_SIZE];
uint8_t KeyFlag ;
extern uint8_t one_ms_cycle;
uint8_t previous_buttons=0;	
uint8_t deviceEvent;
uint8_t data_type;
UINT32 wheel_button_status;
unsigned int LED_Mode ;
W_n_B_CONVERT x_value_delta,y_value_delta;
extern uint8_t tm_usb_sof;
extern BOOL SaveDPIflag;
BOOL DPI_delayflag=FALSE;
BOOL Send_SensorData_F=FALSE;
BOOL Send_Button_F=FALSE;
BOOL Profile_LED_flag=FALSE;
//extern Profile_Mode_Stru profile_mode;
//extern uint16_t profile_timer;
//BOOL DPI_flag = FALSE ;
//uint8_t DPI_count = 0;
UINT16 de_determineUsbAct;
UINT8 device_ReportRata;
UINT16 dpitimerdelay=0;
UINT8 optical_scan_cycle;
UINT16 LED_off_cycle;
UINT8 LED_off_cnt;
UINT16 timer_1s;
LR_WHEEL_STRU device_lrWheel;
DOUBLE_CLICK_STRU device_doubleClick;
UINT8 mouse_temp[13] = {0,0,0,0,0,0,0,0,3,0,0,0,0};
UINT8 keyboard_temp[8] = {0,0,0,0,0,0,0,0};
UINT8 mouse_multimedia_temp[3] = {0,0,0};

UINT8 mouseReport_back[8] = {MOUSE_ID,0,0,0,0,0,0,0};
UINT8 keyboardReport_back[8] = {0,0,0,0,0,0,0,0};
UINT8 mouseMultimediaReport_back[3] = {0,0,0};
REPORT_POINTER report_data={mouse_temp,keyboard_temp,mouse_multimedia_temp};
uint8_t Bypass=0;

extern uint8_t  sus_res;
void Device_DoubleClick_Start(void)
{
	device_doubleClick.press = TRUE;
	device_doubleClick.clickCnt = 0;
}

void Device_DoubleClick_Scan(void)
{
	static UINT8 doubleTime = 0;

	if (device_doubleClick.press == TRUE)
	{
		doubleTime++;
		if (doubleTime > 50)
		{
			doubleTime = 0;
		}else
		{
			return;
		}

		device_doubleClick.clickCnt++;

		if(device_doubleClick.clickCnt % 2)
		{
			device_doubleClick.data = 0x01;
		}else
		{
			device_doubleClick.data = 0x00;
		}

		DEVICE_POST_EVENT(DOUBLE_CLICK_EVENT);

		if (device_doubleClick.clickCnt >= 4)
		{
			device_doubleClick.press = FALSE;
		}
	}
}

void Device_DoubleClick_GetData(UINT8 *data)
{
	if (device_doubleClick.data == 0x01)//press
	{
		*data |= 0x01;	
	}else
	{
		*data &= ~0x01;	
	}
}

void Device_LrWheel_Start(UINT8 data)
{
	device_lrWheel.press = TRUE;
	device_lrWheel.firstCnt = 10;
	device_lrWheel.data = data;
}

void Device_LrWheel_Scan(void)
{
	static UINT8 lrWheel_time = 0;

	//advice 65ms Perform one count
	if (device_lrWheel.press)
	{
		lrWheel_time++;
		if (lrWheel_time > 65)
		{
			lrWheel_time = 0;
		}else
		{
			return;
		}

		if (device_lrWheel.firstCnt) device_lrWheel.firstCnt--;
		if (!device_lrWheel.firstCnt)
		{
			DEVICE_POST_EVENT(LR_WHEEL_EVENT);
		}
	}
}

UINT8 Device_LrWheel_GetData(void)
{
	UINT8 result = 0;

	result = device_lrWheel.data;


	return result;
}

void Device_LrWheel_Stop(void)
{
	device_lrWheel.press = FALSE;
}

Zoom_Out_In_STRU de_zoom;

void Device_Zoom_OutIn_Start(UINT8 type)
{
	de_zoom.type = type;

	de_zoom.start = TRUE;
	de_zoom.first = TRUE;
	de_zoom.phase = Zoom_Phase_3;
	de_zoom.cycle = 0;
	de_zoom.frame = 500;
}

void Device_Zoom_OutIn_Stop(void)
{
	de_zoom.start = FALSE;
}


void Device_Zoom_OutIn_Scan(void)
{
	if (!de_zoom.start && !de_zoom.phase)
	{
		return;
	}

	if (de_zoom.cycle)de_zoom.cycle--;
	
	if (!de_zoom.cycle)
	{
		DEVICE_POST_EVENT(Zoom_Out_In_Event);
	}
}

UINT8 Device_Zoom_OutIn_GetData(UINT8 *mouseBuf,UINT8 *keyboardBuf)
{				
	UINT8 type = 0;

	if (!de_zoom.start &&  !de_zoom.first)
	{
		keyboardBuf[0] &= ~0x01;

		type = KEYBOARD_KEY_REPORT;

		de_zoom.phase = Zoom_Phase_0;
	}

	if (de_zoom.phase == Zoom_Phase_3)
	{
		keyboardBuf[0] |= 0x01;	

		de_zoom.phase = Zoom_Phase_2;

		de_zoom.cycle = 10;

		type = KEYBOARD_KEY_REPORT;

	}else if (de_zoom.phase == Zoom_Phase_2)
	{
		*mouseBuf = de_zoom.type;	

		type = MOUSE_KEY_REPORT;
		
		de_zoom.first = FALSE;

		de_zoom.cycle  = de_zoom.frame;

	}

	return type;
}

#if 0
void DPI_event()
{
	
	if(DPI_flag == TRUE ) {
		DPI_count ++;
		if(DPI_count >= 3)  DPI_count = 0;
		
		switch (DPI_count )
		{
			case 0 :		
				OP_3389_Set_DPI(0x140,0x140);
//				Optical_Set_DPI(0x77,0x77);
			break;
			case 1:
				OP_3389_Set_DPI(0x77,0x77);
//				Optical_Set_DPI(0x20,0x20);
			break;
			case 2:
				OP_3389_Set_DPI(0x20,0x20);
//				Optical_Set_DPI(0x01,0x01);
			break;
			case 3:

			default :
				break;
		 }
		}
		DPI_flag = FALSE ;
}



void Mapp_StaticMachineEvent1(void)
{
	switch(g_StaticMachineEvent1Type)
	{
		case SM_EVENT1_MOUSE_MONITOR:
		 	if(nsAPI_USB_EP_Status(HID_EP1_IN))
			;
				//S_Sensor_MotionCheck();
			g_StaticMachineEvent1Type = SM_EVENT1_KEYBOARD_MONITOR;
			
		break;
		case SM_EVENT1_KEYBOARD_MONITOR:
//			nsAPI_GPIO_ButtonEventMointor();
//			nsAPI_WheelEventMointor();		  
      nsAPI_Sensor_EventMointor();	  
			g_StaticMachineEvent1Type = SM_EVENT1_MOUSE_MONITOR;
		break;
		case SM_EVENT1_IDLE:
		default:
		break;
	}
}


void nsAPI_SensorEventMointor(void)
{
#if REMOTE_WAKEUP_ENABLE
	if ( rSuspendFlag == 1 ) 
	{
		rSuspendFlag = 0;
		g_Main_Setting.SensorReset = 1;
		g_Repot_Process.Mouse_Report_In = 0;
		return;
  	}
	#endif
	#if (SUPPORT_MAC_BOOT_AND_DOS_MODE)
	if (g_USBCore_Setting.EP0_DosMode)
	{
		if(g_Sensor_Data.Changed == 1)
		{
			g_Main_Setting.MouseInReport[GIdx1] = 0;//g_Sensor_Data.X1;
			g_Main_Setting.MouseInReport[GIdx2] = 0;//g_Sensor_Data.Y1;
			g_Main_Setting.MouseInReport[GIdx3] = 0;
			g_Main_Setting.MouseInReport[GIdx4] = 0;
		}
	}
	#endif //SUPPORT_MAC_BOOT_AND_DOS_MODE
	{
      g_Main_Setting.MouseInReport[GIdx0] = 0x01 ;
      g_Main_Setting.MouseInReport[GIdx2] = x_value_delta.b.lo ;
			g_Main_Setting.MouseInReport[GIdx3] = x_value_delta.b.hi;
			g_Main_Setting.MouseInReport[GIdx4] = y_value_delta.b.lo;
			g_Main_Setting.MouseInReport[GIdx5] = y_value_delta.b.hi;
		  

	}
	if(nsAPI_USB_EP_Status(HID_EP1_IN)  )	//if((g_Main_Setting.EPInOutReq & EP1_IN) == EP1_IN)  //modify 20120808
	{	
		if( Optical_Scan() == true ){
			
			nsAPI_USB_WriteEP(HID_EP1_IN, &g_Main_Setting.MouseInReport[0], MOUSE_IN_REPORT_SIZE);
			nsAPI_Main_Clr_EP1Event();
			g_Main_Setting.EPInOutReq &= ~EP1_IN;
			g_Repot_Process.Mouse_Report_In = 0;
			g_Sensor_Data.Changed = 0;
		}
	}

}
#endif

void Device_Manually_Enter_RestoreDefault(void)
{
	UINT8 cnt = 10;
	//BOOL trg = FALSE;
	//RGB_STRU white_rgb = {255,255,255};
//	RGB_STRU none_rgb = {0,0,0};
	//RGB_STRU tmp_rgb;

	while ((Button_GetState() & (RIGHT_BUTTON_REPORT_BIT_MASK | K5_BUTTON_REPORT_BIT_MASK))
		== (RIGHT_BUTTON_REPORT_BIT_MASK | K5_BUTTON_REPORT_BIT_MASK))
	{
		//trg = !trg;

//		if (trg)
//		{
//			tmp_rgb = white_rgb;
//		}else
//		{
//			tmp_rgb = none_rgb;
//		}
		
//		LedEffect_Put_StaticColor(tmp_rgb,FALSE,LED_ALL_SEND);


//		WATCH_DOG_CLEAR;
		TimerDelayMsec(200);
		if (cnt)cnt--;
		if (!cnt)
		{
			///Custom_RestoreAll_Default();
			Custom_Paramter_Init();
//			LedEffect_Put_StaticColor(none_rgb,FALSE,LED_ALL_SEND);
			break;
		}
	}

//	LedEffect_Put_StaticColor(none_rgb,FALSE,LED_ALL_SEND);
}

//void Device_Manually_Enter_Bootloader(void)
//{
//	UINT8 cnt = 10;

//	while ((Button_GetState() & (RIGHT_BUTTON_REPORT_BIT_MASK | K4_BUTTON_REPORT_BIT_MASK))
//		 == (RIGHT_BUTTON_REPORT_BIT_MASK | K4_BUTTON_REPORT_BIT_MASK))
//	{
////		WATCH_DOG_CLEAR;
//		TimerDelayMsec(200);
//		if (cnt)cnt--;
//		if (!cnt)
//		{
//			Bootloadable_Load();
//		}
//	}
//}



void Device_Init(void)
{
//	TimerDelayMsec(100);
//	nsAPI_System_Init();
	wwdt_config(0xFFFF);
	InitSystemOSC( MAIN_SEL , PSEL , MSEL , SYS_AHB_DIV_VAL) ;
	InitGPIO() ;
	Ele_Init();
//	TimerDelayMsec(100);
	#if (ENABLE_UART)
		InitUART(9600) ;
		xprintf("\r\n Uart Init OK0",0);
	#endif
//	Chip_IAP_ReinvokeISP();
	InitUSBOSC( USB_SEL , USB_PSEL , USB_MSEL , USB_DIV_VAL) ;
	//-----MCU Periphert Init-----
//	InitI2C();
	
  Param_Init();
	MyDebug_Init();
	Charger_Init();        //charger IC BQ21128
	
  #if (defined(IRPT_V4))
  //null
  #elif (defined(V4_HALL_HPTIC_V4))
  	InitSPI0Master(); //hall use
  #elif (defined(GT_LINA_T1A_H5))
  InitSPI1Master(); //hall use
  #else
	InitSPI0Master();
	InitSPI1Master();
  #endif
	
	InitCT16B0(TIME_CT16B0_PR ,TIME_CT16B0_COUNT) ;                //1mS Count Time
               
	//-----------------------------------------------------------------APP INIT,
	NVIC_SetPriority(USB_IRQn , 1);
	nsAPI_USB_Init();/* USB Initialization */	

	OperateFlash_Pointer_Init();
	PC_Communicate_Pointer_Init();
	Custom_Pointer_Init();
	
	TimerDelayMsec(500);
  
  #if (defined(IRPT_V4))
  //null
  #else
	HallSensor_INIT();
  #endif
	
	Device_Manually_Enter_RestoreDefault();	
	#if USE_INSIDE_EEPROM
	#else
	FLASH_Communication_Check();
	#endif
	Device_check_Profile();
  #if (defined(IRPT_V4) || defined(V4_HALL_HPTIC_V4) || defined(GT_LINA_T1A_H5))
  #else
	optical_init();
  #endif
	Custom_Init();
	RamMalloc_Init();
	Custom_Load_ModeSetting();
	//Led_Init();

	Profile_LED_flag = TRUE;
  
  LPC_SYSCON->BODCTRL = (3 << 0)   /* BODRSTLEV = Level 3 (2.63V assert / 2.71V release) */
                      | (0 << 2)   /* BODINTVAL = 0 (interrupt disabled) */
                      | (1 << 4);  /* BODRSTENA = 1 (enable brown-out reset) */
                      
  #if (defined(IRPT_V4))
  PAW3950_Init();
  MyADC_Init(pLeftAdcHandle);
  MyADC_Init(pRightAdcHandle);
  #elif (defined(V4_HALL_HPTIC_V4) || defined(GT_LINA_T1A_H5))
  PAW3950_Init();
  #endif
	
}

void Device_Active(void)
{
#if MYDEBUG_ENABLE
	uint32_t uid = 0;
	
	Chip_IAP_ReadUID(&uid);
	InfoLog("UID: %u; mySfVer: 0x%x_%u", uid, PRODUCT_VER, DEBUG_VER);
#endif
	
	while (1)
	{
		x_value_delta.b.hi  = 0;
		x_value_delta.b.lo  = 0;
		y_value_delta.b.hi  = 0;
		y_value_delta.b.lo  = 0;
#if 1
		Device_Scan();

		Device_Pack();

		Device_Report();

		wwdt_feed();

		PC_Communicate();
    
		Suspend_Check();
#endif
		
		Ele_PollingTask();
    
    Task_ParamGuard();

    Charger_PollingTask(); //charger IC BQ21128: feed dog, temperature -> charge current

		MyDebug_Task();
    
    #if (defined(IRPT_V4))
    MyADC_Poll_Test();
    #endif

		
	}

}

void Device_Pack(void)
{
	REPORT_POINTER report_pointerTmp;
	UINT8 manual_mouseReport[8] = {MOUSE_ID,0,0,0,0,0,0,0};
	UINT8 manual_keyboardReport[8] = {0,0,0,0,0,0,0,0};
	UINT8 manual_multimediaReport[3] = {MOUSE_MULTIMEDIA_ID,0,0};
	UINT8 macroTmp=0;
	UINT8 zoomType;
	report_pointerTmp.mouse_report = manual_mouseReport;
	report_pointerTmp.keyboard_report= manual_keyboardReport;
	report_pointerTmp.multimedia_report= manual_multimediaReport;
//	REPORT_POINTER  report_pointerTmp = {manual_mouseReport,manual_keyboardReport,manual_multimediaReport};
 
	if (!deviceEvent)
	{
		return;
	}
	
	Device_Set_LEDOff(gloabSetting.settingMode.enter_LefOff);
//	
//	if((!LED_off_cycle) && (LED_off_cnt))
//	{
//		LedEffect_Start();
//	}	

	if(DEVICE_GET_EVENT((SCROLL_EVENT | BUTTON_EVENT)))
	{
		DEVICE_CLEAR_EVENT((SCROLL_EVENT | BUTTON_EVENT));
   
		data_type |= Custom_HandleKey(wheel_button_status,&report_pointerTmp);

		if (data_type & MOUSE_KEY_REPORT)
		{
			Tool_Assigned_BufferToBuffer(manual_mouseReport,mouseReport_back,8);
		}
		if (data_type & KEYBOARD_KEY_REPORT)
		{
			Tool_Assigned_BufferToBuffer(manual_keyboardReport,keyboardReport_back,8);
		}
	}
	
	if(DEVICE_GET_EVENT(OPTICAL_EVENT))
	{
		DEVICE_CLEAR_EVENT(OPTICAL_EVENT);
    
  #if (defined(IRPT_V4) || defined(V4_HALL_HPTIC_V4) || defined(GT_LINA_T1A_H5))
    manual_mouseReport[2] = x_value_delta.b.lo;
    manual_mouseReport[3] = x_value_delta.b.hi;
    manual_mouseReport[4] = y_value_delta.b.lo;
    manual_mouseReport[5] = y_value_delta.b.hi;
    ///// clear optical data
    x_value_delta.w = 0;
    y_value_delta.w = 0;
  #else
		OP_3325_Packet(&x_value_delta,&y_value_delta,manual_mouseReport);
  #endif

		data_type |= MOUSE_KEY_REPORT;
		manual_mouseReport[1] = mouseReport_back[1];
	}

	
	if (DEVICE_GET_EVENT(Zoom_Out_In_Event))
	{
		DEVICE_CLEAR_EVENT(Zoom_Out_In_Event);

		zoomType = Device_Zoom_OutIn_GetData(&manual_mouseReport[6],manual_keyboardReport);

		if (zoomType == MOUSE_KEY_REPORT)
		{
			data_type |= MOUSE_KEY_REPORT;
			manual_mouseReport[1] = manual_mouseReport[1] | mouseReport_back[1];
		}
        else if (zoomType == KEYBOARD_KEY_REPORT)
		{
			data_type |= KEYBOARD_KEY_REPORT;
		}
	}
	
	if (DEVICE_GET_EVENT(LR_WHEEL_EVENT))
	{
		DEVICE_CLEAR_EVENT(LR_WHEEL_EVENT);
		manual_mouseReport[7] = Device_LrWheel_GetData();	

		data_type |= MOUSE_KEY_REPORT;
		manual_mouseReport[1] = mouseReport_back[1];
	}

	if (DEVICE_GET_EVENT(DOUBLE_CLICK_EVENT))
	{
		DEVICE_CLEAR_EVENT(DOUBLE_CLICK_EVENT);
		Device_DoubleClick_GetData(&manual_mouseReport[1]);	

		data_type |= MOUSE_KEY_REPORT;
		manual_mouseReport[1] = manual_mouseReport[1] | mouseReport_back[1];
	}
	
	if (DEVICE_GET_EVENT(MACRO_DEAL_EVENT))
	{
		DEVICE_CLEAR_EVENT(MACRO_DEAL_EVENT);
	
		macroTmp = Macro_Get_ReportType();

		if (macroTmp & MACRO_MS_KEY)
		{
			data_type |= MOUSE_KEY_REPORT;
		}

		if (macroTmp & MACRO_KB_KEY)
		{
			data_type |= KEYBOARD_KEY_REPORT;
		}
	}

	if (Macro_Proceed())
	{
		Tool_Assigned_BufferToBuffer(keyboardReport_back,manual_keyboardReport,8); 

		manual_mouseReport[1] = manual_mouseReport[1] | mouseReport_back[1];
		
		Macro_Combine_Data(manual_mouseReport,manual_keyboardReport);	
//		test
//		Macro_Keyboard();
	}
	
	Tool_Assigned_BufferToBuffer(manual_mouseReport,report_data.mouse_report,8);
	Tool_Assigned_BufferToBuffer(manual_keyboardReport,report_data.keyboard_report,8);
	Tool_Assigned_BufferToBuffer(manual_multimediaReport,report_data.multimedia_report,3);

}

void Device_Report(void)
{
	uint8_t i=0;
	if (!data_type)
	{
		return;
	}

	if ((data_type & MULTIMEDIA_KEY_REPORT) == MULTIMEDIA_KEY_REPORT)
	{		
		data_type &= ~MULTIMEDIA_KEY_REPORT;

		USB_LoadInEP_Data(EP_1_MOUSE, report_data.multimedia_report, 3); 	
		TimerDelayMsec(1);
	}	
#if 1
	{
		if ((data_type & MOUSE_KEY_REPORT) == MOUSE_KEY_REPORT)
		{	
			data_type &= ~MOUSE_KEY_REPORT;
				
			if(HID_Protocol == 1)
			{
				for(i=0;i<60;i++)
				{
					if(nsAPI_USB_EP_Status(HID_EP1_IN))	
					{
					break;
					}
					nsAPI_SSP_Delay1us(10);
				}
				report_data.mouse_report[0]=0x01;
				
				USB_LoadInEP_Data(EP_1_MOUSE, report_data.mouse_report, 8);
			}
			else
			{
				report_data.mouse_report[1] &= 0x07;
				report_data.mouse_report[3] = report_data.mouse_report[4];
				for(i=0;i<60;i++)
				{
					if(nsAPI_USB_EP_Status(HID_EP1_IN))	
					{
						break;
					}
					nsAPI_SSP_Delay1us(10);
				}
				USB_LoadInEP_Data(EP_1_MOUSE, &report_data.mouse_report[1], 3);
			}
		}
	}
#endif

	if ((data_type & KEYBOARD_KEY_REPORT) == KEYBOARD_KEY_REPORT)
	{
		data_type &= ~KEYBOARD_KEY_REPORT;

		USB_LoadInEP_Data(EP_2_KEYBOARD, report_data.keyboard_report, 8);
	}	
}

#if 0
void nsAPI_Sensor_EventMointor(void)
{			  
//	  DrvGPIOOutput(1,13,0);	
    Optical_Read(&x_value_delta,&y_value_delta);
//    DrvGPIOOutput(1,13,1);	
}

void nsAPI_GPIO_ButtonEventMointor(void)
{
    GPIO_Check_Key();
    if (KeyFlag ==1)
    {
        KeyFlag =0;
        g_StaticMachineEvent2Type = SM_EVENT2_BUTTON_HANDLER;
    }    
}

void Main_Send_Mouse(U8 KeyCode){
	g_Main_Setting.MouseInReport[GIdx0] =0x01;
	g_Main_Setting.MouseInReport[GIdx1] |= KeyCode;	
  g_Repot_Process.Mouse_Report_In = 1;
}
void Main_Release_Mouse(U8 KeyCode){
	g_Main_Setting.MouseInReport[GIdx0] =0x01;
	g_Main_Setting.MouseInReport[GIdx1] &= ~(KeyCode);	
  g_Repot_Process.Mouse_Report_In = 1;
}

void Main_Release_Keyboard(void){
	g_Main_Setting.KeyboardInReport[GIdx0] = 0;		//Function key
	g_Main_Setting.KeyboardInReport[GIdx1] = 0;		//Keycode

	g_Repot_Process.Keyboard_Report_In = 1;
}
void Main_Send_Keyboard(U8 KeyCode){
	g_Main_Setting.KeyboardInReport[GIdx0] = 0;		//Function key
	g_Main_Setting.KeyboardInReport[GIdx1] = KeyCode;	//Keycode

	g_Repot_Process.Keyboard_Report_In = 1;
}
#endif 

void Device_Macro_Scan(void)
{
	if (macro_event)
	{	
		DEVICE_POST_EVENT(MACRO_DEAL_EVENT);
	}
}

#if (defined(IRPT_V4) || defined(V4_HALL_HPTIC_V4) || defined(GT_LINA_T1A_H5))
void Device_Optical_Scan(void)
{
	//UINT8 reg_data;  // 20260508, the code was canceled.
	static UINT8 check_count = 0; 

	if (!optical_scan_cycle)
	{
		optical_scan_cycle = device_ReportRata;

		check_count++;
		if(check_count > 50)
		{
			check_count = 0;
			
			//reg_data = optical_spi_get(0x20); // 20260508, the code was canceled.
			//if(reg_data != 0x40)optical_init(); // 20260508, the code was canceled.
		}
    int16_t xTmp, yTmp;
    if(!PAW3950_GetMotionData(&xTmp, &yTmp))
    {
      x_value_delta.b.lo = (uint8_t)xTmp;
      x_value_delta.b.hi = xTmp >> 8;
      y_value_delta.b.lo = (uint8_t)yTmp;
      y_value_delta.b.hi = yTmp >> 8;
      DEVICE_POST_EVENT(OPTICAL_EVENT);
    }
		
		/*if(optical_get_report(&x_value_delta,&y_value_delta)) // 20260508, the code was canceled.
		{
			if(SaveDPIflag)
			{
				DPI_delayflag=true;
				dpitimerdelay=5;
			}
//			DrvGPIOOutput(0,13,1) ;
			Send_SensorData_F=TRUE;
			DEVICE_POST_EVENT(OPTICAL_EVENT);
		}
		else 
		{
			
		}*/
	}
}
#else
void Device_Optical_Scan(void)
{
	UINT8 reg_data;
	static UINT8 check_count = 0;

	if (!optical_scan_cycle)
	{
		optical_scan_cycle = device_ReportRata;

		check_count++;
		if(check_count > 50)
		{
			check_count = 0;
			
			reg_data = optical_spi_get(0x20);
			if(reg_data != 0x40)optical_init();
		}
		
		if(optical_get_report(&x_value_delta,&y_value_delta))
		{
			if(SaveDPIflag)
			{
				DPI_delayflag=true;
				dpitimerdelay=5;
			}
//			DrvGPIOOutput(0,13,1) ;
			Send_SensorData_F=TRUE;
			DEVICE_POST_EVENT(OPTICAL_EVENT);
		}
		else 
		{
			
		}
	}
}
#endif

void Device_Set_ReportRate(UINT8 reportRate)
{
	device_ReportRata = reportRate;
}

void Device_Set_LEDOff(UINT8 led_off)
{
	timer_1s = 1000;
	LED_off_cnt = led_off;
	LED_off_cycle = led_off * 60;
}

void Device_Scan(void)
{
#if 1
	Device_Optical_Scan();

	if (!one_ms_cycle)
	{
		one_ms_cycle = 1;
		Send_Button_F = TRUE;
		Device_Button_Scan();

    #if (ENABLE_WHEEL_SCAN)
		Device_Wheel_Scan();
    #endif
		
//		Macro_Process();

//		Device_Macro_Scan();

//		Device_LrWheel_Scan();

//		Device_Zoom_OutIn_Scan();

//		Device_DoubleClick_Scan();
	}
#endif

#if 1
	if (!tm_usb_sof)
	{
		tm_usb_sof = 1;

		if (Device_Check_UsbActive())
		{
			//Led_Scan();
			
			//Led_DPI_Scan();
		}
	}
#endif
}

BOOL Device_Check_UsbActive(void)
{
	BOOL active = FALSE;

	if (de_determineUsbAct >= Device_Determine_Time)
	{
	  if(g_USBCore_Setting.Enumeration_Finish == TRUE)
		{
		active = TRUE;
		wwdt_feed();
		}
		else
		{
		 de_determineUsbAct=0;
		}
	}

	return active;
}




//void Device_Button_Scan(void)
//{
//GPIO_Check_Key();
//if (KeyFlag ==1)
//    {
//        KeyFlag =0;
//        g_StaticMachineEvent2Type = SM_EVENT2_BUTTON_HANDLER;
//    }    
//}	
void Device_Button_Scan(void)
{
	static UINT8 btnScan = 0;
	static UINT32  btnLast = 0;
	
	if(btnScan)btnScan--;
	if (!btnScan)
	{
		btnScan = 1;
		if(Button_Scan())
		{
			wheel_button_status &= ~ALL_BUTTON_REPORT_BIT;
			wheel_button_status |= buttonScan_stru.scan_result_pre;
			DEVICE_POST_EVENT(BUTTON_EVENT);
		
			if (btnLast != buttonScan_stru.scan_result_pre)
			{
				btnLast = buttonScan_stru.scan_result_pre;
				if (btnLast)
				{					
//					LightEffect_ActionEvent_Set();
				}
			}	
		}
	}
}



//void Device_Wheel_Scan(void)
//{
//  nsAPI_WheelEventMointor();
//}

void Device_Wheel_Scan(void)
{
	static UINT8 wheelScan = 0;

	if(wheelScan)wheelScan--;
	if (!wheelScan)
	{
		wheelScan = 1;
		if (Scroll_Scan())
		{
			wheel_button_status  &= ~(WHEEL_DOWN_REPORT_BIT | WHEEL_UP_REPORT_BIT);
			wheel_button_status |= wheelScan_stru.wheel_status;
			DEVICE_POST_EVENT(SCROLL_EVENT);
		}
	}
}

#if 0
uint8_t buttons_scan()
{
	uint8_t retval = 0;

	if(DrvGPIORead(0, 12) ==0)  // LB
		retval |= 0x01;
	
	if(DrvGPIORead(1, 27) ==0)  //RB    //  œ»”√ P1_20 ¥˙ÃÊ P1_27   if(DrvGPIORead(1, 27) ==0)  //RB 
		retval |= 0x02;
	
	if(DrvGPIORead(0, 17) ==0)  //MB
		retval |= 0x04;
	
	if(DrvGPIORead(0, 11) ==0)  //side front
		retval |= 0x10;
	
	if(DrvGPIORead(1, 21) ==0)  //side back
		retval |= 0x08;
//	if(DrvGPIORead(1, 31) ==0)  //profile
//		retval |= 0x01;
	return retval;
}


void GPIO_Check_Key(void)
{
	uint8_t buttons;
	static uint8_t last_Button = 0x00;
//	previous_buttons=0;
	buttons = buttons_scan();

//	start
	if(last_Button != buttons)
	{
		last_Button = buttons;
		Chk_Button_state = Debouncing;
		Debounce_cnt = 12;				
	}
	else
	{
		if((Chk_Button_state == Debouncing)&&(Debounce_cnt==0))
		{
				
			#if (ENABLE_UART)
				InitUART(115200) ;
				xprintf("\r\n buttons= %d",buttons);
//				xprintf("\r\n previous_buttons= %d",previous_buttons);	
			#endif
			Chk_Button_state = No_Button;
			
//			*mouse_buttons = buttons;
			KeyFlag =1;
			if(buttons!=0)
			{ 
				if(buttons!=previous_buttons)
				{
					Main_Release_Mouse(previous_buttons);
				}
				previous_buttons =buttons;	
//				LightEffect_ActionEvent_Set();
				Main_Send_Mouse(buttons);
				if(buttons==0x08)
				{
					LED_Mode++ ;
					if(LED_Mode==5)
					{
						LED_Mode = 1 ;
					} 
					Device_Zoom_OutIn_Start(0x01);

//					Device_DoubleClick_Start();		
//					LED_Gon();	
				#if 1
					switch(LED_Mode)
					{	
						case 1 :
							Led_Init();
							LED_Ron();							
						break;
						case 2 :
							Led_Init();
							LED_Gon();							
						break;
						case 3 :
							Led_Init();
							LED_Bon();							
						break;
						case 4 :
							Led_Init();
							LED_RGB();							
						break;
						default :
						break ;
					}
				#endif	
				}
				if(buttons==0x10)
				{
					Device_Zoom_OutIn_Stop();
					LED_off();
				}		 
			}
			else if(buttons==0)
			{
				InitUART(115200) ;
				xprintf("\r\n previous_buttons= %d",previous_buttons);
				Main_Release_Mouse(previous_buttons);
				previous_buttons=0;	
			}
				
//			mouse_buttons_packet[NRFR_REPORT_ID]=buttons;
//			g_Main_Setting.MouseInReport[GIdx0]= mouse_buttons_packet[NRFR_REPORT_ID];
		}
	}
//	end

//	*cpi = false;   
//	*mouse_pan = 0x00;	
//	*advanced_buttons = 0x00;
}	
void nrfr_mouse_buttons_read(uint8_t* mouse_buttons, uint8_t* mouse_pan, uint8_t* advanced_buttons,bool* cpi)
{
	uint8_t buttons;
	static uint8_t last_Button = 0x00;
  
	buttons = buttons_scan();
	
#if (ENABLE_UART)
    InitUART(115200) ;
    xprintf("\r\n Uart Init nK",0);
    #endif
	//start
	if(last_Button != buttons)
	{
		last_Button = buttons;
		Chk_Button_state = Debouncing;
		Debounce_cnt = 12;				
	}
	else
	{
		if((Chk_Button_state == Debouncing)&&(Debounce_cnt==0))
		{
			Chk_Button_state = No_Button;
			*mouse_buttons = buttons;
			
		}
	}
	//end
		
	*cpi = false;   
	*mouse_pan = 0x00;	
	*advanced_buttons = 0x00;
}

void nsAPI_Main_MouseInReportHandler(void)
{

	#if REMOTE_WAKEUP_ENABLE
	if ( rSuspendFlag == 1 ) 
	{
		rSuspendFlag = 0;
		//S_PWM_INIT();
		//nsAPI_Sensor_Init();		
		//RemoteWakeupEvent();
		g_Main_Setting.SensorReset = 1;
		g_Repot_Process.Mouse_Report_In = 0;
		return;
  	}
	#endif
	#if (SUPPORT_MAC_BOOT_AND_DOS_MODE)
	if (g_USBCore_Setting.EP0_DosMode)
	{
		if(g_Sensor_Data.Changed == 1)
		{
			//g_Main_Setting.MouseInReport[GIdx1] = g_Sensor_Data.X1;
			//g_Main_Setting.MouseInReport[GIdx2] = g_Sensor_Data.Y1;
			//g_Main_Setting.MouseInReport[GIdx3] = 0;
			//g_Main_Setting.MouseInReport[GIdx4] = 0;
			//org size is 8 byte, 2016_0608 become 6 byte and remove id
			g_Main_Setting.MouseInReport[GIdx1] = g_Sensor_Data.X1;
			g_Main_Setting.MouseInReport[GIdx2] = g_Sensor_Data.Y1;
			g_Main_Setting.MouseInReport[GIdx3] = 0;
			g_Main_Setting.MouseInReport[GIdx4] = 0;
		}
	}
	else
	#endif //SUPPORT_MAC_BOOT_AND_DOS_MODE
	{
        #if (SIM_MOUSE_SENSOR)
            g_Main_Setting.MouseInReport[GIdx1] = 1;
						g_Main_Setting.MouseInReport[GIdx2] = 0;
						g_Main_Setting.MouseInReport[GIdx3] = 1;
						g_Main_Setting.MouseInReport[GIdx4] = 0;
        #else
//		g_Main_Setting.MouseInReport[GIdx0] = 0x01;//Report ID
		if(g_Sensor_Data.Changed == 1)
		{
			//org size is 8 byte, 2016_0608 become 6 byte and remove id
			//g_Main_Setting.MouseInReport[GIdx1] = g_Sensor_Data.X1;
			//g_Main_Setting.MouseInReport[GIdx2] = g_Sensor_Data.X2;
			//g_Main_Setting.MouseInReport[GIdx3] = g_Sensor_Data.Y1;
			//g_Main_Setting.MouseInReport[GIdx4] = g_Sensor_Data.Y2;
			g_Main_Setting.MouseInReport[GIdx1] = g_Sensor_Data.X1;
			g_Main_Setting.MouseInReport[GIdx2] = g_Sensor_Data.X2;
			g_Main_Setting.MouseInReport[GIdx3] = g_Sensor_Data.Y1;
			g_Main_Setting.MouseInReport[GIdx4] = g_Sensor_Data.Y2;
		}
        #endif
	}
	if(nsAPI_USB_EP_Status(HID_EP1_IN))	//if((g_Main_Setting.EPInOutReq & EP1_IN) == EP1_IN)  //modify 20120808
	{	
	
		nsAPI_USB_WriteEP(HID_EP1_IN, &g_Main_Setting.MouseInReport[0], MOUSE_IN_REPORT_SIZE);
		nsAPI_Main_Clr_EP1Event();
		g_Main_Setting.EPInOutReq &= ~EP1_IN;
		g_Repot_Process.Mouse_Report_In = 0;
		g_Sensor_Data.Changed = 0;
	}
}

//-----------------------------------------------------------------------
void nsAPI_Main_MediaInReportHandler(void)
{
}
//-----------------------------------------------------------------------
void nsAPI_Main_SystemInReportHandler(void)
{
}
//-----------------------------------------------------------------------
void nsAPI_Main_KeyboardInReportHandler(void)
{
  if(nsAPI_USB_EP_Status(HID_EP2_IN))//if((g_Main_Setting.EPInOutReq & EP2_IN) == EP2_IN)
	{
		nsAPI_USB_WriteEP(HID_EP2_IN,&g_Main_Setting.KeyboardInReport[0],KEYBOARD_IN_REPORT_SIZE);
		nsAPI_Main_Clr_KeyboardEvent();
		g_Main_Setting.EPInOutReq &= ~EP2_IN;
		g_Repot_Process.Keyboard_Report_In = 0;
	}
}
//-----------------------------------------------------------------------
void nsAPI_Main_MacroKeyInReportHandler(void)
{
	if(nsAPI_USB_EP_Status(HID_EP2_IN))//if((g_Main_Setting.EPInOutReq & EP2_IN) == EP2_IN)
	{
		nsAPI_USB_WriteEP(HID_EP2_IN,&g_Main_Setting.KeyboardInReport[0],KEYBOARD_IN_REPORT_SIZE);
		//nsAPI_Main_Clr_KeyboardEvent();
		g_Main_Setting.EPInOutReq &= ~EP2_IN;
		g_Repot_Process.Macro_Keyboard_Report_In = 0;
	}    
}
//-----------------------------------------------------------------------
void nsAPI_Main_ShortcutInReportHandler(void)
{
}
//-----------------------------------------------------------------------
void nsAPI_Main_ConsumerInReportHandler(void)
{
}
//-----------------------------------------------------------------------
void nsAPI_Main_Clr_EP1Event(void)
{
	//org size is 8 byte, 2016_0608 become 6 byte and remove id
	
//	if(g_Main_Setting.MouseInReport[GIdx0] != 1)
//	{
//		g_Main_Setting.MouseInReport[GIdx0] = 0;
//		g_Main_Setting.MouseInReport[GIdx1] = 0;
//	}
//	else
	
	{
		g_Main_Setting.MouseInReport[GIdx1] = 0;
		g_Main_Setting.MouseInReport[GIdx2] = 0;
		g_Main_Setting.MouseInReport[GIdx3] = 0;
		g_Main_Setting.MouseInReport[GIdx4] = 0;
		g_Main_Setting.MouseInReport[GIdx5] = 0;
		g_Main_Setting.MouseInReport[GIdx6] = 0;
		g_Main_Setting.MouseInReport[GIdx7] = 0;
	}

}
//-----------------------------------------------------------------------
void nsAPI_Main_Clr_KeyboardEvent(void)
{
	g_Main_Setting.KeyboardInReport[GIdx0] = 1;
	g_Main_Setting.KeyboardInReport[GIdx1] = 0;

}
//-----------------------------------------------------------------------
void nsAPI_Main_Clr_ConsumerEvent(void)
{
	g_Main_Setting.ConsumerInReport[GIdx0] = 2;
	g_Main_Setting.ConsumerInReport[GIdx1] = 0;
	g_Main_Setting.ConsumerInReport[GIdx2] = 0;
//	g_Main_Setting.ConsumerInReport[GIdx3] = 0;   
//	g_Main_Setting.ConsumerInReport[GIdx4] = 0;   
//	g_Main_Setting.ConsumerInReport[GIdx5] = 0;   
//	g_Main_Setting.ConsumerInReport[GIdx6] = 0;   
//	g_Main_Setting.ConsumerInReport[GIdx7] = 0;   
}
//-----------------------------------------------------------------------
void nsAPI_Main_Clr_EP3Event(void)      
{
	U8 i;

	for(i=0;i<CUSTOMER_IN_REPORT_SIZE;i++)
		g_Main_Setting.InReport[i] = 0;
}

void nsAPI_Main_Clr_EP4Event(void) 
{
	U8 i;
	for(i=0;i<FWUPDATE_IN_REPORT_SIZE;i++)
		g_Main_Setting.FirmwareUpdateInReport[i] = 0;
}
#endif
//***********************************************************************
//*								SUB_FUNCTION							*
//***********************************************************************
//-----DrvDelay-----
void DrvDelay(unsigned long DelayData)
{
    unsigned int DelayLoop ;
    while(DelayData!=0)
    {
        DelayLoop = DELAY_COUNT ;
        while(DelayLoop!=0)
        {	  
            DelayLoop-- ;
            if(DelayLoop==0)
            {					 
                DelayData-- ;
            }
        }
    }
}
void DrvDelay1mS(unsigned int DelayData)
{
	unsigned char DelayFg;
//	 unsigned int TimeDelay ;  
    DelayFg = 0 ;
 //   TimeDelay = DelayData ;
    while(DelayFg==0) ;
}
void Delayus(uint16_t u16DelayTime)
{
    uint16_t i;

    if(u16DelayTime < 2) u16DelayTime = 2;
    /* i = 1 -> about 2.68us
    i = 2 -> about 2.68us
    i = 3 -> about 3.96us
    i = 4 -> about 5.16us
    */
    u16DelayTime += ((u16DelayTime -2)*5);

    for(i = u16DelayTime; i > 0; i --){}
}

void nsAPI_SSP_Delay1us(U16 u16DelayTime)
{
	U8 i;

	while(u16DelayTime)
	{
		for (i = 4; i > 0; i--){};
			u16DelayTime --;
	}	
}


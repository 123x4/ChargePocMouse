#include "custom.h"
#include "optical_3325.h"
#include "operation_flash.h"
#include "pc_communicate.h"
#include "device.h"
#include "led_effect.h"
#include "led2.h"
#include "Production_line_test.h"
#include "core_cm0.h"
//#include ".\Led_Layer\light_effect_layer.h"
//#include "test.h"
#include "GPIO.h"
#include "macro.h"
#include "string.h"
#include "button.h"
#include "reaction.h"
#include "eeprom.h"
#include "my_board.h"
GLOAB_SETTING_MODE_TYPE gloabSetting;
UINT8 *usb_sendBuffer_p = NULL;
BOOL SaveDPIflag =FALSE;
LAYER_Message layer_message;
BOOL profile_keyflag=FALSE;
uint16_t profile_timer=3000;
UINT8 cu_manualLedMode;
uint8_t *VecDst=(uint8_t*)0x10001F00;
//uint8_t *ISPADR=(uint8_t*)0x000002FC;
//UINT32  ISPDISDATA= 0x4E697370;
uint8_t FlashData=0xAA;
//uint8_t Zero_FlashData=0x00;
BOOL Check_mode_flag=FALSE;
extern UINT8 mouseReport_back[8];
BOOL LED_Test_Flag = false;
//extern Profile_Mode_Stru profile_mode;
//extern uint8_t profile_timer;

UINT8 Code_Compile_Timer[] = 
{
//202205070
	0x32,0x30,0x32,0x32,0x30,0x34,0x30,0x31,0x30,
};

UINT8 profileName[Profile_Name_Len]  = 
{
	233,133,141,231,189,174,230,150,135,228,
	187,182,49,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0
};

//UINT8 Dpi_Value[30] = 
//{
//	RESOLUTION_3200,RESOLUTION_3300,RESOLUTION_3300,RESOLUTION_3400,RESOLUTION_3400,
//	RESOLUTION_3500,RESOLUTION_3600,RESOLUTION_3600,RESOLUTION_3700,RESOLUTION_3800,
//	RESOLUTION_3800,RESOLUTION_3900,RESOLUTION_3900,RESOLUTION_4000,RESOLUTION_4100,
//	RESOLUTION_4100,RESOLUTION_4200,RESOLUTION_4300,RESOLUTION_4300,RESOLUTION_4400,
//	RESOLUTION_4400,RESOLUTION_4500,RESOLUTION_4600,RESOLUTION_4600,RESOLUTION_4700,
//	RESOLUTION_4800,RESOLUTION_4800,RESOLUTION_4900,RESOLUTION_4900,RESOLUTION_5000
//};

UINT8 Code_Compile_Timer_Len(void)
{
	UINT8 len;

	len = (sizeof(Code_Compile_Timer));

	return len;
}

void Custom_Pointer_Init(void)
{
	usb_sendBuffer_p = receive_buff;

}


void Bootloadable_Load(void)
{
	//RGB_STRU none_rgb = {0,0,0};
//  const UINT8 BootLoader_Flash[2]__attribute__((at(0X0000FE00)))={0xAA,0x01} ;
//	int ebx;
	memcpy((uint8_t *)VecDst, &FlashData,1);
// int *p=(int*)0x10001F00;
// *p=0xAA; 
// *p=&0xAA;
//	__ASM
//	{
//	mov ebx , 0xAA
//	mov *p, ebx
//	}
// int *p = (int*)0x0030; 
//	*p = 0x31;
//	LedEffect_Put_StaticColor(none_rgb,FALSE,LED_ALL_SEND);
//U8 P[2]={0xAA,0x01};
//const UINT8 BootLoader_Flash[2]__attribute__((at(0X0000FF00)));//={0xAA,0x01} ;
//const  UINT8 *flash_gloabSetting_B = NULL;	
//flash_gloabSetting_B =BootLoader_Flash;
//GtechFlash_WriteBytes(BootLoader_Flash,0,P,0x02);		
	NVIC_SystemReset();
}


void Custom_DefineKey_Init(void)
{
	/// DPI,K5,K4,WHEEL_DOWN,WHEEL_UP,MB,   RB, LB;
	///7 , 6  ,5 ,   4        3,     2,   	1,  0; 
	gloabSetting.settingMode.key_function[0].usb_registration.id = 0xb0;//l
	gloabSetting.settingMode.key_function[1].usb_registration.id = 0xb1;//r	//have changed temporary
	gloabSetting.settingMode.key_function[2].usb_registration.id = 0xb2;//m  //have changed temporary
	gloabSetting.settingMode.key_function[3].usb_registration.id = 0XB5;//WHEEL_UP
	gloabSetting.settingMode.key_function[4].usb_registration.id = 0xB6;//WHEEL_DOWN

	gloabSetting.settingMode.key_function[5].usb_registration.id = 0xb3;//k4
	gloabSetting.settingMode.key_function[6].usb_registration.id = 0xb4;//k5


	gloabSetting.settingMode.key_function[7].usb_registration.id = 0;//VOLUME_UP
	gloabSetting.settingMode.key_function[8].usb_registration.id = 1;//VOLUME_DOWN
	gloabSetting.settingMode.key_function[9].usb_registration.id = 0xC1;//DPI_CYCLE
}

void Custom_Perform_Init(UINT8 mode)
{
//	if(mode == 0)
//	{
		gloabSetting.settingMode.dpi_class[0].dpi_x = 0x04;		//DPI_800;//DPI_800;
		gloabSetting.settingMode.dpi_class[0].dpi_y = 0x04;		//DPI_800;//DPI_800;

		gloabSetting.settingMode.dpi_class[1].dpi_x = 0x08;//DPI_1600;//DPI_1600;
		gloabSetting.settingMode.dpi_class[1].dpi_y = 0x08;//DPI_1600;//DPI_1600;

		gloabSetting.settingMode.dpi_class[2].dpi_x = 0x10;//DPI_2400;//DPI_2400;
		gloabSetting.settingMode.dpi_class[2].dpi_y = 0x10;//DPI_2400;//DPI_2400;
	
		gloabSetting.settingMode.dpi_class[3].dpi_x = 0x18;//DPI_2400;//DPI_2400;
		gloabSetting.settingMode.dpi_class[3].dpi_y = 0x18;//DPI_2400;//DPI_2400;

		gloabSetting.settingMode.report_rate = REPORT_RATE_1000; //REPORT_RATE_1000

		gloabSetting.settingMode.dpi_gear = DPI_GEAR_4;//DPI_GEAR_2;

		gloabSetting.settingMode.dpi_default = DPI_GEAR_4; //DPI_GEAR_3;

		gloabSetting.settingMode.dpi_max = DPI_GEAR_4;

//		gloabSetting.settingMode.aloneXY = FALSE;
//		Tool_Clear_ArrayData(gloabSetting.settingMode.aloneXY,0,5);
		Tool_Clear_ArrayData(gloabSetting.settingMode.aloneXY,0,5);
	 
		gloabSetting.settingMode.acceleration = 0;
//	}
}

void Custom_LefEffect_Init(void)
{
	UINT8 ii;

	gloabSetting.settingMode.brightness = 0XCC;//80%
	///LED Effect !!!!!
//	gloabSetting.settingMode.LE_value.mul_interface = FALSE;

	for (ii = 0;ii < LE_Group_Num;ii++)
	{
	
		gloabSetting.settingMode.LE_value.type = LED_MODE_COLORFUL;
		gloabSetting.settingMode.LE_value.typeValue.colorCycle.transparency = 100;
		gloabSetting.settingMode.LE_value.typeValue.colorCycle.speed = 1;

	}

}

void Custom_ModeParamter_Init(UINT8 mode)
{
 
	gloabSetting.settingMode.nameID = mode;

	//Tool_Clear_ArrayData(gloabSetting.settingMode.name,0,29);
	profileName[Profile_Name_ModefyIndex] = 49 + mode;

	Tool_Assigned_BufferToBuffer(profileName,gloabSetting.settingMode.name,Profile_Name_Len);

	Custom_DefineKey_Init();

	Custom_Perform_Init(mode);

	///this value is bigger  the hight is low
	gloabSetting.settingMode.pixThresh = OP_PixThresh_Init;
	gloabSetting.settingMode.min_sq = OP_Min_Sq_Init;	
	gloabSetting.settingMode.enter_LefOff = 0X00;//3 min

	Custom_LefEffect_Init();

//	OperateFlash_Store_ModeSetting(mode);
}

void Custom_Paramter_Init(void)
{
	UINT8 ii;

	///gloab init
	gloabSetting.setting.sign = STORE_ONCE_SIGN;
	gloabSetting.setting.currentMode = 0;

//	for (ii = 0;ii < MODE_NUM;ii++)
//	{
		gloabSetting.setting.FP_ID = 0xff;
//	}

//	OperateFlash_Store_GloabSetting();

	///mode init
	for (ii = 0;ii < MODE_NUM;ii++)
	{
		Custom_ModeParamter_Init(ii);
	}
}

#if 1
// compare the last restored profile checkcum with the current profile checksum, if not the same, restore factory.
void Device_check_Profile(void)
{
//	uint8_t mode=0, CRC_buffer[5]={0,0,0,0,0},CRC_back_buffer[5]={0,0,0,0,0};
	//UINT8 readBuf[OPERATE_MODE_SETTING_SIZE]; //i,FF_count=0,Zero_count=0
	//RGB_STRU none_rgb = {0,0,0};
//	OperateFlash_Read_value(0,CRC_back_buffer,4);   //Read the last restored checksum
//	for(mode=0;mode<MODE_NUM;mode++)
//	{
//		OperateFlash_Modedata_Read(mode,readBuf);  //Read current profile

//		CRC_buffer[mode]=CRC_Calculate(readBuf,0,OPERATE_MODE_SETTING_SIZE-1);  //calculate profile checksum.
//		
//		if(CRC_buffer[mode]!=CRC_back_buffer[mode])   
//		{
//			 Check_mode_flag=TRUE;

//			 TimerDelayMsec(200);
//			 break;
//		}
//	}
	if(Check_mode_flag)
	{
		Custom_Paramter_Init();
	}
}
#endif
//void Dis_U(void)
//{
//	memcpy((uint8_t *)ISPADR, &ISPDISDATA,4);
//}

void Custom_Init(void)
{
//	UINT8 readBuf[Code_Compile_Timer_LEN];
	//UINT8 readBuf[sizeof(Code_Compile_Timer)]; 

//	OperateFlash_Read_GloabSetting();

//	OperateFlash_CodeCompileTimer_Read(readBuf);
//	
//	if (!Check_Buffer_If_String(readBuf,Code_Compile_Timer,Code_Compile_Timer_LEN))//Code_Compile_Timer_LEN))
//	{
//		OperateFlash_CodeCompileTimer_Init();

		Custom_Paramter_Init();

//		Macro_Init_Head();
//	}		

	///if last cnt in temporary profile,change to 0 profile 
//	if (gloabSetting.setting.currentMode >= MODE_NUM - 1)
//	{
//		gloabSetting.setting.currentMode = 0;

//		OperateFlash_Store_GloabSetting(); //
//	}
}



void Custom_GloabSetting_Init(void)
{
//	Device_Set_LEDOff(gloabSetting.settingMode.enter_LefOff);

	Custom_Set_DPI(gloabSetting.settingMode.dpi_gear,FALSE,FALSE,TRUE);// this function in after optical init 

	Device_Set_ReportRate(gloabSetting.settingMode.report_rate);

//	Led_Enter_LefOff_Set(gloabSetting.settingMode.enter_LefOff);

//	Led_Adjust_Brightness(gloabSetting.settingMode.brightness);

//	LedEffect_Start();
//	Led_Update_Mode();
}

void Custom_Load_ModeSetting(void)
{
//	OperateFlash_Read_ModeSetting(gloabSetting.setting.currentMode);

	Custom_GloabSetting_Init();
}


void Custom_RestoreAll_Default(void)
{
	ProductionLine_Start_Clear();
	
	Macro_Init_Head();
	
	Custom_Paramter_Init();

	Custom_Load_ModeSetting();
}

void Custom_Restore_Default(UINT8*buffer)
{

	UINT8 type = buffer[1];

	switch(type)
	{
	case Restore_DefineKey:
		Custom_DefineKey_Init();

		OperateFlash_Store_ModeSetting(gloabSetting.setting.currentMode);

		break;
	case Restore_Perform:
		Custom_Perform_Init(gloabSetting.setting.currentMode);

		OperateFlash_Store_ModeSetting(gloabSetting.setting.currentMode);

		Custom_Set_DPI(gloabSetting.settingMode.dpi_gear,FALSE,FALSE,TRUE);// this function in after optical init 

		Device_Set_ReportRate(gloabSetting.settingMode.report_rate);

		break;
	case Restore_LedEffect:
		Custom_LefEffect_Init();

		OperateFlash_Store_ModeSetting(gloabSetting.setting.currentMode);

//		Led_Adjust_Brightness(gloabSetting.settingMode.brightness);

		//LedEffect_Start();

		break;
	case Restore_Profile:
		ProductionLine_Start_Clear();
	
		Custom_ModeParamter_Init(gloabSetting.setting.currentMode);

		Custom_Load_ModeSetting();
		break;
	case Restore_AllProfile:
		Custom_RestoreAll_Default();
		break;
	default:
		break;

	}


	buffer[2] = gloabSetting.setting.currentMode;
	buffer[3] = TRUE;

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH);	
}


void Custom_Set_DPI(UINT8 gear,BOOL save,BOOL send,BOOL ledAction)
{
	UINT16 dpi_x;// pid;
//  UINT8 buffer[5]={0,0,0,0,0};
	
	if (gear > DPI_GEAR_4)
	{
		return;
	}
  
	dpi_x = gloabSetting.settingMode.dpi_class[gear].dpi_x;
	
	optical_double_flag = 0;
	
	if(dpi_x > 0x3e)
	{
		optical_double_flag = 1;
	}
	
	if(dpi_x > 80)
	{
		dpi_x = 80;
	}
	
	dpi_x = dpi_value_array[dpi_x - 2];
#if (defined(IRPT_V4) || defined(V4_HALL_HPTIC_V4) || defined(GT_LINA_T1A_H5))
#else
	optical_set_dpi(dpi_x);
#endif
  
#if 1
	if (save)
	{
		if(!ProductionLine_Start_Get())
		{
			SaveDPIflag= TRUE;
//			OperateFlash_Set_CycleSave();
//			DrvGPIOOutput(0,23,0);
			OperateFlash_Store_ModeSetting(gloabSetting.setting.currentMode);
//			DrvGPIOOutput(0,23,1);
		}
	}

	if (send)
	{
		Tool_Clear_ArrayData(usb_sendBuffer_p,0,63);
		
//		pid = SPI_Flash_ReadID();

		usb_sendBuffer_p[0] = PERFORMANCE_REALIVE_SETTING;
		usb_sendBuffer_p[1] = DPI_FW_INITIATIVE;
		usb_sendBuffer_p[2] = gloabSetting.setting.currentMode;
		usb_sendBuffer_p[3] = gear;
		*(UINT16 *)(usb_sendBuffer_p + 4) = gloabSetting.settingMode.dpi_class[gear].dpi_x;
//		*(UINT16 *)(usb_sendBuffer_p + 6) = pid; 

//		OperateFlash_Read_value(0,buffer,4);
//		usb_sendBuffer_p[10] = buffer[0];
//		usb_sendBuffer_p[11] = buffer[1];
//		usb_sendBuffer_p[12] = buffer[2];
//		usb_sendBuffer_p[13] = buffer[3];
		
		USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, usb_sendBuffer_p, TRANSFER_LENGTH);

		Tool_Clear_ArrayData(receive_buff,0,(TRANSFER_LENGTH-1));
	}
#endif
//	if (ledAction)
//	{
//		if(!ProductionLine_Start_Get())
//		{
//			LedEffect_Dpi_Start(gear);
//		}
//		if (Device_Check_UsbActive())
//		{
//			LedEffect_Dpi_Start(gear);
//			LedEffect_Profile_Start(gear);
//		}	
//	}
}

void Custom_Dpi_Deal(UINT8 type)
{
	switch(type)
	{
		case CYCLE_DPI:
       gloabSetting.settingMode.dpi_gear++; 
			if (gloabSetting.settingMode.dpi_gear > gloabSetting.settingMode.dpi_max)
			{
				gloabSetting.settingMode.dpi_gear = DPI_GEAR_1;
			}
			break;
		case CU_DPI_Default:
			gloabSetting.settingMode.dpi_gear = gloabSetting.settingMode.dpi_default;
			break;
		case DPI_INCREASE:
			gloabSetting.settingMode.dpi_gear++; 

			if (gloabSetting.settingMode.dpi_gear > gloabSetting.settingMode.dpi_max)
			{
				gloabSetting.settingMode.dpi_gear = gloabSetting.settingMode.dpi_max;
			}
			break;
		case DPI_REDUCE:
			if (gloabSetting.settingMode.dpi_gear)
			{
				gloabSetting.settingMode.dpi_gear--;
			}

			if (gloabSetting.settingMode.dpi_gear > gloabSetting.settingMode.dpi_max)
			{
				gloabSetting.settingMode.dpi_gear = gloabSetting.settingMode.dpi_max;
			}

			break;
		default:
			break;
	}

	Custom_Set_DPI(gloabSetting.settingMode.dpi_gear,TRUE,TRUE,TRUE);
}

void Custom_Profile(BOOL sendFlag)
{
	OperateFlash_Store_GloabSetting();

	Custom_Load_ModeSetting();

	if (sendFlag)
	{
		Tool_Clear_ArrayData(receive_buff,0,63);

		receive_buff[0] = SELECT_WORKING_MODE;
		receive_buff[1] = WM_FW_Report;
		receive_buff[2] = gloabSetting.setting.currentMode;

		USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH);	
	}
}

void Custom_Select_Profile(void)
{
	gloabSetting.setting.currentMode++;

	if (gloabSetting.setting.currentMode > (MODE_NUM-1 ))// the 5 profile is  temporary
	{
		gloabSetting.setting.currentMode = 0;
	}
	Custom_Profile(TRUE);
	
	if (Device_Check_UsbActive())
	{
//		LedEffect_Profile_Start(gloabSetting.setting.currentMode);

	}	
}

UINT8 ClassKeyfunction(UINT8 id)
{
	UINT8 result = 0;
	if ((id == 0) || (id == 1))
	{
		result = NO_RELATIONSHIP;
	}else if ((id > 0xEF) && (id < 0xF4))
	{
		result = MACRO_KEY;
	}else if (id == 0xFE)
	{
		result = ONE_KEYBOARD_KEY;
	}else if (id == 0xFD)
	{
		result = SHORT_KEY;
	}else if ( (id > 0X7F) && (id < 0X90) )
	{
		result = ENABLE_KEY;
	}else if ((id > 0X8F) && (id < 0XA0))
	{
		result = MULTIMEDIA_KEY;
	}else if ((id > 0X9F) && (id < 0XB0))
	{
		result = HOT_KEY;
	}else if ((id > 0XAF) && (id < 0XC0))
	{
		result = MOUSE_KEY;
	}else if ((id > 0XBF) && (id < 0XD0))
	{
		result = SPECIAL_FUNCTION_KEY;
	}

	return result;	
}

UINT8 Custom_Deal_KeyboardOneKey(KEY_REGIST_T key_function,BOOL key_changed,BOOL key_status,UINT8 * p_buffer)
{
	UINT8 result= 0;
	UINT8 ii;
	UINT8 key_index;
	BOOL key_type;
	UINT8 key_code;
	UINT8 * keyboard_temp = p_buffer;


	key_index = key_function.usb_registration.content.key.key_index;

	key_code = key_index;

	if ((key_code & 0xF0) == 0xB0) // if the key is modifer key,
	{
		key_index = key_code - 0xB0;
		key_code = multifuction_table[key_index];
		key_type = SPEC_KEY;		
	}else        //STD key
	{
		key_type = STD_KEY;		
	}	

	if (key_changed) // if the key changed
	{
		result = ONE_KEYBOARD_KEY;
	}

	if (key_status == KEY_STATUS_DOWN)  // if key down
	{
		if (key_type == STD_KEY)
		{
			for (ii = 2;ii < 8;ii++)
			{
				if(!keyboard_temp[ii])
				{
					keyboard_temp[ii] = key_code;

					break;
				}else
				{
					if(keyboard_temp[ii] == key_code)
					{
						break;
					}
				}
			}
		}else if (key_type == SPEC_KEY)
		{
			keyboard_temp[0] |= key_code;
		}
	}else   // key up
	{

	}
	return result;			
}


UINT8 Custom_Deal_KeyboardShortKey(KEY_REGIST_T key_function,BOOL key_changed,BOOL key_status,UINT8 * p_buffer)
{
	UINT8 result= 0;
	UINT8 ii,nn;
	UINT8 modify_key;
	UINT8 std_key[3];
	UINT8 * keyboard_temp = p_buffer;

	modify_key  = key_function.usb_registration.content.shortcut.modify;
	std_key[0]  = key_function.usb_registration.content.shortcut.key;
	std_key[1]  = key_function.usb_registration.addContent.normalKey.key1;

	if (key_changed) // if the key changed
	{
		result = SHORT_KEY;
	}
	if (key_status == KEY_STATUS_DOWN)  // if key down
	{
		keyboard_temp[0] |= modify_key;
		for (nn = 0;nn < 2;nn++)
		{
			if (std_key[nn] != 0)
			{
				for (ii = 2;ii < 8;ii++)
				{
					if(!keyboard_temp[ii])
					{
						keyboard_temp[ii] = std_key[nn];
						break;
					}else
					{
						if(keyboard_temp[ii] == std_key[nn])
						{
							break;
						}
					}
				}
			}
		}
	}else   // key up
	{
		
	}
	return result;			
}

UINT8 Custom_Deal_MouseMediaKey(KEY_REGIST_T key_function,BOOL key_changed,BOOL key_status,UINT8 * p_buffer)
{
	UINT8 result= 0;
	UINT8 id_index = 0;


	id_index  = key_function.usb_registration.id & 0x0f;

	if (key_changed) // if the key changed
	{
		if (key_status == KEY_STATUS_DOWN)// if the key down
		{
			p_buffer[2] = (UINT8)(consumer_a_table[id_index] >> 8);
			p_buffer[1] = (UINT8)(consumer_a_table[id_index]);
		}else // if the key up
		{
			p_buffer[2] = 0;
			p_buffer[1] = 0;
		}

		result = MULTIMEDIA_KEY;
	}	

	return result;
}

UINT8 Custom_Deal_MouseHotKey(KEY_REGIST_T key_function,BOOL key_changed,BOOL key_status,UINT8 * p_buffer)
{
	UINT8 result= 0;
	UINT8 id_index;


	id_index  = key_function.usb_registration.id & 0x0f;

	if (key_changed) // if the key changed
	{
		if (key_status == KEY_STATUS_DOWN)// if the key down
		{
			p_buffer[2] = (UINT8)(consumer_b_table[id_index] >> 8);
			p_buffer[1] = (UINT8)(consumer_b_table[id_index]);

		}else // if the key up
		{
			p_buffer[2] = 0;
			p_buffer[1] = 0;
		}

		result = HOT_KEY;
	}	

	return result;
}

UINT8 Custom_Deal_MouseKey(KEY_REGIST_T key_function,BOOL key_changed,BOOL key_status,UINT8 * p_buffer)
{
	UINT8 result= 0;

	UINT8 id = 0;

	UINT8 *mouse_temp = p_buffer;

	
	id  = key_function.usb_registration.id;

	if (key_changed)
	{
		result = MOUSE_KEY;
	}

	if (key_status)// if the key down
	{
		switch(id)
		{	
		case 0xB0:
			mouse_temp[1] |= 0x01; //L
			break;
		case 0xB1:
			mouse_temp[1] |= 0x02; //R
			break;
		case 0xB2:
			mouse_temp[1] |= 0x04;//M
			break;
		case 0xB3:
			mouse_temp[1] |= 0x10;//K4
//		  DrvGPIOOutput(0,14,1) ;
//			DrvGPIOOutput(0,13,1) ;
//			DrvGPIOOutput(0,16,1) ;
			break;
		case 0xB4:
			mouse_temp[1] |= 0x08;//K5
			break;
		case 0xB5:
			if (key_changed)
			{
				mouse_temp[6] = 0x01; //WHEEL UP
			}	
			break;
		case 0xB6:
			if (key_changed)
			{
				mouse_temp[6] = 0xff;//WHEEL DOWN
			}	
			break;
		case 0xB7:
			if (key_changed)
			{
				mouse_temp[7] = 0xff;
		
				Device_LrWheel_Start(0xff);
			}	
			break;
		case 0xB8:
			if (key_changed)
			{
				mouse_temp[7] = 0x01;
	
				Device_LrWheel_Start(0x01);
			}	
			break;
		case 0XB9:
			if (key_changed)
			{
				Device_DoubleClick_Start();
			}			
			break;
		case 0XBA:
			mouse_temp[1] |= 0x40;//M1
			//UsbSendTest4Byte(0x05,0x05,0x05,0x05);
			break;
		case 0XBB:
			mouse_temp[1] |= 0x80;//M2
			//UsbSendTest4Byte(0x06,0x06,0x06,0x06);
			break;
		default:
			break;
		}	
	}else // if the key up
	{
		switch(id)
		{	
		case 0xB7:
			if (key_changed)
			{
				mouse_temp[7] = 0x00;
				Device_LrWheel_Stop();
			}	
			break;
		case 0xB8:
			if (key_changed)
			{
				mouse_temp[7] = 0x00;
				Device_LrWheel_Stop();
			}	
			break;
		default:
			break;
		}
	}

	return result;
}


UINT8 DealSpecialFunctionKey(KEY_REGIST_T key_function,BOOL key_changed,BOOL key_status,UINT8 * mouseBuf,UINT8 *keyboardBuf,UINT8 key_index)
{
#define IN_DYNAMIC_DPI 1
#define NO_IN_DYNAMIC_DPI 0

	UINT8 result= 0;
	UINT8 enable_function = 0;
	//UINT8 dpi_x,dpi_y,driver_cmd;
	UINT8 *dat = &(key_function.usb_registration.id);
	//BOOL keyStatus;

	//UINT8 *mouse_temp = mouseBuf;
	//UINT8 *keyboard_temp = keyboardBuf;
	
	enable_function  = key_function.usb_registration.id;

	if (key_status == KEY_STATUS_DOWN)// if the key down
	{
		if (key_changed) // if the key changed
		{
			switch (enable_function)         
			{
			case ONCE_DPI:
				//optical_set_dpi(dpi_x,dpi_y);
				break;
			case CYCLE_DPI:
				Custom_Dpi_Deal(CYCLE_DPI);
				break;
			case LOCK_X:
		
				break;
			case LOCK_Y:
				
				break;
			case DPI_INCREASE:
				Custom_Dpi_Deal(DPI_INCREASE);

				break;
			case DPI_REDUCE:
				Custom_Dpi_Deal(DPI_REDUCE);

				break;
			case DYNAMIC_DPI:
				
				break;
			case DRIVEN_CUSTOM:
				PC_Driver_Define(dat);
				break;
			case LED_MODE_SWITCH:
				
				break;
			case CUSTOM_TEST_CHANNEL:
				
				break;
			case SELECT_MODE:

				Onboard_And_Inline_Event_Set();
//			 	profile_timer=3000;
//        profile_keyflag=TRUE;
				break;
			case PRODUCTION_MODE_KEY:
			case PRODUCTION_MODE_LED:
				//keyStatus = 1;
				//ProductionLine_Deal(enable_function,key_index,keyStatus);
				break;
			case CU_DPI_Default:
				Custom_Dpi_Deal(CU_DPI_Default);
				break;

			case CU_Zoon_Out:

				//mouse_temp[6] = 0x01; //WHEEL UP

				Device_Zoom_OutIn_Start(0x01);

				//keyboard_temp[0] |= 0x01;

				//result = ONE_KEYBOARD_KEY;

				break;
			case CU_Zoon_In:

				//mouse_temp[6] = 0xff;//WHEEL Down

				Device_Zoom_OutIn_Start(0xff);

				//keyboard_temp[0] |= 0x01;

				//result = ONE_KEYBOARD_KEY;

				break;
			default:

				break;
			}   
		}
	}else // if the key up
	{
		if (key_changed) // if the key changed
		{
			switch (enable_function)         
			{
			case ONCE_DPI:
				
				break;
			case CYCLE_DPI:
				break;
			case LOCK_X:
			
				break;
			case LOCK_Y:
	
				break;
			case DPI_INCREASE:

				break;
			case DPI_REDUCE:

				break;
			case DYNAMIC_DPI:

				break;

			case PRODUCTION_MODE_KEY:
			case PRODUCTION_MODE_LED:
				//keyStatus = 0;
				//ProductionLine_Deal(enable_function,key_index,keyStatus);
				break;
			case CU_Zoon_Out:
			case CU_Zoon_In:
				
				//result = ONE_KEYBOARD_KEY;

				Device_Zoom_OutIn_Stop();
	
				break;
			default:

				break;
			}   
		}
	}

	return result;
}

RGB_STRU rgb_test_value[7] = 
{
	{255,0,0},{0,255,0},{0,0,255},{255,255,0},{255,0,255},{0,255,255},{255,255,255}
};

//void LED_Test_ByKey(UINT32 changed, UINT32 status)
//{
//	
//	static UINT8 rgb_num = 0;
//	if(changed & (RIGHT_BUTTON_REPORT_BIT_MASK | MIDDLE_BUTTON_REPORT_BIT_MASK))
//	{
//		if(status == (RIGHT_BUTTON_REPORT_BIT_MASK | MIDDLE_BUTTON_REPORT_BIT_MASK))
//		{
//			LED_Test_Flag = !LED_Test_Flag;
//			
//			if(LED_Test_Flag)
//			{
//				gloabSetting.settingMode.LE_value.type = LED_MODE_LIGHT_ON;
//				gloabSetting.settingMode.LE_value.typeValue.staticLed.random = 0;
//				gloabSetting.settingMode.LE_value.typeValue.staticLed.rgb = rgb_test_value[0];
//				
//				LedEffect_Start();
//				
//				gloabSetting.settingMode.dpi_class[gloabSetting.settingMode.dpi_gear].dpi_x = 0x40;
//				Custom_Set_DPI(gloabSetting.settingMode.dpi_gear, FALSE, FALSE, TRUE);
//			}
//			else
//			{
//				OperateFlash_Read_ModeSetting(gloabSetting.setting.currentMode);
//				
//				LedEffect_Start();
//				
//				Custom_Set_DPI(gloabSetting.settingMode.dpi_gear, FALSE, FALSE, TRUE);
//				
//				rgb_num = 0;
//			}
//		}
//	}
//	
//	if(LED_Test_Flag)
//	{
//		if(changed & RIGHT_BUTTON_REPORT_BIT_MASK)
//		{
//			if(status == RIGHT_BUTTON_REPORT_BIT_MASK)
//			{
//				rgb_num++;
//				
////				if(rgb_num > 43)rgb_num = 0;
//				
//				if(rgb_num == 7)rgb_num = 0;
//				
////				if(rgb_num < 43)
//					gloabSetting.settingMode.LE_value.typeValue.staticLed.rgb = rgb_test_value[rgb_num];//0xff - (rgb_num * 6);
////				else gloabSetting.settingMode.LE_value.typeValue.staticLed.rgb.red = 0;
//				
//				LedEffect_Start();
//				
//				if(rgb_num == 0)gloabSetting.settingMode.dpi_class[gloabSetting.settingMode.dpi_gear].dpi_x = 0x40;
//				
//				if(rgb_num == 1)gloabSetting.settingMode.dpi_class[gloabSetting.settingMode.dpi_gear].dpi_x = 0x50;
//				
//				if((rgb_num == 0) || (rgb_num == 1))Custom_Set_DPI(gloabSetting.settingMode.dpi_gear, FALSE, FALSE, TRUE);
//			}
//		}
//	}
//}


UINT8 Custom_HandleKey(UINT32 button_wheel,REPORT_POINTER * report_pointer)
{
	static UINT32 last_button_wheel_status;
	UINT8 result = 0, id;

	UINT32 button_wheel_changed = 0;
	UINT32 ii,mask;

	BOOL key_changed = FALSE;
	BOOL key_status = FALSE;

	UINT8 key_event_type = 0;

	BOOL mouse_multimedia_flag_temp = FALSE;
	BOOL keyboard_flag_temp = FALSE;
	BOOL mouse_flag_temp = FALSE;
	//UINT8 reserve_temp = 0;

	KEY_REGIST_T * key_fun;

	UINT8 mouse_temp[8] = {MOUSE_ID,0,0,0,0,0,0,0};
	UINT8 keyboard_temp[8] = {0,0,0,0,0,0,0,0};
	UINT8 mouse_multimedia_temp[3] = {MOUSE_MULTIMEDIA_ID,0,0};

	button_wheel_changed = (button_wheel ^ last_button_wheel_status); // which changed
	
//	if (!ProductionLine_Start_Get())
//	{
//		LED_Test_ByKey(button_wheel_changed, button_wheel);
//	}
	
	for (ii = 0,mask = 0x01;ii < CUSTOM_BUTTON_NUMBER;ii++,mask <<= 1)  //mask = 0x02 ,because L button not define
	{

		if(button_wheel_changed & mask)// check key if changed
		{
			key_changed = TRUE;
		}

		if (button_wheel & mask)   //  check key up or down
		{
			key_status = KEY_STATUS_DOWN;
		}else
		{
			key_status = KEY_STATUS_UP;
		}
//		if (key_changed && key_status && (ii != 3) && (ii != 4) && (gloabSetting.settingMode.LE_value.type == LED_MODE_REACT))
//		{
//			Led_React_Start(1);
//		}
		key_fun = &gloabSetting.settingMode.key_function[ii];
		id = ClassKeyfunction((*key_fun).usb_registration.id);
		switch (id)
		{
		case NO_RELATIONSHIP:
			break;
		case MACRO_KEY:
			key_event_type = Macro_Start(*key_fun,key_changed,key_status,ii);
			break;
		case ONE_KEYBOARD_KEY:
			key_event_type = Custom_Deal_KeyboardOneKey(*key_fun,key_changed,key_status,keyboard_temp);
			break;
		case SHORT_KEY:
			key_event_type = Custom_Deal_KeyboardShortKey(*key_fun,key_changed,key_status,keyboard_temp);
			break;
		case ENABLE_KEY:
			//key_event_type = DealEnableKey(*key_fun,key_changed,key_status,&usb_rf_enable,send_type);
			break;
		case MULTIMEDIA_KEY:
			key_event_type = Custom_Deal_MouseMediaKey(*key_fun,key_changed,key_status,mouse_multimedia_temp);
			break;
		case HOT_KEY:
			key_event_type = Custom_Deal_MouseHotKey(*key_fun,key_changed,key_status,mouse_multimedia_temp);
			break;
		case MOUSE_KEY:
			key_event_type = Custom_Deal_MouseKey(*key_fun,key_changed,key_status,mouse_temp);
			break;
		case SPECIAL_FUNCTION_KEY:
			key_event_type = DealSpecialFunctionKey(*key_fun,key_changed,key_status,mouse_temp,keyboard_temp,ii);
			break;
		default:
			break;
		}

		if ((key_event_type == MULTIMEDIA_KEY) ||  (key_event_type == HOT_KEY))
		{
			mouse_multimedia_flag_temp = TRUE;	
		}else if ((key_event_type == ONE_KEYBOARD_KEY) || (key_event_type == SHORT_KEY))
		{
			keyboard_flag_temp = TRUE;
		}else if (key_event_type == MOUSE_KEY)
		{
			mouse_flag_temp = TRUE;	
		}else if (key_event_type == Mouse_Add_Keyboard)
		{
			//mouse_flag_temp = TRUE;
			//keyboard_flag_temp = TRUE;
		}

		key_changed = FALSE; //clear the key changed flag
		key_event_type = 0;
	}

	if (mouse_multimedia_flag_temp)
	{
		result |= MULTIMEDIA_KEY_REPORT;
		Tool_Assigned_BufferToBuffer(mouse_multimedia_temp,report_pointer->multimedia_report,3);
	}
	if (keyboard_flag_temp)
	{
		result |= KEYBOARD_KEY_REPORT;
		Tool_Assigned_BufferToBuffer(keyboard_temp,report_pointer->keyboard_report,8);

	}
	if (mouse_flag_temp)
	{
		result |= MOUSE_KEY_REPORT;
		Tool_Assigned_BufferToBuffer(mouse_temp,report_pointer->mouse_report,8);
	}

	last_button_wheel_status = button_wheel;

	return result;	
	 
}



#include "pc_communicate.h"
#include "led_effect.h"
#include "custom.h"
#include "gtech_flash.h"
#include "test.h"
//#include ".\Led_Layer\light_effect_layer.h"
//#include "optical.h"
#include "operation_flash.h"
#include "led2.h"
#include "Production_line_test.h"
#include "string.h"
#include "Timer.h"
#include "gpio.h"
#include "my_electromagnet.h"
#include "my_protocol_mouse.h"

extern const RGB_STRU productionLine_ledTest[5];

UINT8 receive_buff[TRANSFER_LENGTH];

UINT8 report_continue_tag;

UINT8 test_mode = 0;

SYNC_REPORT_STRU syncReport;

extern BOOL Profile_LED_flag;
UINT32 flash_gloabSet_p;
//const  UINT8 *flash_gloabSet_p = NULL;

Profile_Mode_Stru profile_mode;
//BIG_DATA_STRU bigData_stru;

#if 1
void PC_Communicate_Pointer_Init(void)
{
	flash_gloabSet_p = inside_Flash;
}


BOOL CRC_Check(UINT8 *buffer,UINT8 firstIndex,UINT8 endIndex,UINT8 checkValue)
{
	UINT8 ii;
	UINT8 valueData = 0;
	BOOL result = FALSE;

	for (ii = firstIndex;ii < (endIndex - firstIndex + 1);ii++)
	{
		valueData += buffer[ii];
	}

	if (valueData == checkValue)
	{
		result = TRUE;
	}

	return result;
}

UINT8 CRC_Calculate(UINT8 *buffer,UINT8 firstIndex,UINT8 endIndex)
{
	UINT8 ii;
	UINT8 valueDate = 0;

	for (ii = firstIndex;ii < (endIndex + 1);ii++)
	{
		valueDate += buffer[ii];
	}

	return valueDate;
}

void PC_Sync_To_Device(UINT8 *buffer)
{
	static UINT16 sync_index;
	UINT8 profileIndex;
	UINT8 currentPakcet;
	UINT8 sumPakcet;
	UINT8 value_num;
	UINT8 *fromBuffer;
	BOOL crc;
//	MALLOC_STRU mallocValue = {NULL,FALSE};
	UINT8 *readBuf;
	UINT8 buff[2]={0,0};

	crc = CRC_Check(buffer,0,62,buffer[63]);

	///check CRC
	if (crc == FALSE)
	{
		return;
	}

	profileIndex =  buffer[1];
	currentPakcet =  buffer[2];
	sumPakcet =  buffer[3];
	value_num = buffer[4];
	fromBuffer = &buffer[5];

	///first packet
	if (buffer[2] == FIRST_PACKET)
	{
		if (profileIndex == PC_SYNC_FLAG_GLOAB_SETTING)
		{
			sync_index = 0;

		}else 
		{
			sync_index = OPERATE_MODE_SETTING_SIZE * profileIndex + GLOAB_SETTING_SIZE;
		}
	}

	GtechFlash_WriteBytes(flash_gloabSet_p,sync_index,fromBuffer,value_num);

	sync_index += value_num;


	if (currentPakcet == sumPakcet)
	{
//		mallocValue = RamMalloc_Assign(OPERATE_MODE_SETTING_SIZE);
	
		readBuf = malloc_ram;
		
		OperateFlash_Modedata_Read(profileIndex,readBuf);

		buff[0]=CRC_Calculate(readBuf,0,OPERATE_MODE_SETTING_SIZE-1);

		Gtech_MCUFlash_WriteBytes(MCU_inside_Flash,profileIndex,buff,1);
		if (profileIndex == PC_SYNC_FLAG_GLOAB_SETTING)
		{
			OperateFlash_Read_GloabSetting();

			Custom_Load_ModeSetting();

			Macro_Init_Head();

//			LedEffect_Start();
		}
//        RamMalloc_Free(readBuf, OPERATE_MODE_SETTING_SIZE);
	}
	

	Tool_Clear_ArrayData(buffer,1,63);

	buffer[1] =TRUE;

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH); 	

}


void FW_Sync_To_Pc(UINT8 * buffer)
{
	UINT8 profileIndex = buffer[1];

	report_continue_tag = GLOAB_MODE_SYNCHRONOUS_TRG;	

	syncReport.readIndex = 0;
	syncReport.curPacket = 0;

	syncReport.profileIndex = profileIndex;

	if (profileIndex == FW_SYNC_FLAG_GLOAB_SETTING)
	{
		syncReport.allBytes = GLOAB_SYNC_TO_PC_SIZE;

		syncReport.allPacket = GLOAB_SYNC_TO_PC_SIZE /GLOAB_SYNC_TO_PC_MAX_SIZE;

		if (GLOAB_SYNC_TO_PC_SIZE / GLOAB_SYNC_TO_PC_MAX_SIZE != 0)
		{
			syncReport.allPacket++;

		}

		syncReport.offsetAdr = 0;

	}else 
	{
		syncReport.allBytes = MODE_SETTING_SIZE;

		syncReport.allPacket = MODE_SETTING_SIZE /GLOAB_SYNC_TO_PC_MAX_SIZE;

		if (MODE_SETTING_SIZE / GLOAB_SYNC_TO_PC_MAX_SIZE != 0)
		{
			syncReport.allPacket++;
		}

		syncReport.offsetAdr = OPERATE_MODE_SETTING_SIZE * profileIndex + GLOAB_SETTING_SIZE;
	}

}

 

void FW_Sync_To_Pc_Continue(UINT8 *buffer)
{
	UINT8 valueNum;
	UINT16 readAddress;
	UINT8 *sendBuffer = &buffer[5];

	syncReport.curPacket++;

	if (syncReport.curPacket > syncReport.allPacket)
	{
		report_continue_tag = REPORT_NONE;
		return;
	}

	if (syncReport.readIndex >= syncReport.allBytes)
	{
		report_continue_tag = REPORT_NONE;
		return;
	}

	readAddress = syncReport.offsetAdr + syncReport.readIndex; 

	valueNum = ((syncReport.readIndex + GLOAB_SYNC_TO_PC_MAX_SIZE) >  syncReport.allBytes) 
		? (syncReport.allBytes - syncReport.readIndex) : GLOAB_SYNC_TO_PC_MAX_SIZE;

	syncReport.readIndex += valueNum;

	GtechFlash_ReadBytes(flash_gloabSet_p,readAddress,sendBuffer,valueNum);

	buffer[0] = FW_SYNC_GLOAB_SETTING;
	buffer[1] = syncReport.profileIndex;
	buffer[2] = syncReport.curPacket;
	buffer[3] = syncReport.allPacket;
	buffer[4] = valueNum;

	buffer[63] = CRC_Calculate(buffer,0,62);

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH); 	

}

void PC_ModeSetting_To_Device(UINT8 *buffer)
{
	UINT8 crc;
	UINT8 mode = buffer[1];
	UINT8 keyIndex = buffer[2];
	KEY_REGIST_T *keyRegist =(KEY_REGIST_T *)&buffer[3];

	crc = CRC_Check(buffer,0,62,buffer[63]);

	///check CRC
	if (crc == FALSE)
	{
		return;
	}
	
	if (mode >= MODE_NUM)return;
	
	OperateFlash_Read_ModeSetting(mode); //mode//gloabSetting.setting.currentMode

	gloabSetting.settingMode.key_function[keyIndex] = (*keyRegist);
	
	OperateFlash_Store_ModeSetting(mode); //mode//gloabSetting.setting.currentMode

	Tool_Clear_ArrayData(buffer,1,63);

	buffer[1] =TRUE;

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH); 	
}


void PC_Select_WorkingMode(UINT8*buffer)
{
	UINT8  dir = buffer[1];
	UINT8 workMode = buffer[2];
	
	if (dir != WM_PC_Auto)
	{
		return;
	}

	if (workMode >= MODE_NUM)
	{
		return;
	}

	gloabSetting.setting.currentMode = workMode;

	OperateFlash_Store_GloabSetting();

	Custom_Load_ModeSetting();
	
  Profile_LED_flag=TRUE;
	
	Tool_Clear_ArrayData(buffer,1,63);

	buffer[2] = workMode;
	buffer[3] = TRUE;

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH);	

}

#if 0
void PC_BigData_Switch(UINT8 *buffer)
{

	if (buffer[1] == PC_BIG_DATA_SWITCH)
	{
		bigData_stru.flag = buffer[2];
	}

	Tool_Clear_ArrayData(buffer,1,63);

	buffer[1] = 0xff;

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH);	
}


void PC_BigData_Acquire(UINT8 *mouseBuf)
{
	static UINT8 opticalCnt = 0;
	UINT8 ii;
	BOOL btnChangd = FALSE;

	W_TO_B_CONVERT x_delta;
	W_TO_B_CONVERT y_delta;

	if (!(bigData_stru.flag)  && !(OP_liftCut_run.running))
	{
		return;
	}

	x_delta.b.lo = mouseBuf[1];
	x_delta.b.hi = mouseBuf[2];
	y_delta.b.lo = mouseBuf[3];
	y_delta.b.hi = mouseBuf[4];

	bigData_stru.big_x_delta.w += x_delta.w;
	bigData_stru.big_y_delta.w += y_delta.w;

	///if button changed 
	for (ii = 0;ii < 7;ii++)
	{
		if ((ii == 0) || (ii == 5) || (ii == 6))
		{
			if (mouseBuf[ii] != bigData_stru.Data[ii])
			{
				Tool_Assigned_BufferToBuffer(mouseBuf,bigData_stru.Data,7);

				bigData_stru.Data[1] = bigData_stru.big_x_delta.b.lo;
				bigData_stru.Data[2] = bigData_stru.big_x_delta.b.hi;
				bigData_stru.Data[3] = bigData_stru.big_y_delta.b.lo;
				bigData_stru.Data[4] = bigData_stru.big_y_delta.b.hi;

				btnChangd = TRUE;

				bigData_stru.datFlag = TRUE;
			}	
		}	
	}

	///if button no changed optical changed
	if (!btnChangd)
	{
		opticalCnt++;
		if (opticalCnt > 20)//10
		{
			opticalCnt = 0;

			Tool_Assigned_BufferToBuffer(mouseBuf,bigData_stru.Data,7);

			bigData_stru.Data[1] = bigData_stru.big_x_delta.b.lo;
			bigData_stru.Data[2] = bigData_stru.big_x_delta.b.hi;
			bigData_stru.Data[3] = bigData_stru.big_y_delta.b.lo;
			bigData_stru.Data[4] = bigData_stru.big_y_delta.b.hi;

			bigData_stru.datFlag = TRUE;
		}
	}

	if (!bigData_stru.datFlag)
	{
		return;
	}


	if (OP_liftCut_run.running)
	{
		Tool_Clear_ArrayData(receive_buff,0,(TRANSFER_LENGTH-1));

		receive_buff[0] = PC_BIG_DATA;
		receive_buff[1] = OP_HI_Dat;

		Tool_Assigned_BufferToBuffer(bigData_stru.Data,&receive_buff[2],7);

		USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH);

		Tool_Clear_ArrayData(receive_buff,0,(TRANSFER_LENGTH-1));
	}

	if (bigData_stru.flag)
	{
		receive_buff[0] = PC_BIG_DATA;
		receive_buff[1] = PC_BIG_DATA_DATA;

		Tool_Assigned_BufferToBuffer(bigData_stru.Data,&receive_buff[2],7);

		USB_LoadInEP_Data(EP_3_CUSTION_OUT,receive_buff,TRANSFER_LENGTH);

		//Tool_Clear_ArrayData(receive_buff,0,(TRANSFER_LENGTH-1));
	}

	bigData_stru.big_x_delta.w = 0;
	bigData_stru.big_y_delta.w = 0;

	bigData_stru.datFlag = FALSE;
}
#endif

void PC_Driver_Define(UINT8 *buf)
{
	receive_buff[0] = DRIVER_DEFINE;
	receive_buff[1] = 0;

	Tool_Assigned_BufferToBuffer(buf,&(receive_buff[2]),5);

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH);	
}

void Performance_Setting(UINT8 *buffer)
{
	UINT8 type;
	UINT8 setupType = buffer[1];
	UINT8 gear = buffer[3];
	UINT8 reportRate = buffer[3];
	UINT8 acceleration = buffer[3];
	UINT16 dpi_x = (buffer[5] << 8) + buffer[4];//buffer[4];
	UINT16 dpi_y =(buffer[7] << 8) + buffer[6];// buffer[5];
	UINT8 * from = &buffer[2];
	UINT8 * toBuf;

	switch(setupType)
	{
	case DPI_PC_INITIATIVE:
		type = buffer[2];
		switch(type)
		{
		case DPI_Value:
//		gloabSetting.settingMode.dpi_gear = gear;
			gloabSetting.settingMode.dpi_class[gear].dpi_x = dpi_x;
			gloabSetting.settingMode.dpi_class[gear].dpi_y = dpi_y;
			break;
		case DPI_Default_Gear:
			gloabSetting.settingMode.dpi_gear = gear;
//			gloabSetting.settingMode.dpi_default = gear;
			break;
		case DPI_Max_Gear:
			gloabSetting.settingMode.dpi_max = gear;

			if (gloabSetting.settingMode.dpi_gear > gloabSetting.settingMode.dpi_max)
			{
				gloabSetting.settingMode.dpi_gear = 0;
			}
			break;
		default:
			break;
		}

		Custom_Set_DPI(gloabSetting.settingMode.dpi_gear,TRUE,FALSE,TRUE);

		buffer[2] = 1;

		break;
	case REPORT_RATE_PC_INITIATIVE:

		gloabSetting.settingMode.report_rate = reportRate;

		OperateFlash_Store_ModeSetting(gloabSetting.setting.currentMode);

		Device_Set_ReportRate(reportRate);

		buffer[2] = 1;

		break;
	case ACCELERATION_PC_INITIATIVE:
		gloabSetting.settingMode.acceleration = acceleration;

		OperateFlash_Store_ModeSetting(gloabSetting.setting.currentMode);

		buffer[2] = 1;
		break;
	case DPI_PC_Acquire:

		buffer[2] = gloabSetting.setting.currentMode;
		buffer[3] = gloabSetting.settingMode.dpi_gear;
//		buffer[4] = gloabSetting.settingMode.dpi_class[gloabSetting.settingMode.dpi_gear].dpi_x;
//		buffer[5] = gloabSetting.settingMode.dpi_class[gloabSetting.settingMode.dpi_gear].dpi_y;
		memcpy(&buffer[4],&(gloabSetting.settingMode.dpi_class[gloabSetting.settingMode.dpi_gear].dpi_x),2);

		memcpy(&buffer[6],&(gloabSetting.settingMode.dpi_class[gloabSetting.settingMode.dpi_gear].dpi_y),2);
		break;
	case DPI_Alone_x_y:
		toBuf = gloabSetting.settingMode.aloneXY;

		Tool_Assigned_BufferToBuffer(from,toBuf,6);

		OperateFlash_Store_ModeSetting(gloabSetting.setting.currentMode);
		buffer[2] = 1;

		Custom_Set_DPI(gloabSetting.settingMode.dpi_gear,TRUE,FALSE,TRUE);

		break;
	default:
		break;
	}

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH);
}

void Brightness_Control(UINT8*buffer)
{
	if (buffer[1] <= 50)
	{
//		Led_Lightness_Adjust(buffer[1]);
	}	
}



void Custom_Use_Space(UINT8 *buffer)
{
	UINT8 ii;
	UINT32 space;
	UINT8 *data;

    space = OPERATE_MODE_SETTING_SIZE;
	data = (UINT8*)&space;

	for (ii = 1;ii < 5;ii++)
	{
		buffer[ii] = data[4 - ii];
	}

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH); 


	space = OPERATE_FLASH_MAX_SPACE;
	data = (UINT8*)&space;
	for (ii = 1;ii < 5;ii++)
	{
		buffer[ii] = data[4 - ii];
	}

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH); 
}

void Op_Height_LiftCutOff(UINT8 *buffer)
{
//	UINT8 cmdType = buffer[1];
//	UINT8 type = buffer[2];
//	UINT8 pix = buffer[3];
//	UINT8 sq = buffer[4];

//	switch(cmdType)
//	{
//	case OP_Height_Start :

//		OP_LiftCutOff(type);

//		buffer[3] = 1;

//		break;
//	case Op_Height_Restore:

//		Optical_LiftCutOff_Set(pixThresh_back,min_sq_back);

//		buffer[2] = 1;

//		break;
//	case Op_Height_Get_Set:

//		Optical_LiftCutOff_Set(pix,sq);

//		buffer[5] = 1;

//		break;
//	default:
//		break;
//	}
//  if(type!=OP_H_Determine)
//	{
//	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH); 
//	}
//	
}

void Optical_LiftCutOff_Result(UINT8 ret)
{
//	Tool_Clear_ArrayData(receive_buff,0,63);

//	receive_buff[0] = OP_HEIGHT_LiftCutOff;
//	receive_buff[1] = Op_Height_result;
//	receive_buff[2] = ret;

//	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH); 	
}

void FingerPrint_Setting(UINT8 *buffer)
{
	UINT8 mode = buffer[2];
	UINT8 fp_id = buffer[3];

	switch(buffer[1])
	{
	case FP_Set_Mode_And_FingerPrint:
		if ((mode < MODE_NUM) && (fp_id < MODE_NUM))
		{
			gloabSetting.setting.FP_ID = fp_id;

			OperateFlash_Store_GloabSetting();
		}
		break;
	default:
		break;
	}
}


//void Led_Effect_Setting(UINT8 *buffer)
//{
//	static UINT16 LE_sync_index;
//	UINT8 currentPakcet;
//	UINT8 sumPakcet;
//	UINT8 value_num;
//	UINT8 *toBuf; //= (UINT8*)&gloabSetting.settingMode.LE_value;
//	UINT8 *fromBuffer;
//	BOOL crc;

//	crc = CRC_Check(buffer,0,62,buffer[63]);

//	///check CRC
//	if (crc == FALSE)
//	{
//		return;
//	}

//	currentPakcet =  buffer[2];
//	sumPakcet =  buffer[3];
//	value_num = buffer[4];
//	fromBuffer = &buffer[5];

//	///first packet
//	if (currentPakcet == FIRST_PACKET)
//	{
//		LE_sync_index = 0;	

////		LedEffect_Group_Stop();
//	}

//	toBuf = (UINT8*)((UINT8*)&gloabSetting.settingMode.LE_value+ LE_sync_index);

//	Tool_Assigned_BufferToBuffer(fromBuffer,toBuf,value_num);

//	LE_sync_index += value_num;

//	if (currentPakcet == sumPakcet)
//	{

//		OperateFlash_Store_ModeSetting(gloabSetting.setting.currentMode);

//		LedEffect_Start();
//	}


//	Tool_Clear_ArrayData(buffer,4,63);

//	buffer[1] =TRUE;
//	buffer[2] =currentPakcet;

//	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH); 	

//	//LedEffect_Start();
//}

//void PC_Production_Test(UINT8 *buffer)
//{
//	UINT8 index = buffer[1];

//	if (index < 4)
//	{
//		Led_EffectCode_Stop();

//		ProductionLine_Start_Set();
//		
//		switch(index)
//		{
//			case 0:
//				DrvGPIOOutput(0, 19, 0);
//				DrvGPIOOutput(0, 12, 1);
//				DrvGPIOOutput(0, 18, 1);
//				break;
//			case 1:
//				DrvGPIOOutput(0, 19, 1);
//				DrvGPIOOutput(0, 12, 0);
//				DrvGPIOOutput(0, 18, 1);
//				break;
//			case 2:
//				DrvGPIOOutput(0, 19, 1);
//				DrvGPIOOutput(0, 12, 1);
//				DrvGPIOOutput(0, 18, 0);
//				break;
//			case 3:
//				DrvGPIOOutput(0, 19, 0);
//				DrvGPIOOutput(0, 12, 0);
//				DrvGPIOOutput(0, 18, 0);
//				break;
//			default:
//				DrvGPIOOutput(0, 19, 0);
//				DrvGPIOOutput(0, 12, 0);
//				DrvGPIOOutput(0, 18, 0);
//				break;
//		}
//		
//		PWM_set_duty(productionLine_ledTest[index], LED_MODE_LIGHT_ON);
//		
//		buffer[2] = TRUE;
//	}
//	else if(index == 0xff)
//	{
//		ProductionLine_Start_Clear();
//	
//		Custom_ModeParamter_Init(gloabSetting.setting.currentMode);

//		Custom_Load_ModeSetting();		
//		
//		buffer[2] = TRUE;
//	}

//	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH); 
//}

void PC_CodeCompileTimer(UINT8 *buffer)
{
	OperateFlash_CodeCompileTimer_Read(&buffer[1]);

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH);	
}

void LED_Effect_Remain_Setting(UINT8 *buffer)
{
	UINT8 type = buffer[1];
	UINT8 gcc = buffer[2];
	UINT8 ledOff = buffer[2];

	switch(type)
	{
	case Remain_Set_Brightness:
		
//		Led_Adjust_Brightness(gcc);

		gloabSetting.settingMode.brightness = gcc;

		OperateFlash_Store_ModeSetting(gloabSetting.setting.currentMode);
		
		break;
	case Remain_Set_LedOff:

//		Led_Enter_LefOff_Set(ledOff);

		gloabSetting.settingMode.enter_LefOff = ledOff;

		OperateFlash_Store_ModeSetting(gloabSetting.setting.currentMode);

		break;
	default:
		break;
	}

	buffer[3] = 1;

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH);	
}

//Profile_Mode_Stru profile_mode;

void Onboard_And_Inline_Event_Set(void)
{
	profile_mode.flag = TRUE;
	profile_mode.cnt = 500;

	Tool_Clear_ArrayData(receive_buff,0,63);

	receive_buff[0] =Onboard_And_Inline;
	receive_buff[1] =Prof_Inquire;

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH); 	
}


void Onboard_And_Inline_ProfileCnt(void)
{
	if (!profile_mode.flag)
	{
		return;
	}

	if (!profile_mode.cnt)
	{
		return;
	}

	if (profile_mode.cnt)profile_mode.cnt--;

	if (!profile_mode.cnt)
	{
		profile_mode.flag = FALSE;
		Custom_Select_Profile();	
	}

 
}

BOOL Onboard_And_Inline_Check_ProfMode_IfInOnboard(void)
{
	return 0;
}

void Onboard_And_Inline_Profile_Set(UINT8 *buffer)
{
	UINT8 type = buffer[1];
	UINT8 mode = buffer[2];

	switch(type)
	{
	case Prof_Inquire:
		switch(mode)
		{
		case Prof_Onboard:
			if (profile_mode.flag)
			{
				if (profile_mode.cnt)
				{
					//Custom_Select_Profile();	
				}

				profile_mode.flag = FALSE;
			}
			break;
		case Prof_Inline:
			profile_mode.flag = FALSE;
			break;
		default:
			break;
		}

		break;
	default:
		break;
	}

	Tool_Clear_ArrayData(buffer,3,63);

	buffer[0] =Onboard_And_Inline;
	buffer[1] =type;
	buffer[2] =TRUE;

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH); 	
}

void Profile_Modify_Name(UINT8 *buffer)
{
	UINT8 * tobuf = (UINT8*)(gloabSetting.settingMode.name);

	Tool_Assigned_BufferToBuffer(&buffer[2],tobuf,Profile_Name_Len);

	OperateFlash_Store_ModeSetting(gloabSetting.setting.currentMode);

	Tool_Clear_ArrayData(buffer,2,63);

	buffer[2] =TRUE;

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH); 
}

void PC_Communicate(void)
{
	BOOL crc;
	Tool_Clear_ArrayData(receive_buff,0,(TRANSFER_LENGTH-1));

	if (Usb_Read_Bytes(EP_4_CUSTION_IN,receive_buff,TRANSFER_LENGTH))
	{

	}else
	{
		switch(report_continue_tag)
		{
		case GLOAB_MODE_SYNCHRONOUS_TRG:			
			FW_Sync_To_Pc_Continue(receive_buff);
			break;
		case REPORT_MACRO_NAME:
			Macro_Report_Name(receive_buff);
			break;
		case REPORT_MACRO_VALUE:
			Macro_Report_Value(receive_buff);
			break;
		case LAYER_UpLoad_Trg:
			//PC_Layer_Upload_Continue(receive_buff);
			break;
		default:
			break;
		}

		Tool_Clear_ArrayData(receive_buff,0,(TRANSFER_LENGTH-1));

		return;
	}

	switch(receive_buff[COMMAND])
	{

	//////////////general setting///////////////////
	case PC_SYNC_GLOAB_SETTING:
		PC_Sync_To_Device(receive_buff);
		break;

	case PC_MODE_SETTING:							
		PC_ModeSetting_To_Device(receive_buff);
		break;

	case FW_SYNC_GLOAB_SETTING: 
		FW_Sync_To_Pc(receive_buff);
		break;

	case PERFORMANCE_REALIVE_SETTING: 
//		Performance_Setting(receive_buff);
		test_mode = receive_buff[2];
		break;

	case SELECT_WORKING_MODE: 
		PC_Select_WorkingMode(receive_buff);
		break;

	case FW_RESTORE_DEFAULT:
		Custom_Restore_Default(receive_buff);
		break;
	case Onboard_And_Inline:
		Onboard_And_Inline_Profile_Set(receive_buff);
		break;
	case PC_Profile_Name_Modify:
		Profile_Modify_Name(receive_buff);
		break;

	////////////////  other/////////////////////
	case PC_CMD_BOOTLOADER:							
    Ele_DisableVibrate();
		Bootloadable_Load();
		break;

	case CUSTOM_Use_Space:
		Custom_Use_Space(receive_buff);
		break;

	case Code_Compile_Timer_D:
		PC_CodeCompileTimer(receive_buff);
		break;

	////////////////  Other function/////////////////////
	case PC_BIG_DATA:							
//		PC_BigData_Switch(receive_buff);
		break;

	case OP_HEIGHT_LiftCutOff:							
		Op_Height_LiftCutOff(receive_buff);
		break;

	case FP_FingerPrint_Setting:
		FingerPrint_Setting(receive_buff);
		break; 

	case LED_Effect_Setting:
//		Led_Effect_Setting(receive_buff);
		break;

	case LED_Effect_Remain_Set:
//		LED_Effect_Remain_Setting(receive_buff);
		break;

	////////////////  TEST/////////////////////
	case TEST_CMD:
		//Tool_USB_ArrayData(receive_buff,64);
  
    Protocol_ProcessReceiveData(receive_buff, 64);
		break;

	case LED_TEST_CMD:
//		Led_Structure_Test(receive_buff);
		break;

	case Production_Test_CMD:
//		PC_Production_Test(receive_buff);
		break;
	case Entry_U_CMD	:
    Chip_IAP_ReinvokeISP();
    break;	
	case Enable_ISP_CMD:
		write_crp_lv(receive_buff);
	break;

	////////////////  macro deal/////////////////////*
	case MACRO_BLOCK_CHECK_SPACE:
		Macro_Get_FreeSpace(receive_buff);
		break;
	case MACRO_BLOCK_GET_AUTHORITY:
		Macro_Get_Authority(receive_buff);
		break;
	case MACRO_BLOCK_DOWNLOAD:
		Macro_DownLoad(receive_buff);
		break;
	case MACRO_BLOCK_GET_INFOR:
		Macro_Get_Information(receive_buff);
		break;
	case MACRO_BLOCK_DELETE:
		Macro_Delete(receive_buff);
		break;	

	////////////////  LED effect/////////////////////
/*
	case LAYER_Related_Instruct:
		PC_Layer_Related_Instruct(receive_buff);
		break;
	case LAYER_DOWNLOAD:
		PC_Layer_Download(receive_buff);
		break;
	case LAYER_Upload:
		PC_Layer_Upload(receive_buff);
		break;
*/

	default:
		break;
		
	}

	switch(receive_buff[2])
	{
	 	case PC_CMD_BOOTLOADER:		
	  crc = CRC_Check(receive_buff,0,62,0x11);
		///check CRC
		if (crc == FALSE)
		{
			return;
		}		
    Ele_DisableVibrate();
		Bootloadable_Load();
		break;
	  default:
		break;
	}
	
	Tool_Clear_ArrayData(receive_buff,0,(TRANSFER_LENGTH-1));
}

void PC_Communicate_Test(void)
{
	UINT8 ii;
	for (ii = 0;ii < 64;ii++)
	{
		receive_buff[ii] = ii;
	}
	Tool_USB_ArrayData(receive_buff,64);
}


#endif


/*
void PC_Layer_Related_Instruct(UINT8*buffer)
{
	UINT8 layerIndex;

	if (buffer[1] == LAYER_Space_Inquire)
	{
		Tool_Clear_ArrayData(buffer,2,63);

		buffer[2] = (UINT8)(LIGHT_EFFECT_SPACE >> 8);
        buffer[3] = (UINT8)(LIGHT_EFFECT_SPACE);
        
        buffer[4] = (UINT8)(MALLOC_MAX >> 8);
        buffer[5] = (UINT8)(MALLOC_MAX);
	
		USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH); 

	}else if (buffer[1] == LAYER_Specified_Operate)
	{
		layerIndex = buffer[2];

		if (layerIndex < LE_MAX_LAYER)
		{
			Custom_Set_LayerMessage(layerIndex);

			OperateFlash_SwitchLayer_BaseAddr(layerIndex);

			buffer[3] = TRUE;
		}else
		{			
			buffer[3] = FALSE;
		}

		Tool_Clear_ArrayData(buffer,4,63);
		
		USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH); 

	}else if (buffer[1] == LAYER_Perform)
	{
		layerIndex = buffer[2];

		if (layerIndex < LE_MAX_LAYER)
		{
			OperateFlash_SwitchLayer_BaseAddr(layerIndex);

		LightEffect_Layer_Generate();

			buffer[3] = TRUE;

		}else
		{
			buffer[3] = FALSE;
		}
	
		Tool_Clear_ArrayData(buffer,4,63);

		USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH); 
	}
}

void PC_Malloc_Replay_Result(void)
{
	BOOL result;

	result = RamMalloc_Get_Usage();

	receive_buff[0]	 = Led_Layer_GenerateResult;
	receive_buff[1]	 = result;

	Tool_Clear_ArrayData(receive_buff,2,63);

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH); 
}

void PC_Layer_Download(UINT8*buffer)
{
	static UINT16 layerDownd_index;
	UINT16 curPacketNum;
	UINT16 sumPacketNum;
	UINT8 value_num;
	UINT8 *fromBuffer;
	BOOL crc,lastPacket = FALSE;
	RGB_STRU RGB = {0,0,0};

	crc = CRC_Check(buffer,0,62,buffer[63]);

	///check CRC
	if (crc == FALSE)
	{
		return;
	}

	curPacketNum = (((UINT16)buffer[1] << 8) + (UINT16)buffer[2]); 
	sumPacketNum = (((UINT16)buffer[3] << 8) + (UINT16)buffer[4]); 
	value_num = buffer[5];
	fromBuffer = &buffer[6];

	///first packet
	if (curPacketNum == FIRST_PACKET)
	{
		LedEffect_Put_StaticColor(RGB,TRUE,LED_ALL_SEND);//clear rgb data

//		LightEffect_GenerateResult_Clear();

		RamMalloc_Init();

		layerDownd_index = 0;
	}

	OperateFlash_Write_LightEffect_Layer(layerDownd_index,fromBuffer,value_num);

	layerDownd_index += value_num;

	if (curPacketNum == sumPacketNum)
	{
		Custom_Store_LayerSize(layerDownd_index);

		lastPacket = TRUE;
	}

	receive_buff[0]	 = LAYER_DOWNLOAD;
	receive_buff[1]	 = 1;  //lazy for changed !!!!!!
	receive_buff[2]	 = 1; //lazy for changed  !!!!!!
	receive_buff[3]	 = lastPacket;

	Tool_Clear_ArrayData(receive_buff,4,63);

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH); 
}
*/

/*
void PC_Layer_Upload(UINT8*buffer)
{
	UINT16 sumSize;

	report_continue_tag = LAYER_UpLoad_Trg;	

	syncReport.readIndex = 0;
	syncReport.curPacket = 0;

	sumSize = Custom_Get_LayerMessage().layer_size;

	syncReport.allBytes = sumSize;

	syncReport.allPacket = sumSize /LAYER_UpLoadPack_MaxSize;

	if (sumSize / LAYER_UpLoadPack_MaxSize != 0)
	{
		syncReport.allPacket++;
	}
}



void PC_Layer_Upload_Continue(UINT8 *buffer)
{
	UINT8 valueNum;
	UINT16 readAddress;
	UINT8 *sendBuffer = &buffer[4];

	syncReport.curPacket++;

	if (syncReport.curPacket > syncReport.allPacket)
	{
		report_continue_tag = REPORT_NONE;
		return;
	}

	if (syncReport.readIndex >= syncReport.allBytes)
	{
		report_continue_tag = REPORT_NONE;
		return;
	}

	readAddress = syncReport.readIndex; 

	valueNum = ((syncReport.readIndex + LAYER_UpLoadPack_MaxSize) >  syncReport.allBytes) 
		? (syncReport.allBytes - syncReport.readIndex) : LAYER_UpLoadPack_MaxSize;

	syncReport.readIndex += valueNum;

	OperateFlash_Read_LightEffect_Layer(readAddress,sendBuffer,valueNum);

	buffer[0] = LAYER_Upload;
	buffer[1] = syncReport.curPacket;
	buffer[2] = syncReport.allPacket;
	buffer[3] = valueNum;

	buffer[63] = CRC_Calculate(buffer,0,62);

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH); 	

}
*/


/*
void SYNC_Get_Set_Flag(UINT8 *buffer)
{
	UINT8 dirct = buffer[1];

	if (dirct == SYNC_Flag_Set)
	{
		Custom_Set_SyncData(&buffer[2]);

	}else if (dirct == SYNC_Flag_Get)
	{
		Custom_Get_SyncData(&buffer[2]);
	}

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH); 
}
*/

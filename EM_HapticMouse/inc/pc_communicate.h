#ifndef _PC_COMMUNICATE_H_
#define _PC_COMMUNICATE_H_


#include "gloab.h"
#include "test.h"
//#include "macro.h"
#include "optical_3389.h"

#define COMMAND		0

enum{
	PC_SYNC_GLOAB_SETTING		= 0X01,
	PC_MODE_SETTING				= 0X02,
	FW_SYNC_GLOAB_SETTING		= 0X03,
	PERFORMANCE_REALIVE_SETTING	= 0X04,
	SELECT_WORKING_MODE			= 0X05,
	DRIVER_DEFINE				= 0X06,
	FW_RESTORE_DEFAULT			= 0X07,
	Onboard_And_Inline			= 0x08,
	PC_Profile_Name_Modify      = 0X09,
	

	PC_CMD_BOOTLOADER			= 0X11,
	PC_CMD_LIGHTEFFECT			= 0X12,
	CUSTOM_Use_Space			= 0X13,
	Code_Compile_Timer_D		= 0X14,

	PC_BIG_DATA					= 0X20,
	PRODUCTION_LINE_TEST_MODE	= 0X21,
	OP_HEIGHT_LiftCutOff		= 0X22,
	FP_FingerPrint_Setting      = 0X23,
	SYNC_Flag					= 0x24,
	LED_Effect_Setting			= 0X25,
	LED_Effect_Remain_Set		= 0X26,

	TEST_CMD					= 0X55,
	LED_TEST_CMD				= 0X56,
	Production_Test_CMD			= 0X57,
	Entry_U_CMD					= 0X58,
	Enable_ISP_CMD				= 0X59,
	//LED_Enter_SpicialMode		= 0XEA,
	//Led_Brightness_Control		= 0XEB,
	Led_Layer_GenerateResult	= 0XEC,
	LAYER_Upload				= 0XED,
	LAYER_DOWNLOAD				= 0XEE,
	LAYER_Related_Instruct		= 0XEF,

	MACRO_BLOCK_CHECK_SPACE		= 0XFF,
	MACRO_BLOCK_GET_AUTHORITY	= 0XFE,
	MACRO_BLOCK_DOWNLOAD		= 0XFD,
	MACRO_BLOCK_GET_INFOR		= 0XFC,
	MACRO_BLOCK_DELETE			= 0XFB,

};

enum{
	REPORT_NONE  = 0, 
	REPORT_MACRO_VALUE	= 1,
	REPORT_MACRO_NAME	= 2,
	GLOAB_MODE_SYNCHRONOUS_TRG  = 3, 
	REPORT_MACRO_DOWN	= 4,
	LAYER_UpLoad_Trg    = 5,
};


enum{
	DPI_FW_INITIATIVE = 0X00,
	DPI_PC_INITIATIVE = 0X01,
	REPORT_RATE_PC_INITIATIVE = 0X02,
	ACCELERATION_PC_INITIATIVE = 0X03,
	DPI_PC_Acquire = 0X04,
	DPI_Alone_x_y = 0X05,
};

enum{
	DPI_Value = 0X00,
	DPI_Default_Gear = 0X01,
	DPI_Max_Gear = 0X02
};


enum{
	PC_SYNC_FLAG_GLOAB_SETTING	= 0Xff,
	PC_SYNC_FLAG_SINGLE_MODE	= 0X01,

};

enum{
	FW_SYNC_FLAG_GLOAB_SETTING	= 0Xff,
	FW_SYNC_FLAG_SINGLE_MODE	= 0X01,

};

enum{
	PC_SYNC_MODE_SETTING_0 = 0X00,
	PC_SYNC_MODE_SETTING_1 = 0X01,
};

enum{
	PC_BIG_DATA_SWITCH  = 0, 
	PC_BIG_DATA_DATA	= 1,
	OP_HI_Dat			= 2,
};

enum{
	PC_LAYER_INQUIRE_SPACE  = 0, 
	LAYER_LESS_SPACE		= 1,
};

enum{
	WM_PC_Auto   = 0, 
	WM_FW_Report = 1,
};

enum{
	OP_Height_Start  = 0, 
	Op_Height_result = 1,
	Op_Height_Restore = 2,
	Op_Height_Get_Set = 3,
};

enum{
	OP_Height_Set  = 0, 
	Op_Height_Return = 1,
};

enum{
	FP_Set_Mode_And_FingerPrint  = 0, 
};

enum{
	LAYER_Space_Inquire		 = 0, 
	LAYER_Specified_Operate  = 1, 
	LAYER_Perform			 = 2, 
};

enum{
	SYNC_Flag_Set = 0,
	SYNC_Flag_Get = 1,
};

enum{
	Remain_Set_Brightness = 0,
	Remain_Set_LedOff	  = 1,
};


enum{
	Prof_Set			  = 0,
	Prof_Inquire		  = 2,
};

enum{
	Prof_Onboard		  = 0,
	Prof_Inline			  = 1,
};

enum{
	Restore_DefineKey		= 1,
	Restore_Perform			= 2,
	Restore_LedEffect		= 3,
	Restore_Profile			= 4,
	Restore_AllProfile		= 5,
};

#define FIRST_PACKET	1



typedef struct
{
	UINT8 curPacket;
	UINT8 allPacket;
	UINT16 readIndex;
	UINT16 allBytes;
	UINT32  offsetAdr;
	UINT8 profileIndex;
}SYNC_REPORT_STRU;

//typedef struct
//{
//	BOOL flag;
//	BOOL datFlag;
//	UINT8 Data[7];
//	W_TO_B_CONVERT big_x_delta;
//    W_TO_B_CONVERT big_y_delta;
//}BIG_DATA_STRU;


typedef struct  
{
	BOOL flag;
	UINT16 cnt;
	//UINT8 tmpProfle;
}Profile_Mode_Stru;




#define GLOAB_SYNC_TO_PC_SIZE (GLOAB_SETTING_SIZE + (MODE_SETTING_SIZE * MODE_NUM))
#define GLOAB_SYNC_TO_PC_MAX_SIZE 58


#define LAYER_UpLoadPack_MaxSize	59

extern UINT8 receive_buff[TRANSFER_LENGTH];

extern UINT8 report_continue_tag;

extern UINT8 test_mode;

void PC_Communicate(void);
void PC_Communicate_Pointer_Init(void);
UINT8 CRC_Calculate(UINT8 *buffer,UINT8 firstIndex,UINT8 endIndex);
void PC_Communicate_Test(void);
void PC_BigData_Acquire(UINT8 *mouseBuf);

void PC_Malloc_Replay_Result(void);
void PC_Driver_Define(UINT8 *buf);

void Optical_LiftCutOff_Result(UINT8 ret);
BOOL Onboard_And_Inline_Check_ProfMode_IfInOnboard(void);

void Onboard_And_Inline_ProfileCnt(void);
void Onboard_And_Inline_Event_Set(void);

#endif 

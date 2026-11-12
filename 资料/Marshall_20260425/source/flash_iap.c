/*
 * flash_iap.c
 *
 */
#include <stdint.h>
#include <fsl_iap.h>
#include <Flash_iap.h>
#include <usb_misc.h>
#include <hall_spi.h>
#include <led.h>
#include <hid_keyboard.h>
#include <driverapp_communication.h>
#include <flash_zb25v.h>
/*******************************************************************************
 * DEFINE
 ******************************************************************************/
#define BUFFER_LEN 512//512 / 4
/*******************************************************************************
 * Variables
 ******************************************************************************/
DeviceVariableData   g_variableData = {0};
flash_config_t flashInstance;
//-----------------------------------------------------------------------------------------------------
uint32_t MCUFlashBlockBase  	= 0;
uint32_t MCUFlashTotalSize   	= 0;
uint32_t MCUFlashSectorSize  	= 0;
uint32_t MCUFlashPageSize    	= 0;
//-----------------------------------------------------------------------------------------------------
uint8_t Tick_UpdateMCUFlash = 0;
uint8_t Flag_MCUFlash_WriteResult = 0;
uint8_t Flag_MCUFlash_WRFail_Cnt = 0;
uint8_t Flag_MCUFlash_PageIndex = 0;;
/*******************************************************************************
 * FUNCTION
 ******************************************************************************/
void error_trap(void);
void verify_status(status_t status);
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Report_RawData
 * Description  	: Send raw data to test APP.
 *
 * END ****************************************************************************************************************/
void Flash_IntervalFlash_Init(void)
{
	static uint32_t status;
	uint32_t destAdrss;
    /* Initialize flash driver */
    //usb_echo("Initializing flash driver...\r\n");
    if (FLASH_Init(&flashInstance) == kStatus_Success)
    {
        //------------------------------------------------------------------------------------------
    	#if UART_DEBUG
			#if 0
				usb_echo("Flash init successfull!!\r\n");
			#endif
    	#endif
    	//------------------------------------------------------------------------------------------
    }
    else
    {
        //------------------------------------------------------------------------------------------
    	#if UART_DEBUG
			#if 0
				usb_echo("Flash init FAIL!!\r\n");
			#endif
    	#endif
    	//------------------------------------------------------------------------------------------
        //error_trap();
    }
    //------------------------------------------------------------------------------------------
    /* Get flash properties kFLASH_ApiEraseKey */
    FLASH_GetProperty(&flashInstance, kFLASH_PropertyPflashBlockBaseAddr, &MCUFlashBlockBase);
    FLASH_GetProperty(&flashInstance, kFLASH_PropertyPflashSectorSize, &MCUFlashSectorSize);
    FLASH_GetProperty(&flashInstance, kFLASH_PropertyPflashTotalSize, &MCUFlashTotalSize);
    FLASH_GetProperty(&flashInstance, kFLASH_PropertyPflashPageSize, &MCUFlashPageSize);
    //------------------------------------------------------------------------------------------
	#if UART_DEBUG
		#if 0
			usb_echo("MCUFLASH_BlockBaseAddr = 0x%X\r\n", MCUFlashBlockBase);
			usb_echo("MCUFLASH_SectorSize = %x\r\n", MCUFlashSectorSize);
			usb_echo("MCUFLASH_TotalSize = %x\r\n", MCUFlashTotalSize);
			usb_echo("MCUFLASH_PageSize = 0x%X\r\n", MCUFlashPageSize);
		#endif
	#endif
	//------------------------------------------------------------------------------------------
	/* 查询现在用的是哪一个Page在保存数据 */
	Flag_MCUFlash_PageIndex = PAGE_INDEX510_FROM_END;
	//---------------------------------------------------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Report_RawData
 * Description  	: Send raw data to test APP.
 *
 * END ****************************************************************************************************************/
static void Flash_IntervalFlash_Read(uint8_t *buffer, uint32_t Sector_Index, uint32_t Page_len)
{
    static uint32_t status;
    uint32_t destAdrss; /* Address of the target location */
    uint32_t failedAddress, failedData;
    //-----------------------------------------------------------------------------------------------------
    /* Print flash information - PFlash. */
    destAdrss = MCUFlashBlockBase + (MCUFlashTotalSize - (Sector_Index * MCUFlashPageSize));
    //-----------------------------------------------------------------------------------------------------
    /* Check if the flash page is blank before reading to avoid hardfault. */
    status = FLASH_VerifyErase(&flashInstance, destAdrss, MCUFlashPageSize);
	#if 0
	usb_echo("status = %x\n",status);
	#endif
	//-----------------------------------------------------------------------------------------------------
    if (status == kStatus_Success)
    {
    	/* status = kStatus_Success means this flash region is all 0xFF, and the area has not been written before or
    	 * has just been erased successful. */
    	//usb_echo("Error: trying to Read blank flash page!\n");
    	g_variableData.varInitFlag = 0xFF;
        //error_trap();
    }
    else
    {
    	//-----------------------------------------------------------------------------------------------------
		/* Read data
		 * Flash Data can be read only when the flash page is not blank. Otherwise, a hardfault will occur, causing the program to stop. */
		FLASH_Read(&flashInstance, destAdrss, (uint8_t *)buffer, Page_len);
		//-----------------------------------------------------------------------------------------------------
    }
    //-----------------------------------------------------------------------------------------------------
}
/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Report_RawData
 * Description  	: Send raw data to test APP.
 *
 * END ****************************************************************************************************************/
uint8_t Flash_IntervalFlash_Write(uint8_t *buffer, uint32_t Sector_Index, uint32_t Page_len)
{
    static uint32_t status;
    uint32_t destAdrss; /* Address of the target location */
    uint32_t failedAddress, failedData;
    //-----------------------------------------------------------------------------------------------------
    /* Print flash information - PFlash. */
    destAdrss = MCUFlashBlockBase + (MCUFlashTotalSize - (Sector_Index * MCUFlashPageSize));
    //------------------------------------------------------------------------------------------
 	#if 0
 	usb_echo("destAdrss = %x\n",destAdrss);
 	#endif
 	//------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------
    /* erase */
    status = FLASH_Erase(&flashInstance, destAdrss, MCUFlashPageSize, kFLASH_ApiEraseKey);
    verify_status(status);
    //-----------------------------------------------------------------------------------------------------
    /* Verify if the given flash range is successfully erased. */
    status = FLASH_VerifyErase(&flashInstance, destAdrss, MCUFlashPageSize);
    verify_status(status);
    if (status == kStatus_Success)
    {
        //------------------------------------------------------------------------------------------
    	#if 0
    	usb_echo("FLASH Verify erase successful!\n");
    	#endif
    	//------------------------------------------------------------------------------------------
    }
    else
    {
    	return MCUFlash_WR_EraseFail;
    	/* 例程里面用error_trap这个函数把出现异常错误的时候卡在error_trap里面的while(1)，这里去掉，用Result看结果，防止它在这里卡死. */
        //error_trap();
    }
    //-----------------------------------------------------------------------------------------------------
    /* Start programming specified flash region */
    status = FLASH_Program(&flashInstance, destAdrss, (uint8_t *)buffer, Page_len);
    verify_status(status);
    //-----------------------------------------------------------------------------------------------------
    /* Verify if the given flash region is successfully programmed with given data */
    status = FLASH_VerifyProgram(&flashInstance, destAdrss, Page_len, (const uint8_t *)buffer, &failedAddress,&failedData);
    verify_status(status);
    if (status == kStatus_Success)
    {
        //------------------------------------------------------------------------------------------
    	#if 0
    	usb_echo("FLASH Verify Program successful!\n");
    	#endif
    	//------------------------------------------------------------------------------------------;
    }
    else
    {
    	return MCUFlash_WR_ProgramFail;
    	/* 例程里面用error_trap这个函数把出现异常错误的时候卡在error_trap里面的while(1)，这里去掉，用Result看结果，防止它在这里卡死. */
        //error_trap();
    }
    return MCUFlash_WR_Success;
    //-----------------------------------------------------------------------------------------------------
}

void verify_status(status_t status)
{
    char *tipString = "Unknown status";
    switch (status)
    {
        case kStatus_Success:
            tipString = "Done.";
            break;
        case kStatus_InvalidArgument:
            tipString = "Invalid argument.";
            break;
        case kStatus_FLASH_AlignmentError:
            tipString = "Alignment Error.";
            break;
        case kStatus_FLASH_AccessError:
            tipString = "Flash Access Error.";
            break;
        case kStatus_FLASH_CommandNotSupported:
            tipString = "This API is not supported in current target.";
            break;
        default:
            break;
    }
    //usb_echo("%s\r\n\r\n", tipString);
}

/*
 * @brief Gets called when an error occurs.
 */
void error_trap(void)
{
	usb_echo("\r\n\r\n\r\n\t---- HALTED DUE TO FLASH ERROR! ----");
    while (1)
    {
    }
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Report_RawData
 * Description  	: Send raw data to test APP.
 *
 * END ****************************************************************************************************************/
void Variable_init(void)
{
	//g_variableData.varInitFlag = 0xFF;
	//Flash_IntervalFlash_Write((uint8_t *)&g_variableData, PAGE_INDEX500_FROM_END, PAGE_LEN);
	//g_variableData.varInitFlag = 0x00;
	//------------------------------------------------------------------------------------------
	Flash_IntervalFlash_Read((uint8_t *)&g_variableData, Flag_MCUFlash_PageIndex, PAGE_LEN);
	//Flash_IntervalFlash_Write((uint8_t *)&g_variableData, Flag_MCUFlash_PageIndex, PAGE_LEN);
	//------------------------------------------------------------------------------------------
	//usb_echo("g_variableData.varInitFlag = %x\n",g_variableData.varInitFlag);
	if(g_variableData.varInitFlag == 0xFF)
	{
		//------------------------------------------------------------------------------------------
		//先让它每次都来这
		g_variableData.varInitFlag = 0xFF;
		//g_variableData.varInitFlag = 0xAA;
		g_variableData.PageIndex = Flag_MCUFlash_PageIndex;
		//------------------------------------------------------------------------------------------
		/* Led第一次上电 */
		g_variableData.LED_ONOFF = 1;
		g_variableData.LED_MODE = LED_2_mode;
		g_variableData.LED_BRIGHTNESS = 9;
		g_variableData.LED_SPEED = 2;
		g_variableData.LED_COLOR[0] = 0xFF;
		g_variableData.LED_COLOR[1] = 0xFF;
		g_variableData.LED_COLOR[2] = 0xFF;
		g_variableData.LED_RGB_MODE = 1;
		g_variableData.LIGHT_BEACON = 0;
		g_variableData.LedColor_Index = 0;
		//------------------------------------------------------------------------------------------
		/* 第一次上电初始化驱动 */
		g_variableData.DriverAPP_WASD_Switch = 0;
		DriverAPP_WASD_Switch = g_variableData.DriverAPP_WASD_Switch;
		g_variableData.DriverAPP_WinMacMode = 0;
		DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
		g_variableData.DriverAPP_WinLock_Switch = 0;
		DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
		g_variableData.DriverAPP_BottomTriggerState = 0;
		DriverAPP_BottomTriggerState = g_variableData.DriverAPP_BottomTriggerState;
		g_variableData.DriverAPP_Nkey_Rollover_Switch = 0;
		DriverAPP_Nkey_Rollover_Switch = g_variableData.DriverAPP_Nkey_Rollover_Switch;
		g_variableData.Profile_Index = 0;
		//----------------------------------------------------------------------------------------------------------------------
		for(uint8_t i = 0; i < TRANSFER_SIZE; i++)
		{
			/* 存储原始码值的矩阵 */
			g_DrivetData_KeyMatrix.KeyMatrix_Buffer[0][i] = KeyBoardLayout_NormalCode[0][i];
			g_DrivetData_KeyMatrix.KeyMatrix_Buffer[1][i] = KeyBoardLayout_NormalCode[1][i];
			g_DrivetData_KeyMatrix.KeyMatrix_Buffer[2][i] = KeyBoardLayout_NormalCode[2][i];
			g_DrivetData_KeyMatrix.KeyMatrix_Buffer[3][i] = KeyBoardLayout_NormalCode[3][i];
			g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][i] = KeyBoardLayout_NormalCode[4][i];
			g_DrivetData_KeyMatrix.KeyMatrix_Buffer[5][i] = KeyBoardLayout_NormalCode[5][i];
			//------------------------------------------------------------------------------------------------------------------
			/* 存储FN阶的矩阵 */
			g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[0][i] = KeyBoardLayout_FN_Table[0][i];
			g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[1][i] = KeyBoardLayout_FN_Table[1][i];
			g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[2][i] = KeyBoardLayout_FN_Table[2][i];
			g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[3][i] = KeyBoardLayout_FN_Table[3][i];
			g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[4][i] = KeyBoardLayout_FN_Table[4][i];
			g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[5][i] = KeyBoardLayout_FN_Table[5][i];
			//------------------------------------------------------------------------------------------------------------------
			/* 这个矩阵用来对应做功能，比如有什么FN组合键变化了或者是驱动改了按键，同步更新这个矩阵 */
			g_variableData.Driver_KeyMatrix_Table[0][i] = KeyBoardLayout_Table[0][i];
			g_variableData.Driver_KeyMatrix_Table[1][i] = KeyBoardLayout_Table[1][i];
			g_variableData.Driver_KeyMatrix_Table[2][i] = KeyBoardLayout_Table[2][i];
			g_variableData.Driver_KeyMatrix_Table[3][i] = KeyBoardLayout_Table[3][i];
			g_variableData.Driver_KeyMatrix_Table[4][i] = KeyBoardLayout_Table[4][i];
			g_variableData.Driver_KeyMatrix_Table[5][i] = KeyBoardLayout_Table[5][i];
			//------------------------------------------------------------------------------------------------------------------
			g_DrivetData_TriggerPoint.ActualTriggerPoint_OnOff = 1;
			g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[0][i] = 1;
			g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[1][i] = 1;
			g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[2][i] = 1;
			g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[3][i] = 1;
			g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[4][i] = 1;
			g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[5][i] = 1;
			//------------------------------------------------------------------------------------------------------------------
			g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][i] = 0x01;
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][i] = 0x01;
			g_DrivetData_PBPoint.Segment_SET_Trigger_Break[1][i] = 0x01;
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][i] = 0x01;
			g_DrivetData_PBPoint.Segment_SET_Trigger_Break[2][i] = 0x01;
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][i] = 0x01;
			g_DrivetData_PBPoint.Segment_SET_Trigger_Break[3][i] = 0x01;
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][i] = 0x01;
			g_DrivetData_PBPoint.Segment_SET_Trigger_Break[4][i] = 0x01;
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][i] = 0x01;
			g_DrivetData_PBPoint.Segment_SET_Trigger_Break[5][i] = 0x01;
			g_DrivetData_PBPoint.Segment_SET_Trigger_Press[5][i] = 0x01;
			//------------------------------------------------------------------------------------------
			g_DrivetData_TriggerPoint.ActualTriggerPoint[0][i] = 50;
			g_DrivetData_TriggerPoint.ActualTriggerPoint[1][i] = 50;
			g_DrivetData_TriggerPoint.ActualTriggerPoint[2][i] = 50;
			g_DrivetData_TriggerPoint.ActualTriggerPoint[3][i] = 50;
			g_DrivetData_TriggerPoint.ActualTriggerPoint[4][i] = 50;
			g_DrivetData_TriggerPoint.ActualTriggerPoint[5][i] = 50;
			//------------------------------------------------------------------------------------------------------------------
			g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[0][i] = 0;
			g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[1][i] = 0;
			g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[2][i] = 0;
			g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[3][i] = 0;
			g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[4][i] = 0;
			g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[5][i] = 0;
			g_DrivetData_ContinuePoint.ContinueTriggerPoint_OnOff = 0;
			//------------------------------------------------------------------------------------------------------------------
			/* 做自定义的灯效颜色用 */
			g_DrivetData_LED.User_Custom[0][i] = 0;
			g_DrivetData_LED.User_Custom[1][i] = 0;
			g_DrivetData_LED.User_Custom[2][i] = 0;
			g_DrivetData_LED.User_Custom[3][i] = 0;
			g_DrivetData_LED.User_Custom[4][i] = 0;
			g_DrivetData_LED.User_Custom[5][i] = 0;
			//------------------------------------------------------------------------------------------------------------------
			/* 每一个按键对应一个RGB，一组最长可以放21个按键,RGB就是21*3 */
			g_DrivetData_LED.User_Define_RGB[0*63+i*3+0] = 0;
			g_DrivetData_LED.User_Define_RGB[0*63+i*3+1] = 0;
			g_DrivetData_LED.User_Define_RGB[0*63+i*3+2] = 0;

			g_DrivetData_LED.User_Define_RGB[1*63+i*3+0] = 0;
			g_DrivetData_LED.User_Define_RGB[1*63+i*3+1] = 0;
			g_DrivetData_LED.User_Define_RGB[1*63+i*3+2] = 0;

			g_DrivetData_LED.User_Define_RGB[2*63+i*3+0] = 0;
			g_DrivetData_LED.User_Define_RGB[2*63+i*3+1] = 0;
			g_DrivetData_LED.User_Define_RGB[2*63+i*3+2] = 0;

			g_DrivetData_LED.User_Define_RGB[3*63+i*3+0] = 0;
			g_DrivetData_LED.User_Define_RGB[3*63+i*3+1] = 0;
			g_DrivetData_LED.User_Define_RGB[3*63+i*3+2] = 0;

			g_DrivetData_LED.User_Define_RGB[4*63+i*3+0] = 0;
			g_DrivetData_LED.User_Define_RGB[4*63+i*3+1] = 0;
			g_DrivetData_LED.User_Define_RGB[4*63+i*3+2] = 0;
			//------------------------------------------------------------------------------------------------------------------
			//动态增益用，默认是1，根据raw data调节;
			g_variableData.Gain_Buffer[0][i] = Hall_sensor_Gain;
			g_variableData.Gain_Buffer[1][i] = Hall_sensor_Gain;
			g_variableData.Gain_Buffer[2][i] = Hall_sensor_Gain;
			g_variableData.Gain_Buffer[3][i] = Hall_sensor_Gain;
			g_variableData.Gain_Buffer[4][i] = Hall_sensor_Gain;
			g_variableData.Gain_Buffer[5][i] = Hall_sensor_Gain;
			//------------------------------------------------------------------------------------------------------------------
		}
		//----------------------------------------------------------------------------------------------------------------------
		Flash_IntervalFlash_Write((uint8_t *)&g_variableData, Flag_MCUFlash_PageIndex, PAGE_LEN);
		//----------------------------------------------------------------------------------------------------------------------
		//Profile0_Save_Read(1);
		//Profile1_Save_Read(1);
		//Profile2_Save_Read(1);
		//----------------------------------------------------------------------------------------------------------------------
#if 0
		switch(g_variableData.Profile_Index)
		{
			case 0:
				Profile0_Save_Read(1);
				break;
			case 1:
				Profile1_Save_Read(1);
				break;
			case 2:
				Profile2_Save_Read(1);
				break;
			default:
				break;
		}
#endif
		//----------------------------------------------------------------------------------------------------------------------
		/* 测试一下能不能读到正常的数据 */
		//------------------------------------------------------------------------------------------
		#if UART_DEBUG
			#if 0
				uint8_t testBuffer[256];
				for(uint16_t j = 0; j < 256; j++)
				{
					testBuffer[j] = 0;
				}
				//------------------------------------------------------------------------------------------
				FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector16_LED1_Addr, (uint8_t *) &g_DrivetData_LED, 256);
				//------------------------------------------------------------------------------------------
				FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector16_LED2_Addr, (uint8_t *) &g_DrivetData_LED.User_Define_RGB[151], 256);
					//------------------------------------------------------------------------------------------
				for(uint16_t k = 0; k < 315; k++)
				{
					usb_echo("RGB:0x%x\n",g_DrivetData_LED.User_Define_RGB[k]);
				}
			#endif
		#endif
	    //----------------------------------------------------------------------------------------------------------------------
	}
	else if(g_variableData.varInitFlag == 0xAA)
	{
		//---------------------------------------------------------------------------------------------------------------------------------------------
		/* 第一次上电初始化驱动 */
		DriverAPP_WASD_Switch = g_variableData.DriverAPP_WASD_Switch;
		DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
		DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinMacMode;
		DriverAPP_BottomTriggerState = g_variableData.DriverAPP_BottomTriggerState;
		DriverAPP_Nkey_Rollover_Switch = g_variableData.DriverAPP_Nkey_Rollover_Switch;
		//----------------------------------------------------------------------------------------------------------------------
		switch(g_variableData.Profile_Index)
		{
			case 0:
				Profile0_Save_Read(0);
				break;
			case 1:
				Profile1_Save_Read(0);
				break;
			case 2:
				Profile2_Save_Read(0);
				break;
			default:
				break;
		}
		//----------------------------------------------------------------------------------------------------------------------
	}
	//------------------------------------------------------------------------------------------
	//usb_echo("LED MDOE:%d\r\n",g_variableData.LedMode);
	/* RGB Variable init */
	LED_Variable_Init();
	//------------------------------------------------------------------------------------------
	/* Hall Sensor Variable init */
	Hall_Cali_Init();
	//------------------------------------------------------------------------------------------
	Keyboard_Variable_Init();
	//------------------------------------------------------------------------------------------
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: Variable_Reset
 * Description  	: 参数恢复默认
 *
 * END ****************************************************************************************************************/
void Variable_Reset(void)
{
	//------------------------------------------------------------------------------------------
	Flash_IntervalFlash_Read((uint8_t *)&g_variableData, Flag_MCUFlash_PageIndex, PAGE_LEN);
	//------------------------------------------------------------------------------------------
	g_variableData.varInitFlag = 0xAA;
	g_variableData.PageIndex = Flag_MCUFlash_PageIndex;
	//------------------------------------------------------------------------------------------
	/* Led第一次上电 */
	g_variableData.LED_ONOFF = 1;
	g_variableData.LED_MODE = LED_2_mode;
	g_variableData.LED_BRIGHTNESS = 9;
	g_variableData.LED_SPEED = 2;
	g_variableData.LED_COLOR[0] = 0xFF;
	g_variableData.LED_COLOR[1] = 0xFF;
	g_variableData.LED_COLOR[2] = 0xFF;
	g_variableData.LED_RGB_MODE = 1;
	g_variableData.LIGHT_BEACON = 0;
	g_variableData.LedColor_Index = 0;
	//------------------------------------------------------------------------------------------
	/* 第一次上电初始化驱动 */
	g_variableData.DriverAPP_WASD_Switch = 0;
	DriverAPP_WASD_Switch = g_variableData.DriverAPP_WASD_Switch;
	g_variableData.DriverAPP_WinMacMode = 0;
	DriverAPP_WinMacMode = g_variableData.DriverAPP_WinMacMode;
	g_variableData.DriverAPP_WinLock_Switch = 0;
	DriverAPP_WinLock_Switch = g_variableData.DriverAPP_WinLock_Switch;
	g_variableData.DriverAPP_BottomTriggerState = 0;
	DriverAPP_BottomTriggerState = g_variableData.DriverAPP_BottomTriggerState;
	g_variableData.DriverAPP_Nkey_Rollover_Switch = 0;
	DriverAPP_Nkey_Rollover_Switch = g_variableData.DriverAPP_Nkey_Rollover_Switch;
	//----------------------------------------------------------------------------------------------------------------------
	for(uint8_t i = 0; i < TRANSFER_SIZE; i++)
	{
		/* 存储原始码值的矩阵 */
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[0][i] = KeyBoardLayout_NormalCode[0][i];
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[1][i] = KeyBoardLayout_NormalCode[1][i];
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[2][i] = KeyBoardLayout_NormalCode[2][i];
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[3][i] = KeyBoardLayout_NormalCode[3][i];
		g_DrivetData_KeyMatrix.KeyMatrix_Buffer[4][i] = KeyBoardLayout_NormalCode[4][i];
		//------------------------------------------------------------------------------------------------------------------
		/* 存储FN阶的矩阵 */
		g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[0][i] = KeyBoardLayout_FN_Table[0][i];
		g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[1][i] = KeyBoardLayout_FN_Table[1][i];
		g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[2][i] = KeyBoardLayout_FN_Table[2][i];
		g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[3][i] = KeyBoardLayout_FN_Table[3][i];
		g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[4][i] = KeyBoardLayout_FN_Table[4][i];
		//------------------------------------------------------------------------------------------------------------------
		/* 这个矩阵用来对应做功能，比如有什么FN组合键变化了或者是驱动改了按键，同步更新这个矩阵 */
		g_variableData.Driver_KeyMatrix_Table[0][i] = KeyBoardLayout_Table[0][i];
		g_variableData.Driver_KeyMatrix_Table[1][i] = KeyBoardLayout_Table[1][i];
		g_variableData.Driver_KeyMatrix_Table[2][i] = KeyBoardLayout_Table[2][i];
		g_variableData.Driver_KeyMatrix_Table[3][i] = KeyBoardLayout_Table[3][i];
		g_variableData.Driver_KeyMatrix_Table[4][i] = KeyBoardLayout_Table[4][i];
		//------------------------------------------------------------------------------------------------------------------
		g_DrivetData_TriggerPoint.ActualTriggerPoint_OnOff = 1;
		g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[0][i] = 1;
		g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[1][i] = 1;
		g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[2][i] = 1;
		g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[3][i] = 1;
		g_DrivetData_TriggerPoint.ActualTriggerPoint_Mode[4][i] = 1;
		//------------------------------------------------------------------------------------------------------------------
		g_DrivetData_PBPoint.Segment_SET_Trigger_Break[0][i] = 0x01;
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[0][i] = 0x01;
		g_DrivetData_PBPoint.Segment_SET_Trigger_Break[1][i] = 0x01;
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[1][i] = 0x01;
		g_DrivetData_PBPoint.Segment_SET_Trigger_Break[2][i] = 0x01;
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[2][i] = 0x01;
		g_DrivetData_PBPoint.Segment_SET_Trigger_Break[3][i] = 0x01;
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[3][i] = 0x01;
		g_DrivetData_PBPoint.Segment_SET_Trigger_Break[4][i] = 0x01;
		g_DrivetData_PBPoint.Segment_SET_Trigger_Press[4][i] = 0x01;
		//------------------------------------------------------------------------------------------
		g_DrivetData_TriggerPoint.ActualTriggerPoint[0][i] = 0x01;
		g_DrivetData_TriggerPoint.ActualTriggerPoint[1][i] = 0x01;
		g_DrivetData_TriggerPoint.ActualTriggerPoint[2][i] = 0x01;
		g_DrivetData_TriggerPoint.ActualTriggerPoint[3][i] = 0x01;
		g_DrivetData_TriggerPoint.ActualTriggerPoint[4][i] = 0x01;
		//------------------------------------------------------------------------------------------------------------------
		g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[0][i] = 0;
		g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[1][i] = 0;
		g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[2][i] = 0;
		g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[3][i] = 0;
		g_DrivetData_ContinuePoint.ContinueTriggerPoint_Mode[4][i] = 0;
		g_DrivetData_ContinuePoint.ContinueTriggerPoint_OnOff = 0;
		//------------------------------------------------------------------------------------------------------------------
		/* 做自定义的灯效颜色用 */
		g_DrivetData_LED.User_Custom[0][i] = 0;
		g_DrivetData_LED.User_Custom[1][i] = 0;
		g_DrivetData_LED.User_Custom[2][i] = 0;
		g_DrivetData_LED.User_Custom[3][i] = 0;
		g_DrivetData_LED.User_Custom[4][i] = 0;
		//------------------------------------------------------------------------------------------------------------------
		/* 每一个按键对应一个RGB，一组最长可以放21个按键,RGB就是21*3 */
		g_DrivetData_LED.User_Define_RGB[0*63+i*3+0] = 0;
		g_DrivetData_LED.User_Define_RGB[0*63+i*3+1] = 0;
		g_DrivetData_LED.User_Define_RGB[0*63+i*3+2] = 0;

		g_DrivetData_LED.User_Define_RGB[1*63+i*3+0] = 0;
		g_DrivetData_LED.User_Define_RGB[1*63+i*3+1] = 0;
		g_DrivetData_LED.User_Define_RGB[1*63+i*3+2] = 0;

		g_DrivetData_LED.User_Define_RGB[2*63+i*3+0] = 0;
		g_DrivetData_LED.User_Define_RGB[2*63+i*3+1] = 0;
		g_DrivetData_LED.User_Define_RGB[2*63+i*3+2] = 0;

		g_DrivetData_LED.User_Define_RGB[3*63+i*3+0] = 0;
		g_DrivetData_LED.User_Define_RGB[3*63+i*3+1] = 0;
		g_DrivetData_LED.User_Define_RGB[3*63+i*3+2] = 0;

		g_DrivetData_LED.User_Define_RGB[4*63+i*3+0] = 0;
		g_DrivetData_LED.User_Define_RGB[4*63+i*3+1] = 0;
		g_DrivetData_LED.User_Define_RGB[4*63+i*3+2] = 0;
		//------------------------------------------------------------------------------------------------------------------
		//动态增益用，默认是1，根据raw data调节;
		g_variableData.Gain_Buffer[0][i] = Hall_sensor_Gain;
		g_variableData.Gain_Buffer[1][i] = Hall_sensor_Gain;
		g_variableData.Gain_Buffer[2][i] = Hall_sensor_Gain;
		g_variableData.Gain_Buffer[3][i] = Hall_sensor_Gain;
		g_variableData.Gain_Buffer[4][i] = Hall_sensor_Gain;
		//------------------------------------------------------------------------------------------------------------------
	}
	//----------------------------------------------------------------------------------------------------------------------
	Flash_IntervalFlash_Write((uint8_t *)&g_variableData, Flag_MCUFlash_PageIndex, PAGE_LEN);
	//----------------------------------------------------------------------------------------------------------------------
	Profile0_Save_Read(1);
	Profile1_Save_Read(1);
	Profile2_Save_Read(1);
	//----------------------------------------------------------------------------------------------------------------------
#if 0
	switch(g_variableData.Profile_Index)
	{
		case 0:
			Profile0_Save_Read(1);
			break;
		case 1:
			Profile1_Save_Read(1);
			break;
		case 2:
			Profile2_Save_Read(1);
			break;
		default:
			break;
	}
#endif
	//----------------------------------------------------------------------------------------------------------------------
	/* 测试一下能不能读到正常的数据 */
   //------------------------------------------------------------------------------------------
	#if UART_DEBUG
		#if 0
			uint8_t testBuffer[256];
			for(uint16_t j = 0; j < 256; j++)
			{
				testBuffer[j] = 0;
			}
			//------------------------------------------------------------------------------------------
			FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector16_LED1_Addr, (uint8_t *) &g_DrivetData_LED, 256);
			//------------------------------------------------------------------------------------------
			FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector16_LED2_Addr, (uint8_t *) &g_DrivetData_LED.User_Define_RGB[151], 256);
				//------------------------------------------------------------------------------------------
			for(uint16_t k = 0; k < 315; k++)
			{
				usb_echo("RGB:0x%x\n",g_DrivetData_LED.User_Define_RGB[k]);
			}
		#endif
	#endif
	//----------------------------------------------------------------------------------------------------------------------
	//usb_echo("LED MDOE:%d\r\n",g_variableData.LedMode);
	/* RGB Variable init */
	LED_Variable_Init();
	//----------------------------------------------------------------------------------------------------------------------
	/* Hall Sensor Variable init */
	Hall_Cali_Init();
	//----------------------------------------------------------------------------------------------------------------------
	Keyboard_Variable_Init();
	//----------------------------------------------------------------------------------------------------------------------
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name	: VariableUpdate_MCUFlash_Process
 * Description  	: save mcu Flash data
 *
 * END ****************************************************************************************************************/
void VariableUpdate_MCUFlash_Process(void)
{
	if(Flag_NeedUpdate_FlashZB25V)
	{
		/* wait 10ms and save Flash */
		Tick_UpdateMCUFlash++;
		if(Tick_UpdateMCUFlash > 80)
		{
			Tick_UpdateMCUFlash = 0;
			/* 保存flash */
			FlashZB25V_SaveAPPData();
		}
	}
}

void Profile0_Save_Read(uint8_t Flag_Write_Read)
{
	if(Flag_Write_Read)
	{
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector16_TriggerPoint,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector16_ActualPoint_Addr, (uint8_t *)&g_DrivetData_TriggerPoint, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector17_PBPoint,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector17_PBPoint_Addr, (uint8_t *)&g_DrivetData_PBPoint, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector18_ContinueTrigerPoint,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector18_ContinuePoint_Addr, (uint8_t *)&g_DrivetData_ContinuePoint, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector19_TGL,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector19_TGL_Addr, (uint8_t *)&g_DrivetData_TGL, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector20_MT,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector20_MT_Addr, (uint8_t *)&g_DrivetData_MT, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector21_DKS,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector21_DKS1_Addr, (uint8_t *)&g_DrivetData_DKS, 256);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector21_DKS2_Addr, (uint8_t *)&g_DrivetData_DKS.DKS_Buffer[256], 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector22_LED,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector22_LED1_Addr, (uint8_t *)&g_DrivetData_LED, 256);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector22_LED2_Addr, (uint8_t *)&g_DrivetData_LED.User_Define_RGB[151], 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector24_KeyMatrix,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector24_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix, 256);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector24_FN_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[0][0], 256);
		//----------------------------------------------------------------------------------------------------------------------
	}
	else
	{
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector16_ActualPoint_Addr, (uint8_t *)&g_DrivetData_TriggerPoint, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector17_PBPoint_Addr, (uint8_t *)&g_DrivetData_PBPoint, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector18_ContinuePoint_Addr, (uint8_t *)&g_DrivetData_ContinuePoint, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector19_TGL_Addr, (uint8_t *)&g_DrivetData_TGL, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector20_MT_Addr, (uint8_t *)&g_DrivetData_MT, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector21_DKS1_Addr, (uint8_t *)&g_DrivetData_DKS, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector21_DKS2_Addr, (uint8_t *)&g_DrivetData_DKS.DKS_Buffer[256], 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector22_LED1_Addr, (uint8_t *)&g_DrivetData_LED, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector22_LED2_Addr, (uint8_t *)&g_DrivetData_LED.User_Define_RGB[151], 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector24_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.KeyMatrix_Buffer, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector24_FN_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[0][0], 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
	}
}

void Profile1_Save_Read(uint8_t Flag_Write_Read)
{
	if(Flag_Write_Read)
	{
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector25_TriggerPoint,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector25_ActualPoint_Addr, (uint8_t *)&g_DrivetData_TriggerPoint, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector26_PBPoint,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector26_PBPoint_Addr, (uint8_t *)&g_DrivetData_PBPoint, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector27_ContinueTrigerPoint,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector27_ContinuePoint_Addr, (uint8_t *)&g_DrivetData_ContinuePoint, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector28_TGL,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector28_TGL_Addr, (uint8_t *)&g_DrivetData_TGL, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector29_MT,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector29_MT_Addr, (uint8_t *)&g_DrivetData_MT, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector30_DKS,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector30_DKS1_Addr, (uint8_t *)&g_DrivetData_DKS, 256);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector30_DKS2_Addr, (uint8_t *)&g_DrivetData_DKS.DKS_Buffer[256], 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector31_LED,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector31_LED1_Addr, (uint8_t *)&g_DrivetData_LED, 256);
		FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector31_LED2_Addr, (uint8_t *)&g_DrivetData_LED.User_Define_RGB[151], 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector33_KeyMatrix,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector33_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix, 256);
		FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector33_FN_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[0][0], 256);
		//----------------------------------------------------------------------------------------------------------------------
	}
	else
	{
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector25_ActualPoint_Addr, (uint8_t *)&g_DrivetData_TriggerPoint, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector26_PBPoint_Addr, (uint8_t *)&g_DrivetData_PBPoint, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector27_ContinuePoint_Addr, (uint8_t *)&g_DrivetData_ContinuePoint, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector28_TGL_Addr, (uint8_t *)&g_DrivetData_TGL, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector29_MT_Addr, (uint8_t *)&g_DrivetData_MT, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector30_DKS1_Addr, (uint8_t *)&g_DrivetData_DKS, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector30_DKS2_Addr, (uint8_t *)&g_DrivetData_DKS.DKS_Buffer[256], 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector31_LED1_Addr, (uint8_t *)&g_DrivetData_LED, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector31_LED2_Addr, (uint8_t *)&g_DrivetData_LED.User_Define_RGB[151], 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block02_Sector33_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.KeyMatrix_Buffer, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block02_Sector33_FN_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[0][0], 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
	}
}

void Profile2_Save_Read(uint8_t Flag_Write_Read)
{
	if(Flag_Write_Read)
	{
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector34_TriggerPoint,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector34_ActualPoint_Addr, (uint8_t *)&g_DrivetData_TriggerPoint, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector35_PBPoint,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector35_PBPoint_Addr, (uint8_t *)&g_DrivetData_PBPoint, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector36_ContinueTrigerPoint,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector36_ContinuePoint_Addr, (uint8_t *)&g_DrivetData_ContinuePoint, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector37_TGL,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector37_TGL_Addr, (uint8_t *)&g_DrivetData_TGL, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector38_MT,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector38_MT_Addr, (uint8_t *)&g_DrivetData_MT, 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector39_DKS,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector39_DKS1_Addr, (uint8_t *)&g_DrivetData_DKS, 256);
		FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector39_DKS2_Addr, (uint8_t *)&g_DrivetData_DKS.DKS_Buffer[256], 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector40_LED,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector40_LED1_Addr, (uint8_t *)&g_DrivetData_LED, 256);
		FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector40_LED2_Addr, (uint8_t *)&g_DrivetData_LED.User_Define_RGB[151], 256);
		//----------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector42_KeyMatrix,0);
		FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector42_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix, 256);
		FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector42_FN_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[0][0], 256);
		//----------------------------------------------------------------------------------------------------------------------
	}
	else
	{
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block02_Sector34_ActualPoint_Addr, (uint8_t *)&g_DrivetData_TriggerPoint, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block02_Sector35_PBPoint_Addr, (uint8_t *)&g_DrivetData_PBPoint, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block02_Sector36_ContinuePoint_Addr, (uint8_t *)&g_DrivetData_ContinuePoint, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block02_Sector37_TGL_Addr, (uint8_t *)&g_DrivetData_TGL, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block02_Sector38_MT_Addr, (uint8_t *)&g_DrivetData_MT, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block02_Sector39_DKS1_Addr, (uint8_t *)&g_DrivetData_DKS, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block02_Sector39_DKS2_Addr, (uint8_t *)&g_DrivetData_DKS.DKS_Buffer[256], 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block02_Sector40_LED1_Addr, (uint8_t *)&g_DrivetData_LED, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block02_Sector40_LED2_Addr, (uint8_t *)&g_DrivetData_LED.User_Define_RGB[151], 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block02_Sector42_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.KeyMatrix_Buffer, 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
		FlashZB25V_Read_Data(FLASHZB25V_Block02_Sector42_FN_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[0][0], 256);
		//---------------------------------------------------------------------------------------------------------------------------------------------
	}
}





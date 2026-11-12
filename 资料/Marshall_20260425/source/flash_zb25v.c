/*
 * flash_ic.c
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <flash_zb25v.h>
#include <fsl_debug_console.h>
#include <usb_device_config.h>
#include <usb.h>
#include <usb_device.h>
#include <usb_device_class.h>
#include <usb_device_hid.h>
#include <usb_device_ch9.h>
#include <usb_device_descriptor.h>
#include <composite.h>
#include "dma_config.h"
#include "pin_mux.h"
#include "usb_misc.h"
#include <Flash_iap.h>
#include <hid_keyboard.h>
#include <driverapp_communication.h>
#include <led.h>
#include <macro.h>
/*******************************************************************************
 * define
 ******************************************************************************/
#define DELAY_X_NS {__ASM("NOP");__ASM("NOP");__ASM("NOP");__ASM("NOP");__ASM("NOP");\
					__ASM("NOP");__ASM("NOP");__ASM("NOP");__ASM("NOP");__ASM("NOP");\
					__ASM("NOP");__ASM("NOP");__ASM("NOP");__ASM("NOP");__ASM("NOP");}
/*******************************************************************************
 * Variable
 ******************************************************************************/
bool Flag_Read_FlashZB25V_ID_Success = false;
uint8_t 	Flag_FlashZB25V_PageIndex = 0;
uint32_t	Flag_NeedUpdate_FlashZB25V = 0;
uint8_t 	Tick_UpdateFlashZB25V = 0;
/*******************************************************************************
 * FUNCTION
 ******************************************************************************/
//****************************************************************************
//
// Function name: SpiWriteByte(uint8_t data).
// @brief:
//
//*****************************************************************************
static void SpiWriteByte(uint8_t data)
{
    // Read 8 bits MSB...LSB
    for (uint8_t i = 8; i != 0; i--)
    {
    	FlashCLK_PIN_SET_LOW();
    	DELAY_X_NS;
        if (data & 0x80)
        {
        	FlashMOSI_PIN_SET_HIGH();
        }
        else
        {
        	FlashMOSI_PIN_SET_LOW();
        }

        data <<= 1;
        DELAY_X_NS;
        FlashCLK_PIN_SET_HIGH();
        DELAY_X_NS;
    }
}
//****************************************************************************
//
// Function name: SpiWriteByte(uint8_t data).
// @brief:
//
//*****************************************************************************
static uint8_t SpiReadByte(void)
{
    uint8_t ret = 0;

    // Read 8 bits MSB...LSB
    for (uint8_t i = 8; i != 0; i--)
    {
        ret <<= 1;  //Shift read data

        FlashCLK_PIN_SET_LOW();
        DELAY_X_NS;
        FlashCLK_PIN_SET_HIGH();
        if( FlashMISO_PIN_GET_STATE() )
        {
            ret |= 0x01;
        }
        DELAY_X_NS;
    }
    return ret;
}
//****************************************************************************
//
// Function name: Flash_ReadID.
// @brief:	Read flash ic manufacture ID and device ID.
//
//*****************************************************************************
uint16_t FlashZB25V_ReadID(void)
{
	uint16_t ret = 0;

    FlashCS_PIN_SET_LOW();
    DELAY_X_NS;
    //------------------------------------------------------------------------------------------
    SpiWriteByte(FLASHZB25V_ManufactureDeviceID_Reg);
    //------------------------------------------------------------------------------------------
    //Address is 0 in datsheet.
    SpiWriteByte(0);
    SpiWriteByte(0);
    SpiWriteByte(0);
    //------------------------------------------------------------------------------------------
    /* The first byte is Manufacture ID:0x5E */
    ret = (SpiReadByte() << 8);
    /* The Secon byte is device ID:0x13 */
    ret |= SpiReadByte();
    //------------------------------------------------------------------------------------------
    DELAY_X_NS;
    FlashCS_PIN_SET_HIGH();
    DELAY_X_NS;

    return ret;
}

//****************************************************************************
//
// Function name: Flash_Read(uint8_t data).
// @brief:
//
//*****************************************************************************
bool FlashZB25V_PowerON_check_ID(void)
{
	uint16_t Count = 0;
	//------------------------------------------------------------------------------------------
    FlashCS_PIN_SET_LOW();
    hall_mcu_delay_ms(10);
    FlashCS_PIN_SET_HIGH();
    //------------------------------------------------------------------------------------------
    usb_echo("ID_flash:0x%x\n",FlashZB25V_ReadID);

	while(FLASHZB25V_ManufactureDeviceID != FlashZB25V_ReadID())
	{
		Count++;
		if(Count >100)
		{
			return false;
		}
	}
	return true;
}
//****************************************************************************
//
// Function name: FlashZB25V_Read_OneData.
// @brief:
//
//*****************************************************************************
uint16_t FlashZB25V_Read_Data(uint32_t addr, uint8_t* pBuffer, uint16_t ReadLen)
{
	uint16_t i = 0;

    FlashCS_PIN_SET_LOW();
    DELAY_X_NS;
    //------------------------------------------------------------------------------------------
    SpiWriteByte(FLASHZB25V_ReadData);
    SpiWriteByte(addr>>16);
    SpiWriteByte(addr>>8);
    SpiWriteByte(addr);
    //------------------------------------------------------------------------------------------
    for(i = 0; i < ReadLen; i++)
    {
    	pBuffer[i] = SpiReadByte();
    }
    //------------------------------------------------------------------------------------------
    DELAY_X_NS;
    FlashCS_PIN_SET_HIGH();
}
//****************************************************************************
//
// Function name: FlashZB25V_Read_OneData.
// @brief:
//
//*****************************************************************************
uint16_t FlashZB25V_Sector_Erase(uint8_t FlashZB25V_BlockIndex,uint8_t FlashZB25V_PageIndex)
{
	//------------------------------------------------------------------------------------------
	FlashZB25V_CheckBusy();
    //------------------------------------------------------------------------------------------
    FlashZB25V_Write_ENABLE();
    //------------------------------------------------------------------------------------------
    /* 擦除完需要四十多ms */
    FlashZB25V_SectorErase(FlashZB25V_BlockIndex,FlashZB25V_PageIndex);
    //------------------------------------------------------------------------------------------
    FlashZB25V_CheckBusy();
    //------------------------------------------------------------------------------------------
}
//****************************************************************************
//
// Function name: FlashZB25V_Write_Data.
// @brief:
//
//*****************************************************************************
void FlashZB25V_Write_Data(uint32_t addr, uint8_t* pBuffer, uint16_t WriteLen)
{
#if 0
	//------------------------------------------------------------------------------------------
	FlashZB25V_CheckBusy();
    //------------------------------------------------------------------------------------------
    FlashZB25V_Write_ENABLE();
    //------------------------------------------------------------------------------------------
    /* 擦除完需要四十多ms */
    FlashZB25V_SectorErase(Flag_FlashZB25V_PageIndex);
    //------------------------------------------------------------------------------------------
    FlashZB25V_CheckBusy();
    //------------------------------------------------------------------------------------------
#endif
    FlashZB25V_Write_ENABLE();
    //------------------------------------------------------------------------------------------
    FlashCS_PIN_SET_LOW();
    DELAY_X_NS;
    //------------------------------------------------------------------------------------------
    SpiWriteByte(FLASHZB25V_Program);
    SpiWriteByte(addr>>16);
    SpiWriteByte(addr>>8);
    SpiWriteByte(addr);
    //------------------------------------------------------------------------------------------
    for(uint16_t i = 0; i < WriteLen; i++)
    {
    	SpiWriteByte(pBuffer[i]);
    }
    //------------------------------------------------------------------------------------------
    DELAY_X_NS;
    FlashCS_PIN_SET_HIGH();
    //------------------------------------------------------------------------------------------
    FlashZB25V_CheckBusy();
    //------------------------------------------------------------------------------------------
    FlashZB25V_Write_Disable();
    //------------------------------------------------------------------------------------------
}
//****************************************************************************
//
// Function name: Flash_Write_ENABLE.
// @brief:
//
//*****************************************************************************
void FlashZB25V_Write_ENABLE(void)
{
    FlashCS_PIN_SET_LOW();
    DELAY_X_NS;
    //------------------------------------------------------------------------------------------
    SpiWriteByte(FLASHZB25V_WriteEnable);
    //------------------------------------------------------------------------------------------
    DELAY_X_NS;
    FlashCS_PIN_SET_HIGH();
}
//****************************************************************************
//
// Function name: Flash_Write_ENABLE.
// @brief:
//
//*****************************************************************************
void FlashZB25V_Write_Disable(void)
{
    FlashCS_PIN_SET_LOW();
    DELAY_X_NS;
    //------------------------------------------------------------------------------------------
    SpiWriteByte(FLASHZB25V_WriteDisable);
    //------------------------------------------------------------------------------------------
    DELAY_X_NS;
    FlashCS_PIN_SET_HIGH();
}
//****************************************************************************
//
// Function name: FlashZB25V_SectorErase.
// @brief:
//
//*****************************************************************************
void FlashZB25V_SectorErase(uint8_t FlashZB25V_BlockIndex,uint8_t FlashZB25V_PageIndex)
{
	uint32_t destAdrss = 0;
    //------------------------------------------------------------------------------------------
	#if UART_ENABEL
	usb_echo("SectorErase\n");
	#endif
	//------------------------------------------------------------------------------------------
    FlashCS_PIN_SET_LOW();
    DELAY_X_NS;
    //------------------------------------------------------------------------------------------
    /* 用flash ic的block1来存数据，一个block有16个Page，因为是从block1开始，所以index这些要减去block0的16个.比如这里block1的page1对应的index是0. */
    if(Flag_FlashZB25V_PageIndex > 239)
    {
    	Flag_FlashZB25V_PageIndex = 0;
    }
    //destAdrss = FLASHZB25V_Block01_Sector16_ActualPoint_Addr*FlashZB25V_BlockIndex + FlashZB25V_PageIndex*FLASHZB25V_PageSize;
    destAdrss = FLASHZB25V_Block01_Sector16_ActualPoint_Addr + FlashZB25V_BlockIndex*FLASHZB25V_PageSize;
    //------------------------------------------------------------------------------------------
    SpiWriteByte(FLASHZB25V_Erase);
    SpiWriteByte(destAdrss>>16);
    SpiWriteByte(destAdrss>>8);
    SpiWriteByte(destAdrss);
    DELAY_X_NS;
    FlashCS_PIN_SET_HIGH();
}
//****************************************************************************
//
// Function name: FlashZB25V_SR1_Read.
// @brief:
//
//*****************************************************************************
uint8_t FlashZB25V_SR1_Read(void)
{
	uint8_t status = 0;

    FlashCS_PIN_SET_LOW();
    DELAY_X_NS;
    //------------------------------------------------------------------------------------------
    SpiWriteByte(FLASHZB25V_SR1_Reg);
    //------------------------------------------------------------------------------------------
    status = SpiReadByte();
    //------------------------------------------------------------------------------------------
    DELAY_X_NS;
    FlashCS_PIN_SET_HIGH();
    return status;
}
//****************************************************************************
//
// Function name: FlashZB25V_SR1_Read.
// @brief:
//
//*****************************************************************************
void FlashZB25V_CheckBusy(void)
{
	uint8_t StatusRegister = FlashZB25V_SR1_Read();
	//------------------------------------------------------------------------------------------
	while((FlashZB25V_SR1_Read() & 0x01) == 0x01)
	{

	}
}
//****************************************************************************
//
// Function name: FlashZB25V_SR1_Read.
// @brief:
//
//*****************************************************************************
void FlashZB25V_Init(void)
{
	Flag_Read_FlashZB25V_ID_Success = FlashZB25V_PowerON_check_ID();
	//------------------------------------------------------------------------------------------
	#if UART_DEBUG
		#if 1
			usb_echo("ID_Success:0x%x\n",Flag_Read_FlashZB25V_ID_Success);
		#endif
	#endif
	//------------------------------------------------------------------------------------------
}
//****************************************************************************
//
// Function name: FlashZB25V_SaveAPPData.
// @brief:保存驱动的数据，保存一次的话是全部的数据都保存
//
//*****************************************************************************
void FlashZB25V_SaveAPPData(void)
{
	uint32_t Macro_Address;
	/* 整个保存需要114ms左右的时间 */
	/**********************************************************************************************************************/
	//----------------------------------------------------------------------------------------------------------------------
	if(Flag_NeedUpdate_FlashZB25V & FLASH5528_Data)
	{
		Flag_NeedUpdate_FlashZB25V &= (~FLASH5528_Data);
		//------------------------------------------------------------------------------------------------------------------
		Flash_IntervalFlash_Write((uint8_t *)&g_variableData, Flag_MCUFlash_PageIndex, PAGE_LEN);
		//------------------------------------------------------------------------------------------------------------------
	}
	//----------------------------------------------------------------------------------------------------------------------
	if(Flag_NeedUpdate_FlashZB25V & FLASHZB25V_TrigerPoint)
	{
		Flag_NeedUpdate_FlashZB25V &= (~FLASHZB25V_TrigerPoint);
		//----------------------------------------------------------------------------------------------------------------------
		switch(g_variableData.Profile_Index)
		{
			case 0:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector16_TriggerPoint,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector16_ActualPoint_Addr, (uint8_t *)&g_DrivetData_TriggerPoint, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			case 1:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector25_TriggerPoint,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector25_ActualPoint_Addr, (uint8_t *)&g_DrivetData_TriggerPoint, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			case 2:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector34_TriggerPoint,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector34_ActualPoint_Addr, (uint8_t *)&g_DrivetData_TriggerPoint, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			default:
				break;
		}
		//----------------------------------------------------------------------------------------------------------------------
	}
	//----------------------------------------------------------------------------------------------------------------------
	if(Flag_NeedUpdate_FlashZB25V & FLASHZB25V_PBPoint)
	{
		Flag_NeedUpdate_FlashZB25V &= (~FLASHZB25V_PBPoint);
		//----------------------------------------------------------------------------------------------------------------------
		switch(g_variableData.Profile_Index)
		{
			case 0:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector17_PBPoint,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector17_PBPoint_Addr, (uint8_t *)&g_DrivetData_PBPoint, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			case 1:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector26_PBPoint,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector26_PBPoint_Addr, (uint8_t *)&g_DrivetData_PBPoint, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			case 2:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector35_PBPoint,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector35_PBPoint_Addr, (uint8_t *)&g_DrivetData_PBPoint, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			default:
				break;
		}
		//----------------------------------------------------------------------------------------------------------------------
	}
	//----------------------------------------------------------------------------------------------------------------------
	if(Flag_NeedUpdate_FlashZB25V & FLASHZB25V_ContinueTrigerPoint)
	{
		Flag_NeedUpdate_FlashZB25V &= (~FLASHZB25V_ContinueTrigerPoint);
		//----------------------------------------------------------------------------------------------------------------------
		switch(g_variableData.Profile_Index)
		{
			case 0:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector18_ContinueTrigerPoint,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector18_ContinuePoint_Addr, (uint8_t *)&g_DrivetData_ContinuePoint, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			case 1:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector27_ContinueTrigerPoint,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector27_ContinuePoint_Addr, (uint8_t *)&g_DrivetData_ContinuePoint, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			case 2:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector36_ContinueTrigerPoint,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector36_ContinuePoint_Addr, (uint8_t *)&g_DrivetData_ContinuePoint, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			default:
				break;
		}
		//----------------------------------------------------------------------------------------------------------------------
	}
	//----------------------------------------------------------------------------------------------------------------------
	if(Flag_NeedUpdate_FlashZB25V & FLASHZB25V_TGL)
	{
		Flag_NeedUpdate_FlashZB25V &= (~FLASHZB25V_TGL);
		//----------------------------------------------------------------------------------------------------------------------
		switch(g_variableData.Profile_Index)
		{
			case 0:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector19_TGL,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector19_TGL_Addr, (uint8_t *)&g_DrivetData_TGL, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			case 1:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector28_TGL,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector28_TGL_Addr, (uint8_t *)&g_DrivetData_TGL, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			case 2:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector37_TGL,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector37_TGL_Addr, (uint8_t *)&g_DrivetData_TGL, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			default:
				break;
		}
		//----------------------------------------------------------------------------------------------------------------------
	}
	//----------------------------------------------------------------------------------------------------------------------
	if(Flag_NeedUpdate_FlashZB25V & FLASHZB25V_MT)
	{
		Flag_NeedUpdate_FlashZB25V &= (~FLASHZB25V_MT);
		//----------------------------------------------------------------------------------------------------------------------
		switch(g_variableData.Profile_Index)
		{
			case 0:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector20_MT,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector20_MT_Addr, (uint8_t *)&g_DrivetData_MT, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			case 1:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector29_MT,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector29_MT_Addr, (uint8_t *)&g_DrivetData_MT, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			case 2:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector38_MT,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector38_MT_Addr, (uint8_t *)&g_DrivetData_MT, 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			default:
				break;
		}
		//----------------------------------------------------------------------------------------------------------------------
	}
	//----------------------------------------------------------------------------------------------------------------------
	if(Flag_NeedUpdate_FlashZB25V & FLASHZB25V_DKS)
	{
		Flag_NeedUpdate_FlashZB25V &= (~FLASHZB25V_DKS);
		//----------------------------------------------------------------------------------------------------------------------
		switch(g_variableData.Profile_Index)
		{
			case 0:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector21_DKS,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector21_DKS1_Addr, (uint8_t *)&g_DrivetData_DKS, 256);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector21_DKS2_Addr, (uint8_t *)&g_DrivetData_DKS.DKS_Buffer[256], 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			case 1:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector30_DKS,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector30_DKS1_Addr, (uint8_t *)&g_DrivetData_DKS, 256);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector30_DKS2_Addr, (uint8_t *)&g_DrivetData_DKS.DKS_Buffer[256], 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			case 2:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector39_DKS,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector39_DKS1_Addr, (uint8_t *)&g_DrivetData_DKS, 256);
				FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector39_DKS2_Addr, (uint8_t *)&g_DrivetData_DKS.DKS_Buffer[256], 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			default:
				break;
		}
		//----------------------------------------------------------------------------------------------------------------------
	}
	//----------------------------------------------------------------------------------------------------------------------
	if(Flag_NeedUpdate_FlashZB25V & FLASHZB25V_LED)
	{
		Flag_NeedUpdate_FlashZB25V &= (~FLASHZB25V_LED);
		//----------------------------------------------------------------------------------------------------------------------
		switch(g_variableData.Profile_Index)
		{
			case 0:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector22_LED,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector22_LED1_Addr, (uint8_t *)&g_DrivetData_LED, 256);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector22_LED2_Addr, (uint8_t *)&g_DrivetData_LED.User_Define_RGB[151], 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			case 1:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector31_LED,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector31_LED1_Addr, (uint8_t *)&g_DrivetData_LED, 256);
				FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector31_LED2_Addr, (uint8_t *)&g_DrivetData_LED.User_Define_RGB[151], 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			case 2:
				//------------------------------------------------------------------------------------------------------------------
				FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector40_LED,0);
				FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector40_LED1_Addr, (uint8_t *)&g_DrivetData_LED, 256);
				FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector40_LED2_Addr, (uint8_t *)&g_DrivetData_LED.User_Define_RGB[151], 256);
				//------------------------------------------------------------------------------------------------------------------
				break;
			default:
				break;
		}
		//----------------------------------------------------------------------------------------------------------------------
	}
    //----------------------------------------------------------------------------------------------------------------------
	if(Flag_NeedUpdate_FlashZB25V & FLASHZB25V_Macro)
	{
		Flag_NeedUpdate_FlashZB25V &= (~FLASHZB25V_Macro);
		/* 从block05开始，每一个page存一个宏，一共50个宏，虽然浪费空间，但是好算. */
		if((g_DrivetData_MACRO.Buffer_Macro[0]>0) && (g_DrivetData_MACRO.Buffer_Macro[0] != 0xFF))
		{
			g_DrivetData_MACRO.Buffer_Macro[0] = 1;
		}
		Macro_Address = FLASHZB25V_Block01_Sector23_MACRO_Addr + (g_DrivetData_MACRO.Buffer_Macro[0]-1)*FLASHZB25V_PageSize;
		FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector23_Macro,0);
		FlashZB25V_Write_Data(Macro_Address, (uint8_t *)&g_DrivetData_MACRO, 256);
	}
	//----------------------------------------------------------------------------------------------------------------------
	if(Flag_NeedUpdate_FlashZB25V & FLASHZB25V_KeyMatrix)
	{
		Flag_NeedUpdate_FlashZB25V &= (~FLASHZB25V_KeyMatrix);
		//----------------------------------------------------------------------------------------------------------------------
			switch(g_variableData.Profile_Index)
			{
				case 0:
					//------------------------------------------------------------------------------------------------------------------
					FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector24_KeyMatrix,0);
					FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector24_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.KeyMatrix_Buffer[0][0], 256);
					FlashZB25V_Write_Data(FLASHZB25V_Block01_Sector24_FN_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[0][0], 256);
					//------------------------------------------------------------------------------------------------------------------
					break;
				case 1:
					//------------------------------------------------------------------------------------------------------------------
					FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector33_KeyMatrix,0);
					FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector33_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.KeyMatrix_Buffer[0][0], 256);
					FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector33_FN_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[0][0], 256);
					//------------------------------------------------------------------------------------------------------------------
					break;
				case 2:
					//------------------------------------------------------------------------------------------------------------------
					FlashZB25V_Sector_Erase(FLASHZB25V_BLOCK1_Sector42_KeyMatrix,0);
					FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector42_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.KeyMatrix_Buffer[0][0], 256);
					FlashZB25V_Write_Data(FLASHZB25V_Block02_Sector42_FN_KeyMatrix_Addr, (uint8_t *)&g_DrivetData_KeyMatrix.FN_KeyMatrix_Buffer[0][0], 256);
					//------------------------------------------------------------------------------------------------------------------
					break;
				default:
					break;
			}
			//----------------------------------------------------------------------------------------------------------------------
	}
	//----------------------------------------------------------------------------------------------------------------------

	/**********************************************************************************************************************/
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
			FlashZB25V_Read_Data(FLASHZB25V_Block01_Sector16_MT_Addr, (uint8_t *) &g_DrivetData_MT, 256);
			//------------------------------------------------------------------------------------------
			for(uint16_t n = 0; n < 20; n++)
			{
				for(uint16_t m = 0; m < 7; m++)
				{
					usb_echo("g_DrivetData_MT.MT_Buffer[%d][%d]:0x%x\n",n,m,g_DrivetData_MT.MT_Buffer[n][m]);
				}
			}
		#endif
	#endif
}









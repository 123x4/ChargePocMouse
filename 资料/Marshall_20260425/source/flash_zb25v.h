/*
 * flash_ic.h
 *
 *  Created on: 2023年7月30日
 *  Author: Hao.Luo
 */

#ifndef FLASH_ZB25V_H_
#define FLASH_ZB25V_H_
#include <stdint.h>
#include <dma_config.h>
/*******************************************************************************
 * define
 ******************************************************************************/

//------------------------------------------------------------------------------------------
#define FLASHZB25V_Program  									0x02
#define FLASHZB25V_ReadData  									0x03
#define FLASHZB25V_WriteDisable  								0x04
#define FLASHZB25V_SR1_Reg										0x05
#define FLASHZB25V_WriteEnable  								0x06
#define FLASHZB25V_Erase	  									0x20
#define FLASHZB25V_ManufactureDeviceID_Reg  					0x90
//------------------------------------------------------------------------------------------
#define FLASHZB25V_ManufactureDeviceID							0x5E13
//------------------------------------------------------------------------------------------
#define FLASHZB25V_PageSize  									0x1000
#define FLASHZB25V_SectorSize  									0x0FFF
//------------------------------------------------------------------------------------------

/*******************************************************************************
 * 从0x010000这个地址开始存,每一个sector的一个page存一个profile
 * 0x010000-0x0101FF：存五组的Actual point---ActualTriggerPoint和ActualTriggerPoint mode；
 * 0x010200-0x0103FF：存五组按下触发值---Segment_SET_Trigger_Press[TRANSFER_Group][TRANSFER_SIZE]；
 * 0x010400-0x0105FF：存五组抬起触发值---Segment_SET_Trigger_Break[TRANSFER_Group][TRANSFER_SIZE]；
 ******************************************************************************/
//Profile0
#define FLASHZB25V_Block01_Sector16_ActualPoint_Addr			0x010000
#define FLASHZB25V_Block01_Sector17_PBPoint_Addr				0x011000
#define FLASHZB25V_Block01_Sector18_ContinuePoint_Addr			0x012000
#define FLASHZB25V_Block01_Sector19_TGL_Addr					0x013000
#define FLASHZB25V_Block01_Sector20_MT_Addr						0x014000
#define FLASHZB25V_Block01_Sector21_DKS1_Addr					0x015000
#define FLASHZB25V_Block01_Sector21_DKS2_Addr					0x015200
#define FLASHZB25V_Block01_Sector22_LED1_Addr					0x016000
#define FLASHZB25V_Block01_Sector22_LED2_Addr					0x016200
#define FLASHZB25V_Block01_Sector23_MACRO_Addr					0x017000
#define FLASHZB25V_Block01_Sector24_KeyMatrix_Addr				0x018000
#define FLASHZB25V_Block01_Sector24_FN_KeyMatrix_Addr			0x018200
//Profile1
#define FLASHZB25V_Block01_Sector25_ActualPoint_Addr			0x019000
#define FLASHZB25V_Block01_Sector26_PBPoint_Addr				0x01A000
#define FLASHZB25V_Block01_Sector27_ContinuePoint_Addr			0x01B000
#define FLASHZB25V_Block01_Sector28_TGL_Addr					0x01C000
#define FLASHZB25V_Block01_Sector29_MT_Addr						0x01D000
#define FLASHZB25V_Block01_Sector30_DKS1_Addr					0x01E000
#define FLASHZB25V_Block01_Sector30_DKS2_Addr					0x01E200
#define FLASHZB25V_Block01_Sector31_LED1_Addr					0x01F000
#define FLASHZB25V_Block01_Sector31_LED2_Addr					0x01F200
#define FLASHZB25V_Block02_Sector32_MACRO_Addr					0x020000
#define FLASHZB25V_Block02_Sector33_KeyMatrix_Addr				0x021000
#define FLASHZB25V_Block02_Sector33_FN_KeyMatrix_Addr			0x021200
//Profile2
#define FLASHZB25V_Block02_Sector34_ActualPoint_Addr			0x022000
#define FLASHZB25V_Block02_Sector35_PBPoint_Addr				0x023000
#define FLASHZB25V_Block02_Sector36_ContinuePoint_Addr			0x024000
#define FLASHZB25V_Block02_Sector37_TGL_Addr					0x025000
#define FLASHZB25V_Block02_Sector38_MT_Addr						0x026000
#define FLASHZB25V_Block02_Sector39_DKS1_Addr					0x027000
#define FLASHZB25V_Block02_Sector39_DKS2_Addr					0x027200
#define FLASHZB25V_Block02_Sector40_LED1_Addr					0x028000
#define FLASHZB25V_Block02_Sector40_LED2_Addr					0x028200
#define FLASHZB25V_Block02_Sector41_MACRO_Addr					0x029000
#define FLASHZB25V_Block02_Sector42_KeyMatrix_Addr				0x02A000
#define FLASHZB25V_Block02_Sector42_FN_KeyMatrix_Addr			0x02A200

enum
{
	FLASHZB25V_TrigerPoint 				= 0x01,
	FLASHZB25V_PBPoint 					= 0x02,
	FLASHZB25V_ContinueTrigerPoint 		= 0x04,
	FLASHZB25V_KeyMatrix 				= 0x08,
	FLASHZB25V_TGL		 				= 0x10,
	FLASHZB25V_MT		 				= 0x20,
	FLASHZB25V_DKS		 				= 0x40,
	FLASHZB25V_LED 						= 0x80,
	FLASHZB25V_Macro					= 0x100,
	FLASH5528_Data						= 0x200,
};

enum
{
	//Profile0
	FLASHZB25V_BLOCK1_Sector16_TriggerPoint 			= 0x00,
	FLASHZB25V_BLOCK1_Sector17_PBPoint					= 0x01,
	FLASHZB25V_BLOCK1_Sector18_ContinueTrigerPoint  	= 0x02,
	FLASHZB25V_BLOCK1_Sector19_TGL						= 0x03,
	FLASHZB25V_BLOCK1_Sector20_MT						= 0x04,
	FLASHZB25V_BLOCK1_Sector21_DKS						= 0x05,
	FLASHZB25V_BLOCK1_Sector22_LED						= 0x06,
	FLASHZB25V_BLOCK1_Sector23_Macro					= 0x07,
	FLASHZB25V_BLOCK1_Sector24_KeyMatrix				= 0x08,
	//Profile1
	FLASHZB25V_BLOCK1_Sector25_TriggerPoint 			= 0x09,
	FLASHZB25V_BLOCK1_Sector26_PBPoint					= 0x0A,
	FLASHZB25V_BLOCK1_Sector27_ContinueTrigerPoint  	= 0x0B,
	FLASHZB25V_BLOCK1_Sector28_TGL						= 0x0C,
	FLASHZB25V_BLOCK1_Sector29_MT						= 0x0D,
	FLASHZB25V_BLOCK1_Sector30_DKS						= 0x0E,
	FLASHZB25V_BLOCK1_Sector31_LED						= 0x0F,
	FLASHZB25V_BLOCK1_Sector32_Macro					= 0x10,//定义了宏的空间，但是没用,因为通用来说宏不跟profile走
	FLASHZB25V_BLOCK1_Sector33_KeyMatrix				= 0x11,
	//Profile2
	FLASHZB25V_BLOCK1_Sector34_TriggerPoint 			= 0x12,
	FLASHZB25V_BLOCK1_Sector35_PBPoint					= 0x13,
	FLASHZB25V_BLOCK1_Sector36_ContinueTrigerPoint  	= 0x14,
	FLASHZB25V_BLOCK1_Sector37_TGL						= 0x15,
	FLASHZB25V_BLOCK1_Sector38_MT						= 0x16,
	FLASHZB25V_BLOCK1_Sector39_DKS						= 0x17,
	FLASHZB25V_BLOCK1_Sector40_LED						= 0x18,
	FLASHZB25V_BLOCK1_Sector41_Macro					= 0x19,//定义了宏的空间，但是没用,因为通用来说宏不跟profile走
	FLASHZB25V_BLOCK1_Sector42_KeyMatrix				= 0x1A,
};

/*******************************************************************************
 * variable
 ******************************************************************************/
extern bool Flag_Read_FlashZB25V_ID_Success;
extern uint8_t Flag_FlashZB25V_PageIndex;
extern uint32_t	Flag_NeedUpdate_FlashZB25V;
extern uint8_t Tick_UpdateFlashZB25V;
/*******************************************************************************
 * FUNCTION
 ******************************************************************************/
extern bool FlashZB25V_PowerON_check_ID(void);
extern uint16_t FlashZB25V_Read_Data(uint32_t addr, uint8_t* pBuffer, uint16_t ReadLen);
extern uint16_t FlashZB25V_Sector_Erase(uint8_t FlashZB25V_BlockIndex,uint8_t FlashZB25V_PageIndex);
extern void FlashZB25V_Write_Data(uint32_t addr, uint8_t* pBuffer, uint16_t WriteLen);
extern void FlashZB25V_SectorErase(uint8_t FlashZB25V_BlockIndex,uint8_t FlashZB25V_PageIndex);
extern uint8_t FlashZB25V_SR1_Read(void);
extern void FlashZB25V_CheckBusy(void);
extern void FlashZB25V_Write_ENABLE(void);
extern void FlashZB25V_Write_Disable(void);
extern void FlashZB25V_Init(void);
extern void FlashZB25V_SaveAPPData(void);
#endif /* FLASH_IC_H */

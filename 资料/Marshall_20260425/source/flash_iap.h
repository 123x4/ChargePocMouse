/*
 * flash_iap.h
 *
 *  Created on: 2023年7月30日
 *  Author: Hao.Luo
 */

#ifndef FLASH_IAP_H_
#define FLASH_IAP_H_

#include "dma_config.h"
/*******************************************************************************
 * define
 ******************************************************************************/
/* Total Flash is 512k,use 500-512k for saving custom data */
#define PAGE_LEN 				0x200	//must be 0x200
#define PAGE_INDEX511_FROM_END 	1U   	//511page---0x7fe00 -> 0x7ffff//这个地址不要用
#define PAGE_INDEX510_FROM_END 	2U   	//510page---0x7fc00 -> 0x7fdff
#define PAGE_INDEX509_FROM_END 	3U   	//509page---0x7fa00 -> 0x7fbff
#define PAGE_INDEX508_FROM_END 	4U   	//508page---0x7f800 -> 0x7f9ff
#define PAGE_INDEX507_FROM_END 	5U   	//507page---0x7f600 -> 0x7f7ff
#define PAGE_INDEX506_FROM_END 	6U   	//506page---0x7f400 -> 0x7f5ff
#define PAGE_INDEX505_FROM_END 	7U   	//505page---0x7f200 -> 0x7f3ff
#define PAGE_INDEX504_FROM_END	8U   	//504page---0x7f000 -> 0x7f1ff
#define PAGE_INDEX503_FROM_END 	9U   	//503page---0x7ee00 -> 0x7efff
#define PAGE_INDEX502_FROM_END 	10U  	//502page---0x7ec00 -> 0x7edff
#define PAGE_INDEX501_FROM_END 	11U  	//501page---0x7ea00 -> 0x7ebff
#define PAGE_INDEX500_FROM_END 	12U  	//500page---0x7e800 -> 0x7e9ff

/*******************************************************************************
 * variable
 ******************************************************************************/
enum
{
    MCUFlash_WR_Success				= 0x01,
	MCUFlash_WR_EraseFail 		 	= 0x02,
	MCUFlash_WR_ProgramFail 		= 0x03,
};
//-------------------------------------------------------------------------------------------------------------------
typedef struct
{
	//-------------------------------------------------------------------------------------------------------------------
	/* 2 byte */
	uint8_t varInitFlag;
	uint8_t PageIndex;
	//-------------------------------------------------------------------------------------------------------------------
	/* 14 byte */
	uint8_t LED_ONOFF;
	uint8_t LED_MODE;
	uint8_t LED_BRIGHTNESS;
	uint8_t LED_SPEED;
	uint8_t LED_COLOR[3];
	uint8_t LED_RGB_MODE;
	uint8_t LIGHT_BEACON;
	uint8_t LIGHT_BEACON_COLOR[3];
	uint8_t LIGHT_BEACON_RGB_MODE;
	uint8_t LedColor_Index;
	uint8_t Profile_Index;
	//-------------------------------------------------------------------------------------------------------------------
	/* 5 byte */
	uint8_t DriverAPP_WASD_Switch;
	uint8_t DriverAPP_WinMacMode;
	uint8_t DriverAPP_WinLock_Switch;
	uint8_t DriverAPP_Nkey_Rollover_Switch;
	uint8_t DriverAPP_BottomTriggerState;
	//-------------------------------------------------------------------------------------------------------------------
	/* 用这个buffer来复制一份原始的矩阵，然后用来接受驱动修改的键值对应到这个buffer里面 */
	/* 6*21*2 = 252 byte */
	uint16_t Driver_KeyMatrix_Table[6][21];
	//-------------------------------------------------------------------------------------------------------------------
	/* 保存增益值 */
	/* 5*21 = 105 byte */
	uint8_t Gain_Buffer[6][21];
	//-------------------------------------------------------------------------------------------------------------------
	/* 加参数的时候要把这个保留的数组也同步减少，加多少这里就减多少 */
	/* 512-2-14-5-252 = 239 */
	//uint8_t reserve[134];
}DeviceVariableData;

extern DeviceVariableData   g_variableData;
//-------------------------------------------------------------------------------------------------------------------
extern uint32_t MCUFlashBlockBase;
extern uint32_t MCUFlashTotalSize;
extern uint32_t MCUFlashSectorSize;
extern uint32_t MCUFlashPageSize;
//-------------------------------------------------------------------------------------------------------------------
extern uint8_t	Tick_UpdateMCUFlash;
extern uint8_t	Flag_MCUFlash_WriteResult;
extern uint8_t 	Flag_MCUFlash_WRFail_Cnt;
extern uint8_t 	Flag_MCUFlash_PageIndex;
//-------------------------------------------------------------------------------------------------------------------
/*******************************************************************************
 * FUNCTION
 ******************************************************************************/
extern uint8_t Flash_IntervalFlash_Write(uint8_t *buffer, uint32_t Sector_Index, uint32_t Page_len);
static void Flash_IntervalFlash_Read(uint8_t *buffer, uint32_t Sector_Index, uint32_t Page_len);
extern void Variable_init(void);
extern void Variable_Reset(void);
extern void Flash_IntervalFlash_Init(void);
extern void VariableUpdate_MCUFlash_Process(void);
extern void Profile0_Save_Read(uint8_t Flag_Write_Read);
extern void Profile1_Save_Read(uint8_t Flag_Write_Read);
extern void Profile2_Save_Read(uint8_t Flag_Write_Read);
#endif /* FLASH_IAP_H */

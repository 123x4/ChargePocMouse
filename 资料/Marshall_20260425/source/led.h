/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __LED_H__
#define __LED_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/
enum
{
	LED_1_mode			= 0x00,		//光谱循环
	LED_2_mode 			= 0x01,		//常亮
	LED_3_mode 			= 0x02,		//左往右冲浪
	LED_4_mode 			= 0x03,		//呼吸
	LED_5_mode 			= 0x04,		//往中间蛇形跑灯
	LED_6_mode 			= 0x05,		//往下亮
	LED_7_mode 			= 0x06,		//旋转跑灯
	LED_8_mode 			= 0x07,		//两侧往中间浪
	LED_9_mode 			= 0x08,		//七彩喷泉
	LED_10_mode 		= 0x09,		//激光
	LED_20_mode			= 0x0A,		//自定义
	LED_FN_SPACE_mode	= 0x0B,		//FN+SPACE
};

enum
{
	LED_Update_IRGB					= 0x01,
	LED_Update_SET_Highlight		= 0x02,
	LED_Update_Clear_Highlight		= 0x04,
};

enum
{
	RGB_R		= 0x00,
	RGB_G		= 0x01,
	RGB_B		= 0x02,
};

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
}RGB_STRU;

typedef struct{
	uint8_t	row;
	uint8_t col;
}Position;

typedef struct
{
	/* 以256个byte为一组 */
	//-------------------------------------------------------------------------------------------------------------------
	/* 5*21 = 105 byte */
	uint8_t User_Custom[TRANSFER_Group][TRANSFER_SIZE];
	//-------------------------------------------------------------------------------------------------------------------
	/* 5*21*3 = 315 byte */
	uint8_t User_Define_RGB[315];
	//-------------------------------------------------------------------------------------------------------------------
	/* 用两个256来存 */
	/* 420 */
	uint8_t reserve[92];
}DrivetData_LED;
//------------------------------------------------------------------------------------------
extern DrivetData_LED   	g_DrivetData_LED;
//------------------------------------------------------------------------------------------


#define ONE_SETPS          	 	125
#define FIVE_SETPS 				250
#define ID_MAX					126
#define VOLUME_MAX 				10
#define Lighting_ColorMAX 		7
#define LED_ROW_MAX    			5
#define LED_COL_MAX    			21
#define LED_COL_Normal    		16
#define SPECTURN_COLOR_NUM		54
#define NULL_F					{0xFF,0xFF}
#define color_max 				64
/*******************************************************************************
 * mode9
 ******************************************************************************/
#define	LED_MODE9_LOOP_LINE_MAX	8
/*******************************************************************************
 * mode7
 ******************************************************************************/
#if 0

#endif
/*******************************************************************************
 * Variables
 ******************************************************************************/
extern uint8_t Flag_LedMode_mode;
extern uint8_t Flag_LedMode_ColorIndex;
extern uint8_t Flag_LedMode_ModeInit;
extern uint8_t Flag_LedMode_RGBUpdate;
extern uint8_t Flag_Light_Has_Change;
extern const uint8_t LightingColorTable[Lighting_ColorMAX][3];
extern const uint16_t LedVolumeTable[VOLUME_MAX];
extern const uint8_t Gamma_steps[color_max];
extern uint8_t sys_led_status_backup;
//------------------------------------------------------------------------------------------
/* mode5位移参数 */
extern uint8_t led_key_mark[LED_ROW_MAX][LED_COL_MAX];
extern uint8_t led_right_move[LED_ROW_MAX][LED_COL_MAX];
//------------------------------------------------------------------------------------------
/* RGB速度参数 */
extern uint8_t  Flag_LedMode_speed;
extern uint16_t Tick_RGBSpeed_cnt;
extern uint16_t RGB_SpeedLevel_cnt;
//------------------------------------------------------------------------------------------
extern uint16_t LED_Buffer_I[TRANSFER_Group][TRANSFER_SIZE];
extern uint16_t LED_Buffer_R[TRANSFER_Group][TRANSFER_SIZE];
extern uint16_t LED_Buffer_G[TRANSFER_Group][TRANSFER_SIZE];
extern uint16_t LED_Buffer_B[TRANSFER_Group][TRANSFER_SIZE];
//------------------------------------------------------------------------------------------
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
//------------------------------------------------------------------------------------------------------------
extern void Hall_Update_PWM(void);
extern void LED_Variable_Init(void);
extern void LedVolume_Dec(void);
extern void LedVolume_Inc(void);
extern void LedSpeed_Dec(void);
extern void LedSpeed_Inc(void);
extern void LedColorIndex_Inc(void);
extern void LedMode_Change(void);
extern void LedMode_Forward_Change(void);
extern void LedMode_Backward_Change(void);
extern void LEDMode_RGBdataProcess(void);
extern void LED_1_mode_RGBdataInit(void);
extern void LED_1_mode_RGBdataProcess(uint8_t RGB_index);
extern void LED_2_mode_RGBdataInit(void);
extern void LED_2_mode_RGBdataProcess(uint8_t RGB_index);
extern void LED_3_mode_RGBdataInit(void);
extern void LED_3_mode_RGBdataProcess(uint8_t RGB_index);
extern void LED_4_mode_RGBdataInit(void);
extern void LED_4_mode_RGBdataProcess(uint8_t RGB_index);
extern void LED_5_mode_RGBdataInit(void);
extern void LED_5_mode_RGBdataProcess(uint8_t RGB_index);
extern void LED_6_mode_RGBdataInit(void);
extern void LED_6_mode_RGBdataProcess(uint8_t RGB_index);
extern void LED_7_mode_RGBdataInit(void);
extern void LED_7_mode_RGBdataProcess(uint8_t RGB_index);
extern void LED_8_mode_RGBdataInit(void);
extern void LED_8_mode_RGBdataProcess(uint8_t RGB_index);
extern void LED_9_mode_RGBdataInit(void);
extern void LED_9_mode_RGBdataProcess(uint8_t RGB_index);
extern void LED_10_mode_RGBdataInit(void);
extern void LED_10_mode_RGBdataProcess(uint8_t RGB_index);
extern void LED_11_mode_RGBdataInit(void);
extern void LED_11_mode_RGBdataProcess(uint8_t RGB_index);
extern void LED_20_mode_RGBdataInit(void);
extern void LED_20_mode_RGBdataProcess(uint8_t RGB_index);
extern void LED_FN_SPACE_mode_RGBdataInit(void);
extern void Hall_PWMOff_Suspend(void);
extern void Ledoff(void);
extern void LED_Handle(void);
extern void sys_led_scan(void);
extern void Set_HighLight_Buffer(uint8_t Flag_AllKey, uint8_t Flag_Highlight, uint8_t KeyGroup, uint8_t Keyindex);
//------------------------------------------------------------------------------------------------------------
#endif /* __DMA_CONFIG_H__ */

/*
 * h6_hw_tab.h
 *
 *  Created on:
 *      Author:
 */

#ifndef SRC_H6_H6_HW_TAB_H_
#define SRC_H6_H6_HW_TAB_H_

#include "stdint.h"
#include "app_config.h"			// Provides the global HOSC_TEST build switch used by the H6 layout policy.
#include "gtry_gpio_drv.h"

#if HOSC_TEST					// Keep the compile-time HOSC feature gate controlled by app_config.h.
#define H6_HOSC_TEST_BUILD_ENABLE	1	// Build includes the HOSC-capable path.
#else
#define H6_HOSC_TEST_BUILD_ENABLE	0	// Build excludes the HOSC-capable path.
#endif

// Runtime gate: HOSC is allowed only when the build enables it.
// 96% keyboards keep the HOSC behavior.
// Full-size keyboards keep the HOSC behavior.
#define H6_HOSC_TEST_ENABLE			((H6_HOSC_TEST_BUILD_ENABLE) && \
									 ((g_keyboard_type == KB_TYPE_96_PERCENT) || \
									  (g_keyboard_type == KB_TYPE_FULL_SIZE)))

//#define POWER_LEVEL_CLK 			(H6_HOSC_TEST_ENABLE ? 7 : 4)	// Use stronger drive only on HOSC-enabled 96%/full-size layouts.
//#define POWER_LEVEL_SDO 			(H6_HOSC_TEST_ENABLE ? 7 : 4)	// Keep the original non-HOSC drive level for 65% and TKL.
//#define POWER_LEVEL_SDI 			(H6_HOSC_TEST_ENABLE ? 7 : 4)	// Keep data input drive policy aligned with CLK/SDO.

#define HASP_CHAIN0_SDO     	PA9
#define HASP_CHAIN0_SCK     	PA7
#define HASP_CHAIN0_SDI     	PA8

#define HASP_CHAIN1_SDO     	PA12
#define HASP_CHAIN1_SCK     	PA10
#define HASP_CHAIN1_SDI     	PA11

#define HASP_CHAIN2_SDO     	PA15
#define HASP_CHAIN2_SCK     	PA13
#define HASP_CHAIN2_SDI     	PA14

#define HASP_CHAIN3_SDO     	PA18
#define HASP_CHAIN3_SCK     	PA16
#define HASP_CHAIN3_SDI     	PA17

#define HASP_CHAIN4_SDO     	PA21
#define HASP_CHAIN4_SCK     	PA19
#define HASP_CHAIN4_SDI     	PA20

#define HASP_CHAIN5_SDO     	PA24
#define HASP_CHAIN5_SCK     	PA22
#define HASP_CHAIN5_SDI     	PA23

#define HASP_CHAIN6_SDO     	PA27
#define HASP_CHAIN6_SCK     	PA25
#define HASP_CHAIN6_SDI     	PA26

#define HASP_CHAIN7_SDO     	0xff
#define HASP_CHAIN7_SCK    		0xff
#define HASP_CHAIN7_SDI     	0xff

#define  KEY_DOWN_INCREASES		1

#define KRY_DOWN_VALUE			50			/**1000  Button trigger threshold **/
#define H6_TH					3		    //30/** H6 trigger threshold sampling threshold **/

#define KRY_DOWN_SLEEP_VALUE	500
#define KRY_UP_SLEEP_VALUE		50

#define ACTUAL_PRINT			100
#define RT_PRESS				50
#define RT_RELEASE				50

/*--------------------------------------------------------------------+
 * Per-layout parameter macros
 *--------------------------------------------------------------------*/
/* ---- JP65 ---- */
#define JP65_MAX_CHAIN_LENGTH       10
#define JP65_MAX_HASP_NUM           (JP65_MAX_CHAIN_LENGTH * HASP_CHANNL_NUM)
#define JP65_KEY_BOARD_NUM          70
#define JP65_CHAIN_LEN              {10, 10, 10, 10, 10, 10, 10, 0}

//You can find the index of the space_L and space_R LED by referring to the position of this "hall" array.
//	const uint8_t hall_index[KEY_BOARD_NUM] =
//	{
//		0,	7,	14,	21,	28,	35,	42,	49,	56,	63,   //10
//		1,	8,	15,	22,	29,	36,	43,	50,	57,	64,   //10
//		2,	9,	16,	23,	30,	37,	44,	51,	58,	65,   //10
//		3,	10,	17,	24,	31,	38,	45,	52,	59, 66,   //10
//		4,	11,	18,	25,	32,	39,	46,	53,	60,	67,   //10
//		5,	12,	19,	26,	33,	40,	47,	54,	61,	68,   //10
//		6,	13,	20,	27,	34,	41,	48,	55,	62,	69,   //10
//	};
#define JP65_R_L                    25
#define JP65_R_R                    46
#define JP65_G_L                    18
#define JP65_G_R                    39
#define JP65_B_L                    11
#define JP65_B_R                    32

/* ---- JP_TKL ---- */
#define JP_TKL_MAX_CHAIN_LENGTH     13
#define JP_TKL_MAX_HASP_NUM         (JP_TKL_MAX_CHAIN_LENGTH * HASP_CHANNL_NUM)
#define JP_TKL_KEY_BOARD_NUM        89
#define JP_TKL_CHAIN_LEN            {13, 13, 13, 13, 13, 12, 12, 0}

//You can find the index of the space_L and space_R LED by referring to the position of this "hall" array.
//	const uint8_t hall_index[KEY_BOARD_NUM] =
//	{
//		0,	7,	14,	21,	28,	35,	42,	49,	56,	63, 70, 77, 84,  //13
//		1,	8,	15,	22,	29,	36,	43,	50,	57,	64, 71, 78, 85,  //13
//		2,	9,	16,	23,	30,	37,	44,	51,	58,	65, 72, 79, 86,	 //13
//		3,	10,	17,	24,	31,	38,	45,	52,	59, 66, 73, 80, 87,	 //13
//		4,	11,	18,	25,	32,	39,	46,	53,	60,	67, 74, 81, 88,	 //13
//		5,	12,	19,	26,	33,	40,	47,	54,	61,	68, 75, 82,  	 //12
//		6,	13,	20,	27,	34,	41,	48,	55,	62,	69, 76, 83, 	 //12
//	};
#define JP_TKL_R_L                  11
#define JP_TKL_R_R                  53
#define JP_TKL_G_L                  32
#define JP_TKL_G_R                  46
#define JP_TKL_B_L                  25
#define JP_TKL_B_R                  39

/* ---- JP96 ---- */
#define JP96_MAX_CHAIN_LENGTH       15
#define JP96_MAX_HASP_NUM           (JP96_MAX_CHAIN_LENGTH * HASP_CHANNL_NUM)
#define JP96_KEY_BOARD_NUM          103
#define JP96_CHAIN_LEN              {15, 15, 14, 15, 15, 14, 15, 0}

//You can find the index of the space_L and space_R LED by referring to the position of this "hall" array.
//	const uint8_t hall_index[KEY_BOARD_NUM] =
//	{
//		0,	7,	14,	21,	28,	35,	42,	49,	56,	63, 70, 77, 84, 91, 98, //15
//		1,	8,	15,	22,	29,	36,	43,	50,	57,	64, 71, 78, 85, 92, 99, //15
//		2,	9,	16,	23,	30,	37,	44,	51,	58,	65, 72, 79, 86, 93,		//14
//		3,	10,	17,	24,	31,	38,	45,	52,	59, 66, 73, 80, 87, 94,101,	//15
//		4,	11,	18,	25,	32,	39,	46,	53,	60,	67, 74, 81, 88, 95,102,	//15
//		5,	12,	19,	26,	33,	40,	47,	54,	61,	68, 75, 82, 89, 96,	    //14
//		6,	13,	20,	27,	34,	41,	48,	55,	62,	69, 76, 83, 90, 97,104,	//15
//	};
#define JP96_R_L                    39
#define JP96_R_R                    60
#define JP96_G_L                    32
#define JP96_G_R                    53
#define JP96_B_L                    25
#define JP96_B_R                    46


/* ---- JP Full size ---- */
#define JP_FS_MAX_CHAIN_LENGTH       16
#define JP_FS_MAX_HASP_NUM           (JP_FS_MAX_CHAIN_LENGTH * HASP_CHANNL_NUM)
#define JP_FS_KEY_BOARD_NUM          108
#define JP_FS_CHAIN_LEN              {16, 16, 15, 16, 15, 14, 16, 0}

//You can find the index of the space_L and space_R LED by referring to the position of this "hall" array.
//	const uint8_t hall_index[KEY_BOARD_NUM] =
//	{
//		0,	7,	14,	21,	28,	35,	42,	49,	56,	63, 70, 77, 84, 91, 98,  105, 	//16
//		1,	8,	15,	22,	29,	36,	43,	50,	57,	64, 71, 78, 85, 92, 99,  106,	//16
//		2,	9,	16,	23,	30,	37,	44,	51,	58,	65, 72, 79, 86, 93,	100,		//15
//		3,	10,	17,	24,	31,	38,	45,	52,	59, 66, 73, 80, 87, 94,	101, 108	//16
//		4,	11,	18,	25,	32,	39,	46,	53,	60,	67, 74, 81, 88, 95,	102,		//15
//		5,	12,	19,	26,	33,	40,	47,	54,	61,	68, 75, 82, 89, 96,	    		//14
//		6,	13,	20,	27,	34,	41,	48,	55,	62,	69, 76, 83, 90, 97,	104, 111	//16
//	};
#define JP_FS_R_L                    25
#define JP_FS_R_R                    67
#define JP_FS_G_L                    46
#define JP_FS_G_R                    60
#define JP_FS_B_L                    39
#define JP_FS_B_R                    53

/* ---- US_65 ---- */
#define US_65_MAX_CHAIN_LENGTH     10
#define US_65_MAX_HASP_NUM         (US_65_MAX_CHAIN_LENGTH * HASP_CHANNL_NUM)
#define US_65_KEY_BOARD_NUM        66
#define US_65_CHAIN_LEN            {10, 10, 10, 9, 9, 9, 9, 0}

//You can find the index of the space_L and space_R LED by referring to the position of this "hall" array.
//	const uint8_t hall_index[KEY_BOARD_NUM] =
//	{
//		0,	7,	14,	21,	28,	35,	42,	49,	56,	63,   //10
//		1,	8,	15,	22,	29,	36,	43,	50,	57,	64,   //10
//		2,	9,	16,	23,	30,	37,	44,	51,	58,	65,   //10
//		3,	10,	17,	24,	31,	38,	45,	52,	59,   	  //9
//		4,	11,	18,	25,	32,	39,	46,	53,	60,	   	  //9
//		5,	12,	19,	26,	33,	40,	47,	54,	61,	      //9
//		6,	13,	20,	27,	34,	41,	48,	55,	62,	      //9
//	};
#define US_65_R_L                    25
#define US_65_R_R                    46
#define US_65_G_L                    18
#define US_65_G_R                    39
#define US_65_B_L                    11
#define US_65_B_R                    32

/* ---- US_TKL ---- */
#define US_TKL_MAX_CHAIN_LENGTH     13
#define US_TKL_MAX_HASP_NUM         (US_TKL_MAX_CHAIN_LENGTH * HASP_CHANNL_NUM)
#define US_TKL_KEY_BOARD_NUM        85
#define US_TKL_CHAIN_LEN            {12, 12, 13, 12, 12, 12, 12, 0}

//You can find the index of the space_L and space_R LED by referring to the position of this "hall" array.
//	const uint8_t hall_index[KEY_BOARD_NUM] =
//	{
//		0,	7,	14,	21,	28,	35,	42,	49,	56,	63, 70, 77,  //12
//		1,	8,	15,	22,	29,	36,	43,	50,	57,	64, 71, 78,  //12
//		2,	9,	16,	23,	30,	37,	44,	51,	58,	65, 72, 79,  86,//13
//		3,	10,	17,	24,	31,	38,	45,	52,	59, 66, 73, 80,  //12
//		4,	11,	18,	25,	32,	39,	46,	53,	60,	67, 74, 81,  //12
//		5,	12,	19,	26,	33,	40,	47,	54,	61,	68, 75, 82,  //12
//		6,	13,	20,	27,	34,	41,	48,	55,	62,	69, 76, 83,  //12
//	};
#define US_TKL_R_L                  11
#define US_TKL_R_R                  53
#define US_TKL_G_L                  32
#define US_TKL_G_R                  46
#define US_TKL_B_L                  25
#define US_TKL_B_R                  39

/* ---- UK_65 ---- */
#define UK_65_MAX_CHAIN_LENGTH     10
#define UK_65_MAX_HASP_NUM         (UK_65_MAX_CHAIN_LENGTH * HASP_CHANNL_NUM)
#define UK_65_KEY_BOARD_NUM        67
#define UK_65_CHAIN_LEN            {9, 10, 10, 10, 9, 9, 10, 0}

//You can find the index of the space_L and space_R LED by referring to the position of this "hall" array.
//	const uint8_t hall_index[KEY_BOARD_NUM] =
//	{
//		0,	7,	14,	21,	28,	35,	42,	49,	56,	      //9
//		1,	8,	15,	22,	29,	36,	43,	50,	57,	64,   //10
//		2,	9,	16,	23,	30,	37,	44,	51,	58,	65,   //10
//		3,	10,	17,	24,	31,	38,	45,	52,	59, 66,   //10
//		4,	11,	18,	25,	32,	39,	46,	53,	60,	   	  //9
//		5,	12,	19,	26,	33,	40,	47,	54,	61,	      //9
//		6,	13,	20,	27,	34,	41,	48,	55,	62,	69,   //10
//	};

#define UK_65_R_L                    25
#define UK_65_R_R                    46
#define UK_65_G_L                    18
#define UK_65_G_R                    39
#define UK_65_B_L                    11
#define UK_65_B_R                    32

/* ---- US96 ---- */
#define US96_MAX_CHAIN_LENGTH       15
#define US96_MAX_HASP_NUM           (US96_MAX_CHAIN_LENGTH * HASP_CHANNL_NUM)
#define US96_KEY_BOARD_NUM          99
#define US96_CHAIN_LEN              {15, 14, 14, 14, 14, 14, 14, 0}

//You can find the index of the space_L and space_R LED by referring to the position of this "hall" array.
//	const uint8_t hall_index[KEY_BOARD_NUM] =
//	{
//		0,	7,	14,	21,	28,	35,	42,	49,	56,	63, 70, 77, 84, 91, 98, //15
//		1,	8,	15,	22,	29,	36,	43,	50,	57,	64, 71, 78, 85, 92,  	//14
//		2,	9,	16,	23,	30,	37,	44,	51,	58,	65, 72, 79, 86, 93,		//14
//		3,	10,	17,	24,	31,	38,	45,	52,	59, 66, 73, 80, 87, 94,		//14
//		4,	11,	18,	25,	32,	39,	46,	53,	60,	67, 74, 81, 88, 95,		//14
//		5,	12,	19,	26,	33,	40,	47,	54,	61,	68, 75, 82, 89, 96,	    //14
//		6,	13,	20,	27,	34,	41,	48,	55,	62,	69, 76, 83, 90, 97,		//14
//	};

#define US96_R_L                    39
#define US96_R_R                    60
#define US96_G_L                    32
#define US96_G_R                    53
#define US96_B_L                    25
#define US96_B_R                    46

/* ---- US Full size ---- */
#define US_FS_MAX_CHAIN_LENGTH       16
#define US_FS_MAX_HASP_NUM           (US_FS_MAX_CHAIN_LENGTH * HASP_CHANNL_NUM)
#define US_FS_KEY_BOARD_NUM          104
#define US_FS_CHAIN_LEN              {16, 15, 16, 15, 14, 12, 16, 0}

//You can find the index of the space_L and space_R LED by referring to the position of this "hall" array.
//	const uint8_t hall_index[KEY_BOARD_NUM] =
//	{
//		0,	7,	14,	21,	28,	35,	42,	49,	56,	63, 70, 77, 84, 91, 98,  105, 	//16
//		1,	8,	15,	22,	29,	36,	43,	50,	57,	64, 71, 78, 85, 92, 99,  		//15
//		2,	9,	16,	23,	30,	37,	44,	51,	58,	65, 72, 79, 86, 93,	100, 107	//16
//		3,	10,	17,	24,	31,	38,	45,	52,	59, 66, 73, 80, 87, 94,	101, 		//15
//		4,	11,	18,	25,	32,	39,	46,	53,	60,	67, 74, 81, 88, 95,				//14
//		5,	12,	19,	26,	33,	40,	47,	54,	61,	68, 75, 82, 		    		//12
//		6,	13,	20,	27,	34,	41,	48,	55,	62,	69, 76, 83, 90, 97,	104, 111	//16
//	};

#define US_FS_R_L                    25
#define US_FS_R_R                    67
#define US_FS_G_L                    46
#define US_FS_G_R                    60
#define US_FS_B_L                    39
#define US_FS_B_R                    53

/* ---- UK_TKL ---- */
#define UK_TKL_MAX_CHAIN_LENGTH     13
#define UK_TKL_MAX_HASP_NUM         (UK_TKL_MAX_CHAIN_LENGTH * HASP_CHANNL_NUM)
#define UK_TKL_KEY_BOARD_NUM        86
#define UK_TKL_CHAIN_LEN            {12, 12, 13, 12, 13, 12, 12, 0}

//You can find the index of the space_L and space_R LED by referring to the position of this "hall" array.
//	const uint8_t hall_index[KEY_BOARD_NUM] =
//	{
//		0,	7,	14,	21,	28,	35,	42,	49,	56,	63, 70, 77,  	 //12
//		1,	8,	15,	22,	29,	36,	43,	50,	57,	64, 71, 78,  	 //12
//		2,	9,	16,	23,	30,	37,	44,	51,	58,	65, 72, 79,  86, //13
//		3,	10,	17,	24,	31,	38,	45,	52,	59, 66, 73, 80, 	 //12
//		4,	11,	18,	25,	32,	39,	46,	53,	60,	67, 74, 81,  	 //12
//		5,	12,	19,	26,	33,	40,	47,	54,	61,	68, 75, 82,  	 //12
//		6,	13,	20,	27,	34,	41,	48,	55,	62,	69, 76, 83,  	 //12
//	};
#define UK_TKL_R_L                  18
#define UK_TKL_R_R                  53
#define UK_TKL_G_L                  39
#define UK_TKL_G_R                  53
#define UK_TKL_B_L                  32
#define UK_TKL_B_R                  46

/* ---- UK96 ---- */
#define UK96_MAX_CHAIN_LENGTH       15
#define UK96_MAX_HASP_NUM           (UK96_MAX_CHAIN_LENGTH * HASP_CHANNL_NUM)
#define UK96_KEY_BOARD_NUM          100
#define UK96_CHAIN_LEN              {15, 14, 14, 14, 15, 14, 14, 0}

//You can find the index of the space_L and space_R LED by referring to the position of this "hall" array.
//	const uint8_t hall_index[KEY_BOARD_NUM] =
//	{
//		0,	7,	14,	21,	28,	35,	42,	49,	56,	63, 70, 77, 84, 91, 98, //15
//		1,	8,	15,	22,	29,	36,	43,	50,	57,	64, 71, 78, 85, 92,  	//14
//		2,	9,	16,	23,	30,	37,	44,	51,	58,	65, 72, 79, 86, 93,		//14
//		3,	10,	17,	24,	31,	38,	45,	52,	59, 66, 73, 80, 87, 94,		//14
//		4,	11,	18,	25,	32,	39,	46,	53,	60,	67, 74, 81, 88, 95,		//14
//		5,	12,	19,	26,	33,	40,	47,	54,	61,	68, 75, 82, 89, 96,	    //14
//		6,	13,	20,	27,	34,	41,	48,	55,	62,	69, 76, 83, 90, 97,		//14
//	};
#define UK96_R_L                    46
#define UK96_R_R                    67
#define UK96_G_L                    39
#define UK96_G_R                    60
#define UK96_B_L                    32
#define UK96_B_R                    53

/* ---- UK Full size ---- */
#define UK_FS_MAX_CHAIN_LENGTH       16
#define UK_FS_MAX_HASP_NUM           (UK_FS_MAX_CHAIN_LENGTH * HASP_CHANNL_NUM)
#define UK_FS_KEY_BOARD_NUM          105
#define UK_FS_CHAIN_LEN              {16, 15, 15, 16, 15, 12, 16, 0}

//You can find the index of the space_L and space_R LED by referring to the position of this "hall" array.
//	const uint8_t hall_index[KEY_BOARD_NUM] =
//	{
//		0,	7,	14,	21,	28,	35,	42,	49,	56,	63, 70, 77, 84, 91, 98,  105, 	//16
//		1,	8,	15,	22,	29,	36,	43,	50,	57,	64, 71, 78, 85, 92, 99,  		//15
//		2,	9,	16,	23,	30,	37,	44,	51,	58,	65, 72, 79, 86, 93,	100, 		//16
//		3,	10,	17,	24,	31,	38,	45,	52,	59, 66, 73, 80, 87, 94,	101, 108,	//15
//		4,	11,	18,	25,	32,	39,	46,	53,	60,	67, 74, 81, 88, 95,	102,		//14
//		5,	12,	19,	26,	33,	40,	47,	54,	61,	68, 75, 82, 		    		//12
//		6,	13,	20,	27,	34,	41,	48,	55,	62,	69, 76, 83, 90, 97,	104, 111	//16
//	};

#define UK_FS_R_L                    32
#define UK_FS_R_R                    74
#define UK_FS_G_L                    53
#define UK_FS_G_R                    67
#define UK_FS_B_L                    46
#define UK_FS_B_R                    60

/*Key Code*/
#define RESERVE  							0xFF
#define STEEL 								0xF0

#define NONE	  							0x00
#define KEY_ERRORROLLOVER 					0x01
#define KEY_POSTFAIL 						0x02
#define KEY_ERRORUNDEFINED 					0x03
#define KEY_A 								0x04
#define KEY_B 								0x05
#define KEY_C 								0x06
#define KEY_D 								0x07
#define KEY_E 								0x08
#define KEY_F 								0x09
#define KEY_G 								0x0A
#define KEY_H 								0x0B
#define KEY_I 								0x0C
#define KEY_J 								0x0D
#define KEY_K 								0x0E
#define KEY_L 								0x0F
#define KEY_M 								0x10
#define KEY_N 								0x11
#define KEY_O 								0x12
#define KEY_P 								0x13
#define KEY_Q 								0x14
#define KEY_R 								0x15
#define KEY_S 								0x16
#define KEY_T 								0x17
#define KEY_U 								0x18
#define KEY_V 								0x19
#define KEY_W 								0x1A
#define KEY_X 								0x1B
#define KEY_Y 								0x1C
#define KEY_Z 								0x1D
#define KEY_1 								0x1E
#define KEY_2 								0x1F
#define KEY_3 								0x20
#define KEY_4 								0x21
#define KEY_5 								0x22
#define KEY_6 								0x23
#define KEY_7 								0x24
#define KEY_8 								0x25
#define KEY_9 								0x26
#define KEY_0 								0x27
#define KEY_ENTER 							0x28
#define KEY_ESC 							0x29
#define KEY_BACKSPACE 						0x2A
#define KEY_TAB 							0x2B
#define KEY_SPACE							0x2C
#define KEY_MINUS 							0x2D			//[-_]
#define KEY_PLUS 							0x2E			//[=+]
#define KEY_OBRACE 							0x2F			//[[{]
#define KEY_CBRACE 							0x30			//[]}]
#define KEY_BACKSLASH						0x31			//k29 \|
#define KEY_K42								0x32  			//k42
#define KEY_COLON 							0x33			//[;:]
#define KEY_QUOTE 							0x34			//['"]
#define KEY_TILDE 							0x35			//[`~]
#define KEY_COMMA 							0x36			//[,<]
#define KEY_DOT 							0x37			//[.>]
#define KEY_QUESTION		 				0x38			//[/?]
#define KEY_CAPS 							0x39
#define KEY_F1 								0x3A
#define KEY_F2 								0x3B
#define KEY_F3 								0x3C
#define KEY_F4 								0x3D
#define KEY_F5 								0x3E
#define KEY_F6 								0x3F
#define KEY_F7 								0x40
#define KEY_F8 								0x41
#define KEY_F9 								0x42
#define KEY_F10 							0x43
#define KEY_F11 							0x44
#define KEY_F12 							0x45

#define KEY_PRINT							0x46
#define KEY_SCROLL_LOCK 					0x47
#define KEY_PAUSE 							0x48
#define KEY_INSERT 							0x49
#define KEY_HOME 							0x4A
#define KEY_PAGEUP 							0x4B
#define KEY_DELETE 							0x4C
#define KEY_END 							0x4D
#define KEY_PAGEDOWN 						0x4E

#define KEY_RIGHT 							0x4F
#define KEY_LEFT 							0x50
#define KEY_DOWN 							0x51
#define KEY_UP								0x52

#define KEY_KEYPAD_NUM_LOCK_AND_CLEAR 		0x53
#define KEY_KEYPAD_SLASH 					0x54
#define KEY_KEYPAD_MUL 						0x55
#define KEY_KEYPAD_MINUS 					0x56
#define KEY_KEYPAD_PLUS 					0x57
#define KEY_KEYPAD_ENTER 					0x58
#define KEY_KEYPAD_1	 					0x59
#define KEY_KEYPAD_2						0x5A
#define KEY_KEYPAD_3						0x5B
#define KEY_KEYPAD_4						0x5C
#define KEY_KEYPAD_5 						0x5D
#define KEY_KEYPAD_6 						0x5E
#define KEY_KEYPAD_7 						0x5F
#define KEY_KEYPAD_8 						0x60
#define KEY_KEYPAD_9						0x61
#define KEY_KEYPAD_0						0x62
#define KEY_KEYPAD_DOT						0x63
#define KEY_SLASH 							0x64 //KEY_UK_VK_OEM_102
#define KEY_APPLICATION 					0x65
#define KEY_POWER 							0x66
#define KEY_KEYPAD_EQUAL 					0x67

#define KEY_F13 							0x68
#define KEY_F14 							0x69
#define KEY_F15 							0x6A
#define KEY_F16 							0x6B
#define KEY_F17 							0x6C
#define KEY_F18 							0x6D
#define KEY_F19 							0x6E
#define KEY_F20 							0x6F
#define KEY_F21 							0x70
#define KEY_F22 							0x71
#define KEY_F23 							0x72
#define KEY_F24 							0x73
#define KEY_EXECUTE 						0x74
#define KEY_HELP 							0x75
#define KEY_MENU 							0x76
#define KEY_SELECT 							0x77
#define KEY_STOP 							0x78
#define KEY_AGAIN 							0x79
#define KEY_UNDO 							0x7A
#define KEY_CUT			 					0x7B
#define KEY_COPY 							0x7C
#define KEY_PASTE 							0x7D
#define KEY_FIND 							0x7E
#define KEY_MUTE 							0x7F
#define KEY_VOLUME_UP 						0x80
#define KEY_VOLUME_DOWN 					0x81
#define KEY_LOCKING_CAPS_LOCK 				0x82
#define KEY_LOCKING_NUM_LOCK 				0x83
#define KEY_LOCKING_SCROLL_LOCK 			0x84
#define KEY_KEYPAD_COMMA 					0x85
#define KEY_KEYPAD_EQUAL_SIGN 				0x86
#define KEY_K56	 							0x87  //k56
#define KEY_KANA							0x88
#define KEY_K14			 					0x89
#define KEY_K132		 					0x8A
#define KEY_K131		 					0x8B
#define KEY_INTERNATIONAL6 					0x8C
#define KEY_INTERNATIONAL7 					0x8D
#define KEY_INTERNATIONAL8 					0x8E
#define KEY_INTERNATIONAL9 					0x8F
#define KEY_LANG1 							0x90
#define KEY_LANG2 							0x91
#define KEY_LANG3 							0x92
#define KEY_LANG4 							0x93
#define KEY_LANG5 							0x94
#define KEY_LANG6 							0x95
#define KEY_LANG7 							0x96
#define KEY_LANG8 							0x97
#define KEY_LANG9 							0x98
#define KEY_ALTERNATE_ERASE 				0x99
#define KEY_SYSREQ 							0x9A
#define KEY_CANCEL 							0x9B
#define KEY_CLEAR 							0x9C
#define KEY_PRIOR 							0x9D
#define KEY_RETURN 							0x9E
#define KEY_SEPARATOR 						0x9F
#define KEY_OUT 							0xA0
#define KEY_OPER 							0xA1
#define KEY_CLEAR_AGAIN 					0xA2
#define KEY_CRSEL 							0xA3
#define KEY_EXSEL 							0xA4
#define KEY_KEYPAD_00 						0xB0
#define KEY_KEYPAD_000 						0xB1
#define KEY_THOUSANDS_SEPARATOR 			0xB2
#define KEY_DECIMAL_SEPARATOR 				0xB3
#define KEY_CURRENCY_UNIT 					0xB4
#define KEY_CURRENCY_SUB_UNIT 				0xB5
#define KEY_KEYPAD_OPARENTHESIS 			0xB6
#define KEY_KEYPAD_CPARENTHESIS 			0xB7
#define KEY_KEYPAD_OBRACE 					0xB8
#define KEY_KEYPAD_CBRACE 					0xB9
#define KEY_KEYPAD_TAB 						0xBA
#define KEY_KEYPAD_BACKSPACE 				0xBB
#define KEY_KEYPAD_A 						0xBC
#define KEY_KEYPAD_B 						0xBD
#define KEY_KEYPAD_C 						0xBE
#define KEY_KEYPAD_D 						0xBF
#define KEY_KEYPAD_E 						0xC0
#define KEY_KEYPAD_F 						0xC1
#define KEY_KEYPAD_XOR 						0xC2
#define KEY_KEYPAD_CARET 					0xC3
#define KEY_KEYPAD_PERCENT 					0xC4
#define KEY_KEYPAD_LESS 					0xC5
#define KEY_KEYPAD_GREATER 					0xC6
#define KEY_KEYPAD_AMPERSAND 				0xC7
#define KEY_KEYPAD_LOGICAL_AND 				0xC8
#define KEY_KEYPAD_VERTICAL_BAR 			0xC9
#define KEY_KEYPAD_LOGIACL_OR 				0xCA
#define KEY_KEYPAD_COLON 					0xCB
#define KEY_KEYPAD_NUMBER_SIGN 				0xCC
#define KEY_KEYPAD_SPACE 					0xCD
#define KEY_KEYPAD_AT 						0xCE
#define KEY_KEYPAD_EXCLAMATION_MARK	 		0xCF
#define KEY_KEYPAD_MEMORY_STORE 			0xD0
#define KEY_KEYPAD_MEMORY_RECALL 			0xD1
#define KEY_KEYPAD_MEMORY_CLEAR 			0xD2
#define KEY_KEYPAD_MEMORY_ADD 				0xD3
#define KEY_KEYPAD_MEMORY_SUBTRACT 			0xD4
#define KEY_KEYPAD_MEMORY_MULTIPLY 			0xD5
#define KEY_KEYPAD_MEMORY_DIVIDE 			0xD6
#define KEY_KEYPAD_PLUSMINUS 				0xD7
#define KEY_KEYPAD_CLEAR 					0xD8
#define KEY_KEYPAD_CLEAR_ENTRY 				0xD9
#define KEY_KEYPAD_BINARY 					0xDA
#define KEY_KEYPAD_OCTAL 					0xDB
#define KEY_KEYPAD_DECIMAL 					0xDC
#define KEY_KEYPAD_HEXADECIMAL 				0xDD

#define   KEY_L_CTRL       					0xE0//0x01
#define   KEY_L_SHIFT					    0xE1//0x02
#define   KEY_L_ALT        					0xE2//0x04
#define   KEY_L_WIN        					0xE3//0x08
#define   KEY_R_CTRL      	 				0xE4//0x10
#define   KEY_R_SHIFT      					0xE5//0x20
#define   KEY_R_ALT        					0xE6//0x40
#define   KEY_R_WIN        					0xE7//0x80

#define   KEY_L_CTRL_B       				0xE0
#define   KEY_L_SHIFT_B     				0xE1
#define   KEY_L_ALT_B       				0xE2
#define   KEY_L_WIN_B        				0xE3
#define   KEY_R_CTRL_B       				0xE4
#define   KEY_R_SHIFT_B      				0xE5
#define   KEY_R_ALT_B        				0xE6
#define   KEY_R_WIN_B        				0xE7


#define KEY_BLUETOOTH_MODETONS_NUM			10

#define HALL_CHECK_COUNTS		128
#define HALL_REG_READ_WRITE_VERIFY_COUNTS  3

#define LINE_SECTION 			400//400//33			/** Number of linearization segments **/
#define LINE_LEN 				4096//4096      /** Linearization segment value **/

#ifndef HASP_KEY_NUM_MAX
#define HASP_KEY_NUM_MAX		128
#endif

#define HASP_CHANNL_NUM			7
#define HASP_CHAIN_LENGTH		16
#define HASP_KEY_NUM			112
#define KEY_BOARD_NUM			105

#define HASP_CHANNL_NUM_MAX		8				/** Maximum number of keyboard chains **/

/********************** HASP Parameter Configuration *****************************/
#define MAX_ABSDATA_VALUE		(7000)			/** Maximum effective value setting after converting raw sampled data to absolute value **/
#define MIN_ABSDATA_VALUE		(0)				/** Minimum effective value setting after converting raw sampled data to absolute value **/
#define RAW2ABS_REF_VALUE		(0)				/** Reference value for converting raw data to absolute value: absolute value = raw data - reference value **/
#define FDDEL_KDLP_VALUE		(0)				/** Fast data update adjustment threshold **/
#define NOISEV_KDLP_VALUE		(0)		 		/** Fast data update noise configuration **/
#define FDDEL_BTOPLP_VALUE		(8192)			/** Fast top update adjustment threshold **/
#define NOISEV_BTOPLP_VALUE		(20)			/** Fast top update noise configuration **/
#define BTOPLP_PDLT_VALUE		0				/** Top filter positive deviation adjustment range **/
#define BTOPLP_NDLT_VALUE		0				/** Top filter negative deviation adjustment range **/
#define BTOP_CALCYCLE_VALUE		(100)			/** Baseline top calculation cycle configuration **/
#define BTOP_ADDCNT_VALUE		(50)			/** Number of consecutive increases for Baseline top data adjustment **/
#define BTOP_SUBCNT_VALUE		(50)			/** Number of consecutive decreases for Baseline top data adjustment **/
#define BTOP_ADDDLT_VALUE		(2<<2)			/** Increment value per Baseline top increase adjustment **/
#define BTOP_SUBDLT_VALUE		(2<<2)			/** Decrement value per Baseline top decrease adjustment **/
#define BTOP_PHYST_VALUE		(5<<2)			/** Positive hysteresis for Baseline top adjustment decision **/
#define BTOP_NHYST_VALUE		(5<<2)			/** Negative hysteresis for Baseline top adjustment decision **/
#define BTOP_PDLT_VALUE			10				/** Top baseline auto-adjustment positive deviation range **/
#define BTOP_NDLT_VALUE			10				/** Top baseline auto-adjustment negative deviation range **/
#define BBOT_CALCYCLE_VALUE		(50)			/** Baseline bottom calculation cycle configuration **/
#define BBOT_LOCKCNT_VALUE		(4)				/** Baseline bottom adjustment cycle configuration after a single key press **/
#define BBOT_ADDDLT_VALUE		(50<<2)			/** Baseline bottom increment configuration, applied when sampled value exceeds current Baseline bottom value by BASLBOT_HYST2 **/
#define BBOT_SUBDLT_VALUE		(10<<2)			/** Decrement value for Baseline bottom after each key release, adjusted only once per key press **/
#define BBOT_HYST1_VALUE		(5<<2)			/** Baseline bottom adjustment threshold 1 **/
#define BBOT_HYST2_VALUE		(500<<2)		/** Baseline bottom adjustment threshold 2 **/
#define BBOT_PDLT_VALUE			(1000<<2)		/** Bottom baseline auto-adjustment positive deviation range **/
#define BBOT_NDLT_VALUE			(1000<<2)		/** Bottom baseline auto-adjustment negative deviation range **/
#define BBOT_LKRLS_VALUE		(100)			/** Threshold for key release detection: key is considered released when the linearized value is less than this threshold **/
#define PRESSDN_CFG_VALUE		(800)			/** Threshold for key press detection: key is considered pressed when the linearized value is greater than this threshold **/

#define HASP_CRC_ERROR_HALL_TIMES	(3)
#define HASP_AUTO_TRAIN_TIMES	(128)
#define BASLTOP_VALUE			(210<<2)		/** Top baseline register, lower 2 bits are fractional bits. The addresses correspond to the order of reception from low to high. **/
#define BASLBOT_VALUE			(11000)			/** Bottom baseline register, lower 2 bits are fractional bits. The addresses correspond to the order of reception from low to high. **/
#define RAWDLP_VALUE			(0)				/** RAWDATA LP data path low-pass filter register, lower 18 bits are fractional bits **/
#define BTOPLP_VALUE			(210<<18)		/** Top baseline LP calculation path low-pass filter register, lower 18 bits are fractional bits **/
#define BBOTLP_VALUE			(3000<<18)		/** Bottom baseline LP calculation path low-pass filter register, lower 18 bits are fractional bits **/
#define BTOP_INIT_VALUE			(200<<2)//(210<<2)	/** Top baseline LP initialization value, 16-bit width. After power-on, it is filled with the data from the data path low-pass filter register (right-shifted by 16 bits) when the key is released. **/
#define KEY_OFFSET_REF			(200)			/** Key offset reference **/
#define DATAERR_INIT_VALUE		(0)

#define HASP_SCK_DIV			(4)				/** HASP SCK divider ratio, SCK = INNER_CLK / HASP_SCK_DIV **/


#define TEST_FREQ				0

#if TEST_FREQ
// Spectrum Configuration
#define FREQUENCY_RANGE			340				/** SCK spread spectrum frequency range setting				0xD4	1~340	MAX (FREQUENCY_RANGE * 35K)  **/
#define FREQUENCY_RATE			96				/** SCK spread spectrum frequency adjustment rate setting	0xD8	2-81	50 = 1us  FREQUENCY_RATE > 100 **/
#define FREQUENCY_STEP			15				/** SCK spread spectrum frequency step setting				0xD8	0-15    FREQUENCY_STEP * 35K step **/

#else
#define FREQUENCY_RANGE			80
#define FREQUENCY_RATE			56
#define FREQUENCY_STEP			8
#endif

#define CYCLE_TIME_SPREAD_SPECTRUM  (FREQUENCY_RANGE / 113)		/** Compensate for the time variation caused by spread spectrum by adjusting the TD1 duration **/

typedef enum
{
	KB_TYPE_65_PERCENT = 0,
	KB_TYPE_TKL        = 1,
	KB_TYPE_96_PERCENT = 2,
	KB_TYPE_FULL_SIZE  = 3,
} keyboard_type_t;

typedef enum
{
	LANG_US      = 0,
	LANG_UK      = 1,
	LANG_JP      = 2,
	LANG_UNKNOWN = 3,
} language_type_t;

typedef enum
{
	PRODUCT_XENO   = 0,
	PRODUCT_JARVIS = 1,
} product_series_t;

#pragma pack(push, 1)
typedef struct
{
	const uint8_t *usage_id;
	uint8_t table_size;
	uint8_t chain_length;
	uint8_t hasp_num;
	uint8_t actual_board_num;
	uint8_t chain_len[HASP_CHANNL_NUM_MAX];
	uint8_t r_l;
	uint8_t r_r;
	uint8_t g_l;
	uint8_t g_r;
	uint8_t b_l;
	uint8_t b_r;
} layout_cfg_t;
#pragma pack(pop)

extern const uint32_t sck_pins[];
extern const uint32_t sdo_pins[];
extern const uint32_t sdi_pins[];

extern uint8_t chain_len_buff[];

extern uint8_t g_hasp_chain_length;
extern uint8_t g_hasp_key_num;
extern uint8_t g_key_board_num;
extern uint8_t g_keyboard_type;
extern uint8_t g_language_type;
extern uint8_t g_product_series;

extern uint8_t g_space_r_l;
extern uint8_t g_space_r_r;
extern uint8_t g_space_g_l;
extern uint8_t g_space_g_r;
extern uint8_t g_space_b_l;
extern uint8_t g_space_b_r;

extern uint8_t Key_HID_Usage_ID[HASP_KEY_NUM];
extern  uint8_t chain_miss_buff_reg[HASP_KEY_NUM];
extern  uint8_t chain_miss_buff_led[HASP_KEY_NUM];

extern uint16_t Hall_Linearization_table[];
extern const layout_cfg_t k_layouts[];

extern void calc_chain_miss_buff_reg(void);
extern void calc_chain_miss_buff_led(void);
extern void h6_hal_fill_crc_null_hall(void);

#endif /* SRC_H6_H6_HW_TAB_H_ */

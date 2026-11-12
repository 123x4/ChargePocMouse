/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MACRO_H__
#define __MACRO_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct
{
	uint8_t 	fIntoMacro    		: 1;
	uint8_t		fReadROMdata      	: 1;
	uint8_t		fKeyDelay         	: 1;
	uint8_t		fTime1ms         	: 1;
	uint8_t		fStartCount			: 1;
	uint8_t		fContinueCount		: 1;
	uint8_t		Macro_Pres			: 1;
	uint8_t		Macro_Break			: 1;
	uint8_t		Macro_KeyPres		: 1;
	uint8_t		Macro_KeyBreak		: 1;
}flagField;

typedef  struct
{
	uint8_t  	keycode_L;
	uint8_t  	keycode_H;
	uint8_t  	keycode_DriverL;
	uint8_t  	keycode_DriverH;
	uint8_t  	keycode_BackUpL;
	uint8_t  	keycode_BackUpH;
	uint8_t  	buff_index;
	uint8_t  	buff_drive;
}kchkVar;

/* 以256个byte为一组,一个宏256个byte */
typedef struct
{
	//-------------------------------------------------------------------------------------------------------------------
	uint8_t Buffer_Macro[256];
	//-------------------------------------------------------------------------------------------------------------------
}DrivetData_MACRO;

extern	flagField 			u1;
extern 	kchkVar  			kchk;
extern	DrivetData_MACRO 	g_DrivetData_MACRO;
extern	uint8_t  			ReadMacroCount;
extern	uint8_t 			Repeat;
extern	uint8_t  			fKeyDelay;
extern	uint16_t 			KeyDelay;
extern	uint16_t 			FirstByte;
extern	uint16_t 			SecondByte;
extern	uint16_t 			ThirdByte;
extern	uint8_t  			KeyCount;
extern	uint8_t  			FinishKeycodeSend;
extern	uint8_t 			MACROKey_State;
extern	uint32_t 			Time_1ms;
extern  uint8_t 			Macro_Press_again;
extern  uint8_t 			Macro_ID_Check;
/*******************************************************************************
 * API
 ******************************************************************************/
extern void ProcessMacroKey();

#endif /* __MACRO_H__ */

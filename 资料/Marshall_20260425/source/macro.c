/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <usb_device_config.h>
#include <usb.h>
#include <usb_device.h>
#include <usb_device_class.h>
#include <usb_device_hid.h>
#include <usb_device_ch9.h>
#include <usb_device_descriptor.h>
#include <composite.h>
#include <hid_keyboard.h>
#include <hid_mouse.h>
#include <fsl_device_registers.h>
#include <fsl_debug_console.h>
#include <macro.h>
#include <flash_zb25v.h>
#include <hid_keyboard.h>
#include <testapp_communication.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void CheckMouseKey();
void AnalysisFXcode();
void AnalysisFXcode();
/*******************************************************************************
 * Variables
 ******************************************************************************/
flagField 			u1;
kchkVar  			kchk;
DrivetData_MACRO 	g_DrivetData_MACRO;
uint8_t 			ReadMacroCount = 0;
uint8_t 			Repeat = 0;
uint8_t 			fKeyDelay = 0;
uint16_t 			KeyDelay = 0;
uint16_t 			FirstByte = 0;
uint16_t 			SecondByte = 0;
uint16_t 			ThirdByte = 0;
uint8_t 			KeyCount = 0;
uint8_t 			FinishKeycodeSend = 0;
uint8_t	 			MACROKey_State;
uint32_t 			Time_1ms = 0;
uint8_t 			Macro_Press_again = 0;
uint8_t	 			Macro_ID_Check = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/
/******************************************************************************************

******************************************************************************************/
void ProcessMacroKey()
{
	uint16_t i = 0;
	uint16_t j = 0;
	uint32_t Macro_Add = 0;
	if(u1.fIntoMacro && FinishKeycodeSend && fKeyDelay)	//是macro key,送完一個keycode,delaytime也結束,才會進此.
	{
		FinishKeycodeSend=0;
		if(u1.fReadROMdata)
		{
			u1.fReadROMdata = 0;
			ReadMacroCount = 0;
			Macro_Add = FLASHZB25V_Block01_Sector23_MACRO_Addr + (Macro_ID_Check-1)*FLASHZB25V_PageSize;
			FlashZB25V_Read_Data(Macro_Add, (uint8_t *)&g_DrivetData_MACRO, 256);

#if 0
			usb_echo("MACROKey_buffer:\n");
			for(i = 0; i < 256; i++)
			{
				usb_echo("  0x%x",g_DrivetData_MACRO.Buffer_Macro[i]);
			}
			usb_echo("\n");
#endif

			MACROKey_State = g_DrivetData_MACRO.Buffer_Macro[2];							//0x01 循环至按键松开；0x02 循环至按键松开，立即停止;0x03 循环至此键按下;0x04 执行次数
			Repeat = g_DrivetData_MACRO.Buffer_Macro[3];
			KeyCount = 4;
			//usb_echo("Macro_Press_again = %x\n",Macro_Press_again);
		}
		u1.fTime1ms=0;

		FirstByte = g_DrivetData_MACRO.Buffer_Macro[ReadMacroCount*64+KeyCount];			//按键按下或抬起，0是按下0x10是抬起
		KeyCount++;
		SecondByte = g_DrivetData_MACRO.Buffer_Macro[ReadMacroCount*64+KeyCount];		//Keycode
		KeyCount++;
		ThirdByte = g_DrivetData_MACRO.Buffer_Macro[ReadMacroCount*64+KeyCount];			//key持续时间，两个byte，高位在前低位在后
		KeyCount++;
		ThirdByte = ThirdByte<<8;
		ThirdByte &= 0xff00;
		ThirdByte |= g_DrivetData_MACRO.Buffer_Macro[ReadMacroCount*64+KeyCount];
		KeyCount++;

		if(MACROKey_State == 0x02)							//循环至按键松开，立即停止
		{
			if(u1.Macro_Break == 1)
			{
				u1.fIntoMacro=0;
				u1.fStartCount=0;
				u1.fContinueCount=0;
				u1.fKeyDelay=0;
				FinishKeycodeSend = 0;
				fKeyDelay=0;
				KeyCount = 4;
				ReadMacroCount=0;
				Repeat = 0;
				Macro_Press_again = 0;
				MACROKey_State = 0;
				//要发一次抬起
				Clear_buffer(s_KeyboardBuffer1_DKS,64);
				Clear_buffer(s_KeyboardBuffer2_DKS,64);
				Clear_buffer(s_ConsumerBufferIn,8);
				Has_Key_Report_Interface0 = HIDData_Mouse;
				Has_Key_Report_Interface1_DKS = 1;
				Has_Key_Report_Interface2_DKS = 1;
			}
		}

		if(FirstByte==0)
		{
			if(SecondByte==0)
			{
				if(ThirdByte==0)											//连续四个byte为0结束
				{
					if(MACROKey_State == 0x04)								//只有宏的状态是执行次数的时候，才会根据次数停止宏
					{
						if((--Repeat)==0)									//宏结束
						{
							u1.fIntoMacro = 0;;
							u1.fStartCount = 0;
							u1.fContinueCount = 0;
							u1.fKeyDelay=0;
							FinishKeycodeSend = 0;
							fKeyDelay=0;
							KeyCount = 4;
							ReadMacroCount=0;
							Macro_Press_again = 0;
							MACROKey_State = 0;
						}
						else
						{
							u1.fKeyDelay=1;
							FinishKeycodeSend = 1;
							fKeyDelay=1;
							KeyCount = 4;
							ReadMacroCount=0;
						}
					}
					else if(MACROKey_State == 0x01)							//循环至按键松开
					{
						if(u1.Macro_Break)
						{
							if((--Repeat)==0)									//宏结束
							{
								u1.fIntoMacro=0;
								u1.fStartCount=0;
								u1.fContinueCount=0;
								u1.fKeyDelay=0;
								FinishKeycodeSend = 0;
								fKeyDelay=0;
								KeyCount = 4;
								ReadMacroCount=0;
								Macro_Press_again = 0;
								MACROKey_State = 0;
							}
							else
							{
								u1.fKeyDelay=1;
								FinishKeycodeSend = 1;
								fKeyDelay=1;
								KeyCount = 4;
								ReadMacroCount=0;
							}
						}
						else
						{
							u1.fKeyDelay=1;
							FinishKeycodeSend = 1;
							fKeyDelay=1;
							KeyCount = 4;
							ReadMacroCount=0;
						}
					}
					else if(MACROKey_State == 0x02)							//循环至按键松开，立即停止
					{
						u1.fKeyDelay=1;
						FinishKeycodeSend = 1;
						fKeyDelay=1;
						KeyCount = 4;
						ReadMacroCount=0;
					}
					else if(MACROKey_State == 0x03)							//循环至此按键按下
					{
						if(Macro_Press_again == 2)
						{
							u1.fIntoMacro=0;
							u1.fStartCount=0;
							u1.fContinueCount=0;
							u1.fKeyDelay=0;
							FinishKeycodeSend = 0;
							fKeyDelay=0;
							KeyCount = 4;
							ReadMacroCount=0;
							Macro_Press_again = 0;
							MACROKey_State = 0;
							Repeat = 0;
							//要发一次抬起
							Clear_buffer(s_KeyboardBuffer1_DKS,64);
							Clear_buffer(s_KeyboardBuffer2_DKS,64);
							Clear_buffer(s_ConsumerBufferIn,8);
							Has_Key_Report_Interface0 = HIDData_Mouse;
							Has_Key_Report_Interface1_DKS = 1;
							Has_Key_Report_Interface2_DKS = 1;
						}
						else
						{
							{
								u1.fKeyDelay=1;
								FinishKeycodeSend = 1;
								fKeyDelay=1;
								KeyCount = 4;
								ReadMacroCount=0;
							}
						}
					}
				}
				else
				{
					KeyDelay=ThirdByte;										//delay time
					u1.fTime1ms=1;
					u1.fKeyDelay=0;
					fKeyDelay=0;

					if(!KeyDelay)
					{
						u1.fKeyDelay=1;
						fKeyDelay=1;
					}
					FinishKeycodeSend=0;
				}
			}
			else if(u1.fIntoMacro)												//按下
			{
				KeyDelay=ThirdByte;											//delay time
				u1.Macro_KeyPres=1;
				u1.fKeyDelay=0;
				fKeyDelay=0;

				if(!KeyDelay)
				{
					fKeyDelay=1;
				}

				if((SecondByte & 0xf0) == 0xe0)								//shift,ctrl,alt key
				{
					HandleDKSKey_Press(SecondByte, 0, 0);
					//AnalysisEXcode();
					//CheckShiftCtl();
				}
				else if((SecondByte & 0xf0) == 0xf0)						//mouse key
				{
					AnalysisFXcode();
					CheckMouseKey();
					u1.fStartCount=0;
					u1.fContinueCount=0;
				}
				else
				{
					HandleDKSKey_Press(SecondByte, 0, 0);
					//kchk.keycode_L=SecondByte;							//keycode
					//CheckKeyBoard();
				}
				u1.fTime1ms=1;
			}
		}
		else if((FirstByte==0x10) && u1.fIntoMacro)								//第一个byte= 0x10是break
		{
			//kchk.f.key_break=1;
			KeyDelay=ThirdByte;												//delay time
			u1.fKeyDelay=0;													//不在進ProcessMacroKey(),直到delay結束.
			fKeyDelay=0;

			if(!KeyDelay)													//KeyDelay=0,不需delay,fKeyDelay=1.
			{
				fKeyDelay=1;
			}

			if((SecondByte & 0xf0) == 0xe0)									//shift,ctrl,alt key
			{
				HandleDKSKey_Break(SecondByte, 0, 0);
				//AnalysisEXcode();
				//CheckShiftCtl();
			}
			else if((SecondByte & 0xf0) == 0xf0)							//mouse key
			{
				AnalysisFXcode();
				CheckMouseKey();
				u1.fStartCount=0;
				u1.fContinueCount=0;
			}
			else
			{
				HandleDKSKey_Break(SecondByte, 0, 0);
				//kchk.keycode_L=SecondByte;								//keycode
				//CheckKeyBoard();
			}
			u1.fTime1ms=1;
		}
		if(KeyCount >= 64)
		{
			if(ReadMacroCount >= 4)											//读完一个macro
			{
				if(MACROKey_State == 0x04)								//只有宏的状态是执行次数的时候，才会根据次数停止宏
				{
					if((--Repeat)==0)									//宏结束
					{
						u1.fIntoMacro = 0;
						u1.fStartCount = 0;
						u1.fContinueCount = 0;
						u1.fKeyDelay=0;
						FinishKeycodeSend = 0;
						fKeyDelay=0;
						KeyCount = 4;
						ReadMacroCount=0;
						Macro_Press_again = 0;
						MACROKey_State = 0;
					}
					else
					{
						u1.fKeyDelay=1;
						FinishKeycodeSend = 1;
						fKeyDelay=1;
						KeyCount = 4;
						ReadMacroCount=0;
					}
				}
				else if(MACROKey_State == 0x01)							//循环至按键松开
				{
					if(u1.Macro_Break)
					{
						if((--Repeat)==0)									//宏结束
						{
							u1.fIntoMacro=0;
							u1.fStartCount=0;
							u1.fContinueCount=0;
							u1.fKeyDelay=0;
							FinishKeycodeSend = 0;
							fKeyDelay=0;
							KeyCount = 4;
							ReadMacroCount=0;
							Macro_Press_again = 0;
							MACROKey_State = 0;
						}
						else
						{
							u1.fKeyDelay=1;
							FinishKeycodeSend = 1;
							fKeyDelay=1;
							KeyCount = 4;
							ReadMacroCount=0;
						}
					}
					else
					{
						u1.fKeyDelay=1;
						FinishKeycodeSend = 1;
						fKeyDelay=1;
						KeyCount = 4;
						ReadMacroCount=0;
					}
				}
				else if(MACROKey_State == 0x02)							//循环至按键松开，立即停止
				{
					u1.fKeyDelay=1;
					FinishKeycodeSend = 1;
					fKeyDelay=1;
					KeyCount = 4;
					ReadMacroCount=0;
				}
				else if(MACROKey_State == 0x03)							//循环至此按键按下
				{
					if(Macro_Press_again == 2)
					{
						u1.fIntoMacro=0;
						u1.fStartCount=0;
						u1.fContinueCount=0;
						u1.fKeyDelay=0;
						FinishKeycodeSend = 0;
						fKeyDelay=0;
						KeyCount = 4;
						ReadMacroCount=0;
						Macro_Press_again = 0;
						MACROKey_State = 0;
						Repeat = 0;
						//要发一次抬起
						Clear_buffer(s_KeyboardBuffer1_DKS,64);
						Clear_buffer(s_KeyboardBuffer2_DKS,64);
						Clear_buffer(s_ConsumerBufferIn,8);
						Has_Key_Report_Interface0 = HIDData_Mouse;
						Has_Key_Report_Interface1_DKS = 1;
						Has_Key_Report_Interface2_DKS = 1;
					}
					else
					{
						{
							u1.fKeyDelay=1;
							FinishKeycodeSend = 1;
							fKeyDelay=1;
							KeyCount = 4;
							ReadMacroCount=0;
						}
					}
				}
			}
			else
			{
				KeyCount=4;													//每一包的按键内容都是从第四个byte开始
				ReadMacroCount++;
			}
		}
	}
}
/******************************************************************************************
* code=Ex,琌shift,ctrl...单龄
******************************************************************************************/
void AnalysisEXcode()
{
	switch (SecondByte)
	{
		case 0xe0:							//L-ctrl
					kchk.keycode_L=0x01;
					break;
		case 0xe1:							//L-shift
					kchk.keycode_L=0x02;
					break;
		case 0xe2:							//L-alt
					kchk.keycode_L=0x04;
					break;
		case 0xe3:							//R-ctrl
					kchk.keycode_L=0x10;
					break;
		case 0xe4:							//R-shift
					kchk.keycode_L=0x20;
					break;
		case 0xe5:							//R-alt
					kchk.keycode_L=0x40;
					break;
		case 0xe6:							//L-win
					kchk.keycode_L=0x08;
					break;
		case 0xe7:							//R-win
					kchk.keycode_L=0x80;
					break;
	}
}

/******************************************************************************************
* code=Fx,琌mouse key
******************************************************************************************/
void AnalysisFXcode()
{
	switch (SecondByte)
	{
		case 0xf0:
					kchk.keycode_L=0x01;	//mouse left button
					break;
		case 0xf1:
					kchk.keycode_L=0x02;	//mouse right button
					break;
		case 0xf2:
					kchk.keycode_L=0x04;	//mouse middle button
					break;
		case 0xf5:
					kchk.keycode_L=0x08;	//mouse B5 butto
					break;
		case 0xf4:
					kchk.keycode_L=0x10;	//mouse B4 button
					break;
		case 0xf3:
					kchk.keycode_L=0x05;	//Fire key
					break;
		case 0xf6:
					kchk.keycode_L=0x06;	//簎
					break;
		case 0xf7:
					kchk.keycode_L=0x07;	//簎
					break;
	}
}
void CheckMouseKey()
{
	if(u1.Macro_KeyBreak)	//release
	{
		s_ConsumerBufferIn[1] = 0;
		s_ConsumerBufferIn[6] = 0;
	   	//kchk.f.MouseKeyChg = 1;
	   	u1.fStartCount=0;
	   	u1.fContinueCount=0;
	}
	else 										//press
	{
		switch(kchk.keycode_L)
		{
			case 0x01:							//mouse left button
				s_ConsumerBufferIn[1] = 0x01;
			break;

			case 0x02:							//mouse right button
				s_ConsumerBufferIn[1] = 0x02;
			 break;

			case 0x04:							//mouse middle button
				s_ConsumerBufferIn[1] = 0x04;
			break;

			case 0x08:							//mouse B5 button
				s_ConsumerBufferIn[1] = 0x08;
			break;

			case 0x10:							//mouse B4 button
				s_ConsumerBufferIn[1] = 0x10;
			break;

			case 0x06:							//上滾
				s_ConsumerBufferIn[6] = 0x01;
				u1.fStartCount=1;
			break;

			case 0x07:							//下滾
				s_ConsumerBufferIn[6] = 0xff;
				u1.fStartCount=1;
			break;

			default:
				break;
	   	}
	}
}

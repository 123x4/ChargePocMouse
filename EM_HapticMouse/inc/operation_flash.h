#ifndef OPERATION_FLASH_H		
#define OPERATION_FLASH_H		

#include "gloab.h"    
//#include "project.h"
#include "gtech_flash.h"
#include "custom.h"
#include "macro.h"

///Modular code
//#include ".\Led_Layer\light_effect_layer.h"


#define OPERATE_GLOAB_SETTING_SIZE		GLOAB_SETTING_SIZE   //size  8
#define OPERATE_MODE_SETTING_SIZE		MODE_SETTING_SIZE     //size 189 

#define OPERATE_ALL_MODE_SETTING_SIZE	(OPERATE_MODE_SETTING_SIZE * MODE_NUM)	 //size 189*6=1134

#define OPERATE_MACRO_HEAD_SIZE			MACRO_HEAD_SPACE  //size 1600
#define OPERATE_MACRO_DATA_SIZE			MACRO_SPACE    //25600

#define OPERATE_LIGHT_EFFECT_SIZE		0//LIGHT_EFFECT_SPACE

#define OPERATE_ALL_LIGHT_EFFECT_SIZE	0//(LIGHT_EFFECT_SPACE * MODE_NUM)



#define MACRO_HEART_OFFSET		(OPERATE_GLOAB_SETTING_SIZE + OPERATE_ALL_MODE_SETTING_SIZE)//922, size  1134+8=1142
#define MACRO_DATA_OFFSET		(MACRO_HEART_OFFSET + OPERATE_MACRO_HEAD_SIZE)  // size 1142+ 1600 =2742

#define LIGHT_EFFECT_OFFSET		(MACRO_DATA_OFFSET + OPERATE_MACRO_DATA_SIZE)   //size 2742+25600 =28342

#define OPERATE_FLASH_MAX_SPACE	(LIGHT_EFFECT_OFFSET + OPERATE_ALL_LIGHT_EFFECT_SIZE) //flash need space  //size 28342

#define APP_OFFSET  0xFE00

#define Code_Compile_Timer_OFFSET	OPERATE_FLASH_MAX_SPACE



#if 1
void OperateFlash_Read_MacroHeart(UINT32 address,UINT8 *buffer,UINT16 len);
void OperateFlash_Write_MacroHeart(UINT32 address,UINT8 *buffer,UINT16 len);
void OperateFlash_Read_MacroData(UINT32 address,UINT8 *buffer,UINT16 len);
void OperateFlash_Write_MacroData(UINT32 address,UINT8 *buffer,UINT16 len);

void OperateFlash_Read_LightEffect_Layer(UINT32 address,UINT8 *buffer,UINT16 len);
void OperateFlash_Write_LightEffect_Layer(UINT32 address,UINT8 *buffer,UINT16 len);
void OperateFlash_Modedata_Read(UINT8 modeIndex,UINT8* buffer);
#endif


void OperateFlash_Pointer_Init(void);

void OperateFlash_Store_GloabSetting(void);
void OperateFlash_Read_GloabSetting(void);

void OperateFlash_Store_ModeSetting(UINT8 modeIndex);
void OperateFlash_Read_ModeSetting(UINT8 modeIndex);
#if 0
void OperateFlash_Read_MacroHeart(UINT16 address,UINT8 *buffer,UINT16 len);
void OperateFlash_Write_MacroHeart(UINT16 address,UINT8 *buffer,UINT16 len);
void OperateFlash_Read_MacroData(UINT16 address,UINT8 *buffer,UINT16 len);
void OperateFlash_Write_MacroData(UINT16 address,UINT8 *buffer,UINT16 len);

void OperateFlash_Read_LightEffect_Layer(UINT16 address,UINT8 *buffer,UINT16 len);
void OperateFlash_Write_LightEffect_Layer(UINT16 address,UINT8 *buffer,UINT16 len);
#endif
void OperateFlash_SwitchLayer_BaseAddr(UINT8 layerCnt);

void OperateFlash_CodeCompileTimer_Init(void);
void OperateFlash_CodeCompileTimer_Read(UINT8* buffer);

void OperateFlash_CycleSave(void);
void OperateFlash_Set_CycleSave(void);

#endif


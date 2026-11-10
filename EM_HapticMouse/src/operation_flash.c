#include "operation_flash.h"
#include "gtech_flash.h"
#include "custom.h"
#include "pc_communicate.h"

#if 1
UINT32 flash_gloabSetting_p = NULL;
UINT32 flash_modeSetting_p = NULL;


UINT8 *gloabSetting_p = NULL;
UINT8 *modeSetting_p = NULL;

UINT32 macroHeart_Address_p = NULL;
UINT32 macroData_Address_p = NULL;

UINT32 lightEffect_addr = NULL;

UINT32 Code_CompileTimer_Adr = NULL;
UINT32 lightEffect_OffsetAddr;

BOOL OF_shouldSave = FALSE;
extern UINT8 Code_Compile_Timer[];

void OperateFlash_Set_CycleSave(void)
{
	OF_shouldSave = TRUE;
}

void OperateFlash_CycleSave(void)
{
	static UINT16 cycleSave = 0;

//	if (OF_shouldSave)
	{
		OF_shouldSave = FALSE;

		cycleSave = 3000;
	}

	if (cycleSave)cycleSave--;

	if (cycleSave == 1)
	{
		OperateFlash_Store_GloabSetting();
	}
}


void OperateFlash_Pointer_Init(void)
{
	gloabSetting_p = (UINT8*)&gloabSetting;

	modeSetting_p = (UINT8*)&gloabSetting.settingMode;

	///Flash space assign
	flash_gloabSetting_p = inside_Flash;

	flash_modeSetting_p = (inside_Flash + OPERATE_GLOAB_SETTING_SIZE) ; //0x0000A008

	macroHeart_Address_p = (inside_Flash + MACRO_HEART_OFFSET); //0x0000A476

	macroData_Address_p = (inside_Flash + MACRO_DATA_OFFSET);  //0x0000AAB6

	lightEffect_addr = (inside_Flash + LIGHT_EFFECT_OFFSET);   //0x00010EB6

	Code_CompileTimer_Adr = (inside_Flash + LIGHT_EFFECT_OFFSET);  //0x00010EB6
}

void OperateFlash_Store_GloabSetting(void)
{
	GtechFlash_WriteBytes(flash_gloabSetting_p,0,gloabSetting_p,OPERATE_GLOAB_SETTING_SIZE);	
//	SPI_Flash_Write_Byte(gloabSetting_p,flash_gloabSetting_p,OPERATE_GLOAB_SETTING_SIZE);
	
}

void OperateFlash_Read_GloabSetting(void)
{
	GtechFlash_ReadBytes(flash_gloabSetting_p,0,gloabSetting_p,OPERATE_GLOAB_SETTING_SIZE);	
//	SPI_Flash_Read(gloabSetting_p,flash_gloabSetting_p,OPERATE_GLOAB_SETTING_SIZE);
}

void OperateFlash_Store_ModeSetting(UINT8 modeIndex)
{
//	MALLOC_STRU mallocValue = {NULL,FALSE};
	UINT32 writeAddress;
  UINT8 *readBuf;
	UINT8 buffer[2]={0,0};
	
	if(modeIndex >= MODE_NUM)modeIndex = 0;
//	mallocValue = RamMalloc_Assign(OPERATE_MODE_SETTING_SIZE);
	
	readBuf = malloc_ram;
	
	writeAddress = (flash_modeSetting_p + (OPERATE_MODE_SETTING_SIZE * modeIndex));

	if ( (OPERATE_MODE_SETTING_SIZE * modeIndex) + OPERATE_MODE_SETTING_SIZE > OPERATE_ALL_MODE_SETTING_SIZE)
	{
		return;
	}

	GtechFlash_WriteBytes(writeAddress,0,modeSetting_p,OPERATE_MODE_SETTING_SIZE);

	Delayus(3);
//	SPI_Flash_Write_Byte(modeSetting_p,writeAddress,OPERATE_GLOAB_SETTING_SIZE);
	OperateFlash_Modedata_Read(modeIndex,readBuf);
	Delayus(3);

  buffer[0]=CRC_Calculate(readBuf,0,OPERATE_MODE_SETTING_SIZE-1);
	Delayus(3);

	Gtech_MCUFlash_WriteBytes(MCU_inside_Flash,modeIndex,buffer,1);
	
//	RamMalloc_Free(readBuf, OPERATE_MODE_SETTING_SIZE);
}

void OperateFlash_Read_ModeSetting(UINT8 modeIndex)
{
	UINT32 writeAddress;

	if(modeIndex >= MODE_NUM)modeIndex = 0;
	
	writeAddress = (flash_modeSetting_p + (OPERATE_MODE_SETTING_SIZE * modeIndex));

//	SPI_Flash_Read(modeSetting_p,writeAddress,OPERATE_GLOAB_SETTING_SIZE);
	GtechFlash_ReadBytes(writeAddress,0,modeSetting_p,OPERATE_MODE_SETTING_SIZE);	
}




void OperateFlash_Read_MacroHeart(UINT32 address,UINT8 *buffer,UINT16 len)
{
	UINT32 writeAddress;

	writeAddress = (macroHeart_Address_p + address);

//	SPI_Flash_Read(buffer,writeAddress,len);
	GtechFlash_ReadBytes(writeAddress,0,buffer,len);	
}

void OperateFlash_Write_MacroHeart(UINT32 address,UINT8 *buffer,UINT16 len)
{
	UINT32 writeAddress;

	writeAddress = (macroHeart_Address_p + address);

	if (address+len > OPERATE_MACRO_HEAD_SIZE)
	{
		return;
	}
//  SPI_Flash_Read(buffer,writeAddress,len);
	GtechFlash_WriteBytes(writeAddress,0,buffer,len);	
}



void OperateFlash_Read_MacroData(UINT32 address,UINT8 *buffer,UINT16 len)
{
	UINT32 writeAddress;

	writeAddress = (macroData_Address_p + address);

//	SPI_Flash_Read(buffer,writeAddress,len);
	GtechFlash_ReadBytes(writeAddress,0,buffer,len);	
}

void OperateFlash_Write_MacroData(UINT32 address,UINT8 *buffer,UINT16 len)
{
	UINT32 writeAddress;

	writeAddress = (macroData_Address_p + address);

	if (address+len > OPERATE_MACRO_DATA_SIZE)
	{
		return;
	}

//	SPI_Flash_Read(buffer,writeAddress,len);
	GtechFlash_WriteBytes(writeAddress,0,buffer,len);	
}

void OperateFlash_CodeCompileTimer_Init(void)
{
	UINT32 adr = Code_CompileTimer_Adr;

//	SPI_Flash_Write_Byte(Code_Compile_Timer,adr, Code_Compile_Timer_LEN);
	
	GtechFlash_WriteBytes(adr,0,Code_Compile_Timer, Code_Compile_Timer_LEN);//Code_Compile_Timer_LEN);//Code_Compile_Timer_LEN);	
}

void OperateFlash_CodeCompileTimer_Read(UINT8* buffer)
{
	UINT32 adr = Code_CompileTimer_Adr;

//	SPI_Flash_Read(Code_Compile_Timer,adr, Code_Compile_Timer_LEN);
	GtechFlash_ReadBytes(adr,0,buffer,Code_Compile_Timer_LEN);//Code_Compile_Timer_LEN);	
}

void OperateFlash_Modedata_Read(UINT8 modeIndex,UINT8* buffer)
{
	UINT32 writeAddress;

	writeAddress = (flash_modeSetting_p + (OPERATE_MODE_SETTING_SIZE * modeIndex));
//  UINT32 adr = Code_CompileTimer_Adr;
	
	GtechFlash_ReadBytes(writeAddress,0,buffer,OPERATE_MODE_SETTING_SIZE);//Code_Compile_Timer_LEN);	
}
#endif

#if 0
const  UINT8 *flash_gloabSetting_p = NULL;
const  UINT8 *flash_modeSetting_p = NULL;


UINT8 *gloabSetting_p = NULL;
UINT8 *modeSetting_p = NULL;

UINT8 *macroHeart_Address_p = NULL;
UINT8 *macroData_Address_p = NULL;

UINT8 *lightEffect_addr = NULL;

UINT8 *Code_CompileTimer_Adr = NULL;

UINT32 lightEffect_OffsetAddr;

BOOL OF_shouldSave = FALSE;
extern UINT8 Code_Compile_Timer[];

void OperateFlash_Set_CycleSave(void)
{
	OF_shouldSave = TRUE;
}

void OperateFlash_CycleSave(void)
{
	static UINT16 cycleSave = 0;

	if (OF_shouldSave)
	{
		OF_shouldSave = FALSE;

		cycleSave = 5;
	}

	if (cycleSave)cycleSave--;

	if (cycleSave == 1)
	{
		OperateFlash_Store_GloabSetting();
	}
}



void OperateFlash_Pointer_Init(void)
{
	gloabSetting_p = (UINT8*)&gloabSetting;

	modeSetting_p = (UINT8*)&gloabSetting.settingMode;

	///Flash space assign
	flash_gloabSetting_p = inside_Flash;

	flash_modeSetting_p = (UINT8*)(inside_Flash + OPERATE_GLOAB_SETTING_SIZE) ; //0x0000A008

	macroHeart_Address_p = (UINT8*)(inside_Flash + MACRO_HEART_OFFSET); //0x0000A476

	macroData_Address_p = (UINT8*)(inside_Flash + MACRO_DATA_OFFSET);  //0x0000AAB6

	lightEffect_addr = (UINT8*)(inside_Flash + LIGHT_EFFECT_OFFSET);   //0x00010EB6

	Code_CompileTimer_Adr = (UINT8*)(inside_Flash + OPERATE_FLASH_MAX_SPACE);  //0x00010EB6
	
//	APP_Adr = (UINT8*)(inside_Flash+ APP_OFFSET)
}

void OperateFlash_Store_GloabSetting(void)
{
	GtechFlash_WriteBytes(flash_gloabSetting_p,0,gloabSetting_p,OPERATE_GLOAB_SETTING_SIZE);	
}

void OperateFlash_Read_GloabSetting(void)
{
	GtechFlash_ReadBytes(flash_gloabSetting_p,0,gloabSetting_p,OPERATE_GLOAB_SETTING_SIZE);	
}

void OperateFlash_Store_ModeSetting(UINT8 modeIndex)
{
	UINT8 *writeAddress;
//  UINT32 writeAddress;
	
	writeAddress = (UINT8*)(flash_modeSetting_p + (OPERATE_MODE_SETTING_SIZE * modeIndex));

	if ( (OPERATE_MODE_SETTING_SIZE * modeIndex) + OPERATE_MODE_SETTING_SIZE > OPERATE_ALL_MODE_SETTING_SIZE)
	{
		return;
	}

	GtechFlash_WriteBytes(writeAddress,0,modeSetting_p, OPERATE_MODE_SETTING_SIZE);	//OPERATE_MODE_SETTING_SIZE
}

void OperateFlash_Read_ModeSetting(UINT8 modeIndex)
{
	UINT8 *writeAddress;
//  UINT32 writeAddress;
	
	writeAddress = (UINT8*)(flash_modeSetting_p + (OPERATE_MODE_SETTING_SIZE * modeIndex));

	GtechFlash_ReadBytes(writeAddress,0,modeSetting_p,OPERATE_MODE_SETTING_SIZE);	
}




void OperateFlash_Read_MacroHeart(UINT16 address,UINT8 *buffer,UINT16 len)
{
	UINT8 *writeAddress;
//  UINT32 writeAddress;
	
	writeAddress = (UINT8*)(macroHeart_Address_p + address);

	GtechFlash_ReadBytes(writeAddress,0,buffer,len);	
}

void OperateFlash_Write_MacroHeart(UINT16 address,UINT8 *buffer,UINT16 len)
{
	UINT8 *writeAddress;
//  UINT32 writeAddress;
	
	writeAddress = (UINT8*)(macroHeart_Address_p + address);

	if (address+len > OPERATE_MACRO_HEAD_SIZE)
	{
		return;
	}

	GtechFlash_WriteBytes(writeAddress,0,buffer,len);	
}



void OperateFlash_Read_MacroData(UINT16 address,UINT8 *buffer,UINT16 len)
{
	UINT8 *writeAddress;
//  UINT32 writeAddress;
	
	writeAddress = (UINT8*)(macroData_Address_p + address);

	GtechFlash_ReadBytes(writeAddress,0,buffer,len);	
}

void OperateFlash_Write_MacroData(UINT16 address,UINT8 *buffer,UINT16 len)
{
	UINT8 *writeAddress;
//  UINT32 writeAddress;
	
	writeAddress = (UINT8*)(macroData_Address_p + address);

	if (address+len > OPERATE_MACRO_DATA_SIZE)
	{
		return;
	}

	GtechFlash_WriteBytes(writeAddress,0,buffer,len);	
}



void OperateFlash_SwitchLayer_BaseAddr(UINT8 layerCnt)
{
	///Because it adds five lights in the back, so it's compatible
	lightEffect_OffsetAddr = layerCnt * OPERATE_LIGHT_EFFECT_SIZE;
}

void OperateFlash_Read_LightEffect_Layer(UINT16 address,UINT8 *buffer,UINT16 len)
{
	UINT8 *writeAddress;
  
	writeAddress = (UINT8 *)(lightEffect_addr +lightEffect_OffsetAddr + address);

	GtechFlash_ReadBytes(writeAddress,0,buffer,len);	
}

void OperateFlash_Write_LightEffect_Layer(UINT16 address,UINT8 *buffer,UINT16 len)
{
	UINT8 *writeAddress;

	writeAddress = (UINT8 *)(lightEffect_addr + lightEffect_OffsetAddr+ address);

	GtechFlash_WriteBytes(writeAddress,0,buffer,len);	
}


void OperateFlash_CodeCompileTimer_Init(void)
{
	UINT8 *adr = (UINT8*)Code_CompileTimer_Adr;
//  UINT32 adr = Code_CompileTimer_Adr;
	
	GtechFlash_WriteBytes(adr,0,Code_Compile_Timer, Code_Compile_Timer_LEN);//Code_Compile_Timer_LEN);//Code_Compile_Timer_LEN);	
}

void OperateFlash_CodeCompileTimer_Read(UINT8* buffer)
{
	UINT8 *adr = (UINT8*)Code_CompileTimer_Adr;
//  UINT32 adr = Code_CompileTimer_Adr;
	
	GtechFlash_ReadBytes(adr,0,buffer,Code_Compile_Timer_LEN);//Code_Compile_Timer_LEN);	
}

void OperateFlash_Modedata_Read(UINT8 modeIndex,UINT8* buffer)
{
	UINT8 *writeAddress;

	writeAddress = (UINT8*)(flash_modeSetting_p + (OPERATE_MODE_SETTING_SIZE * modeIndex));
//  UINT32 adr = Code_CompileTimer_Adr;
	
	GtechFlash_ReadBytes(writeAddress,0,buffer,OPERATE_MODE_SETTING_SIZE);//Code_Compile_Timer_LEN);	
}
#endif


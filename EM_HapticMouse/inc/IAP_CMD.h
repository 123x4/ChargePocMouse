#ifndef __IAP_CMD_H__
#define __IAP_CMD_H__


#include "stdint.h"
#include "OSC.h"



#define USE_INSIDE_EEPROM		1

//#define LOGO_LED_EFFECT			0	//don't enable; remain disable
//#define DPI_LED_EFFECT			0 //don't enable; remain disable


uint8_t Chip_EEPROM_Write(uint32_t dstAdd, uint8_t *ptr, uint32_t byteswrt);
uint8_t Chip_EEPROM_Read(uint32_t srcAdd, uint8_t *ptr, uint32_t bytesrd);
uint32_t Chip_IAP_ReadUID(uint32_t *uid);

#endif


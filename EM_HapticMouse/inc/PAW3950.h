/*******************************************************************************
 * @file    PAW3950.h
 * @brief
 * @author  chengjie
 * @date    created on 2026-05-08
 * @version V0.1
 * @note
*******************************************************************************/

#ifndef __PAW3950_h__
#define __PAW3950_h__

#include "stdint.h"
#include "LPC11Uxx.h"
#include "GPIO.h"
#include "my_tick.h"
#include "my_debug.h"
#include "SPI.h"
#include "Timer.h"
#include "my_board.h"

#if (defined(IRPT_V4) || defined(V4_HALL_HPTIC_V4) || defined(GT_LINA_T1A_H5))

int32_t PAW3950_Init(void);
void PAW3950_Exit(void);
int32_t PAW3950_Write(uint8_t regAddr, uint8_t data);
int32_t PAW3950_Read(uint8_t regAddr, uint8_t *pData, uint16_t length);
int32_t PAW3950_SetCPI(uint16_t cpi);
int32_t PAW3950_GetMotionData(int16_t *pX, int16_t *pY);

#endif


#endif /* __PAW3950_h__ */


/*******************************************************************************
 * @file    my_hallkey_func.h
 * @brief
 * @author  chengjie
 * @date    created on 2026-04-14
 * @version V0.1
 * @note
*******************************************************************************/

#ifndef __my_hallkey_func_h__
#define __my_hallkey_func_h__

#include "stdint.h"
#include "button.h"
#include "my_board.h"

#define ANA_TOTAL_COUNT   2
//#define MAGNETIC_POLARITY_N   1 //If pressed, the sampling value will increase.
//#define MAGNETIC_POLARITY_S   !(MAGNETIC_POLARITY_N)

#if (defined(IRPT_V4))
#define MAGNETIC_NOISE        20
#else
#define MAGNETIC_NOISE        60
#endif
#define MAGNETIC_SCAN_RATE    1 // 1 represents 1K, and 8 represents 8K.
#define ONE_MS_COUNT          (MAGNETIC_SCAN_RATE)


//void key_rapid_trigger_handle(unsigned short segment, unsigned char key_index);
//void ProcessRecvSampleRawData(uint8_t* pData, uint16_t length);


#endif /* __my_hallkey_func_h__ */


/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef GLOAB_H		
#define GLOAB_H

#include "usb.h"
//#include "project.h"
#include <stdlib.h>


#pragma pack(1)

    
typedef unsigned char   UINT8;
//typedef unsigned char   BOOL;
typedef unsigned short  UINT16;
typedef unsigned long   UINT32;

typedef signed char     INT8;
typedef short int       INT16;
typedef long  int       INT32;
typedef     float  FLOAT32;

#define BOOL _Bool
//#define TRUE 1
//#define FALSE 0
    
//#define   TimerDelayMsec(x)    CyDelay(x)
//#define   TimerDelayUsec(x)    CyDelayUs(x)

typedef struct 
{
	UINT8 *mouse_report;
	UINT8 *keyboard_report;
	UINT8 *multimedia_report;
}REPORT_POINTER;

enum
{
	PHASE0	= 0,
	PHASE1,	
	PHASE2,
	PHASE3,	
	PHASE4,	
};
extern void TimerDelayMsec(uint32_t t_count) ;  
#endif /* #ifndef GLOAB_H */


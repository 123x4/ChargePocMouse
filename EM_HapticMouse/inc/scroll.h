#ifndef _SCROLL_H_
#define _SCROLL_H_

#include "stdint.h"
#include "../inc/GPIO.h"
#include "../inc/device.h"

#define WHEEL_UP_REPORT_BIT		(1<<3)
#define WHEEL_DOWN_REPORT_BIT	(1<<4)

typedef struct
{
	INT8  iDelta;
	UINT8 bLastState[2];
	INT8  cz1;
	INT8  cz2;
	UINT8 bLastPollState;
	UINT8 bPollCounter;
	UINT32 wheel_status;
}WHEEL_SCAN_DATA;

extern WHEEL_SCAN_DATA wheelScan_stru;

BOOL Scroll_Scan(void);
void Main_Mouse_SCROLL_UP(void);
void Main_Mouse_SCROLL_DN(void);
//void nsAPI_WheelEventMointor(void);
#endif

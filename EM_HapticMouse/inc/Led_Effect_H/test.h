#ifndef _TEST_H_
#define _TEST_H_


#include "gloab.h"

typedef union {
	UINT32   data; 
	unsigned char buffer[4];
}TEST_t;



void Tool_USB_32bitsData(UINT32 data);
void Tool_Uart_32bitsData(UINT32 data);
void Tool_USB_ArrayData(UINT8 *data,UINT8 length);

void Tool_Clear_ArrayData(UINT8 *buffer,UINT8 first_subscript,UINT8 end_subscript);
void Tool_Assigned_BufferToBuffer(UINT8 *from_buffer,UINT8 *to_buffer,UINT8 length);

void Tool_Array_crol(UINT8 *array,UINT8 length,UINT8 offSet);
void Tool_Array_cror(UINT8 *array,UINT8 length,UINT8 offSet);

BOOL Tool_Check_If_Had_NonZero(UINT8 *buffer,UINT8 firstIndex,UINT8 endIndex);

UINT8 Tool_GetArray_MaxValue(UINT8 *dat,UINT8 len);
BOOL Check_Buffer_If_String(UINT8 *bufOne,UINT8 *bufTwo,UINT8 len);

void Main_Test(void);
UINT8 Tool_Rand(UINT8 minVal,UINT8 maxVal);

void Tool_USB_Test_ArrayData(UINT8 *data,UINT8 length);
void Tool_Uart_Send_Byte(UINT8 dat);

void Test(void);

#define LEFT_MOVE 0
#define RIGHT_MOVE 1

#endif 


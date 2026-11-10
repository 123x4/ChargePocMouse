#include "test.h"
#include "macro.h"
#include "stdlib.h"
//#include ".\Led_Layer\light_effect_breathe.h"
//#include "operation_flash.h"

void Tool_USB_32bitsData(UINT32 data)
{
	UINT8 ii = 0;
	UINT8 temp[64];

	for (ii = 0;ii < 64;ii++)
	{
		temp[ii] = 0;
	}

	temp[0] = (UINT8)(data>>24);
	temp[1] = (UINT8)(data>>16);
	temp[2] = (UINT8)(data>>8);
	temp[3] = (UINT8)data;

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, temp, 4);
}

void Tool_USB_ArrayData(UINT8 *data,UINT8 length)
{
	UINT8 ii = 0;
	UINT8 temp[64];
	UINT8 writeCount;
	UINT8 writeIndex = 0;
	UINT8 remain = length;

	while (writeIndex < length)
	{
		for (ii = 0;ii < 64;ii++)
		{
			temp[ii] = 0;
		}

		writeCount = remain >= 64 ? 64 : remain;

		for (ii = 0;ii < writeCount;ii++)
		{
			temp[ii] = data[writeIndex+ii];
		}
		
		writeIndex += writeCount;
		remain -= writeCount;
//    nsAPI_USB_WriteEP(HID_EP3_IN , temp, writeCount);
		USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, temp, writeCount);
		//TimerDelayMsec(2);
	}
}

void Tool_USB_Test_ArrayData(UINT8 *data,UINT8 length)
{
	UINT8 ii = 0;
	UINT8 temp[64];
	UINT8 writeCount;
	UINT8 writeIndex = 0;
	UINT8 remain = length;

	while (writeIndex < length)
	{
		for (ii = 0;ii < 64;ii++)
		{
			temp[ii] = 0;
		}

		writeCount = remain >= 64 ? 64 : remain;

		for (ii = 0;ii < writeCount;ii++)
		{
			temp[ii] = data[writeIndex+ii];
		}

		writeIndex += writeCount;
		remain -= writeCount;

		USB_LoadInEP_Data(EP_3_CUSTION_OUT, temp, writeCount);
//		DrvDelay1mS(2);
		TimerDelayMsec(2);
	}
}

#if 0
void Tool_Uart_32bitsData(UINT32 data)
{
	UINT8 ii = 0;
	UINT8 temp[64];

	for (ii = 0;ii < 64;ii++)
	{
		temp[ii] = 0;
	}

	temp[0] = (UINT8)(data>>24);
//	SW_Tx_UART_PutHexByte(temp[0]);

	temp[1] = (UINT8)(data>>16);
//	SW_Tx_UART_PutHexByte(temp[1]);

	temp[2] = (UINT8)(data>>8);
//	SW_Tx_UART_PutHexByte(temp[2]);

	temp[3] = (UINT8)data;
//	SW_Tx_UART_PutHexByte(temp[3]);

//	SW_Tx_UART_PutString("\r\n"); 
	
}
#endif

void Tool_Uart_Send_Byte(UINT8 dat)
{
//	SW_Tx_UART_PutChar(dat);
}

void Tool_Clear_ArrayData(UINT8 *buffer,UINT8 first_subscript,UINT8 end_subscript)
{
	UINT8 ii = 0;
	for (ii = first_subscript; ii < (end_subscript+1); ii++)
	{
		buffer[ii] = 0;
	}

}

void Tool_Assigned_BufferToBuffer(UINT8 *from_buffer,UINT8 *to_buffer,UINT8 length)
{
	UINT8 ii = 0;
	for (ii = 0;ii < length;ii++)
	{
		to_buffer[ii] = from_buffer[ii];
	}
}

void Tool_Array_crol(UINT8 *array,UINT8 length,UINT8 offSet)
{  
	UINT8 ii,tem,kk;

	UINT8 arrayNum = length;


	for (ii = 0;ii < offSet;ii++)
	{
		tem = array[0];
		for (kk = 0;kk < (arrayNum-1);kk++)
		{
			array[kk] = array[kk+1];
		}
		array[(arrayNum-1)] = tem;
	}

	
}  


void Tool_Array_cror(UINT8 *array,UINT8 length,UINT8 offSet)
{  
	UINT8 ii,tem,kk;
	UINT8 arrayNum = length;

	for (ii = 0;ii < offSet;ii++)
	{
		tem = array[(arrayNum-1)] ;
		for (kk = 0;kk < (arrayNum-1);kk++)
		{
			array[arrayNum-1-kk] = array[arrayNum-2-kk];
		}
		array[0] = tem;
	}

	
}  

///53550
BOOL Tool_Check_If_Had_NonZero(UINT8 *buffer,UINT8 firstIndex,UINT8 endIndex)
{
	UINT8 ii;
	BOOL result = FALSE;

	for (ii = firstIndex;ii < (endIndex + 1);ii++)
	{
		if (buffer[ii] != 0)
		{
			buffer[ii] = 0;
			result = TRUE;
		}
	}

	return result;
}


UINT8 Tool_GetArray_MaxValue(UINT8 *dat,UINT8 len)
{
	UINT8 ii;

	for (ii = 1;ii < len ;ii++)
	{
		if (dat[0] < dat[ii])
		{
			dat[0] = dat[ii];
		}
	}

	return dat[0];
}



BOOL Check_Buffer_If_String(UINT8 *bufOne,UINT8 *bufTwo,UINT8 len)
{
	BOOL ifSame = TRUE;
	UINT8 ii;

	for (ii = 0;ii < len;ii++)
	{
		if (bufOne[ii] != bufTwo[ii])
		{
			ifSame = FALSE;

			break;
		}
	}

	return ifSame;
}


UINT8 Tool_Rand(UINT8 minVal,UINT8 maxVal)
{
	return (rand()%(maxVal-minVal+1)+minVal);
}

void Test(void)
{
//	UINT8 a[50];
	///TEST!!!!!!!!!!!!!!!
//	OperateFlash_Read_LightEffect_Layer(0,(UINT8*)&a,50);
}


void Main_Test(void)
{
	UINT8 k = 0;

	k = Tool_Rand(1,3);

	Tool_USB_32bitsData((UINT32)k);
}









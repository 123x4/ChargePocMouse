//**********************************
//
//this file for IS31FL3731 use
//
//according calculate I2C Transmission rate ,
//I2C send one byte use 20 us
//
//**********************************

#ifndef __LED1_C__
#define __LED1_C__

#include "led1.h"
//#include ".\Led_Layer\light_effect_layer.h"
#include "custom.h"
#include "../inc/LedControl.h"

UINT8 led_sendType;

UINT8 led_pwmSend_index;
UINT8 led_stateSend_index;
UINT8 led_blinkSend_index;

LED_RGB_STRU led_stru[LED_NUMBER];
//LED_RGB_STRU led_dealStru[LED_NUMBER];

UINT8 led_pwmRegData[LED_PWM_ALL_DATALINE];
UINT8 led_blinkRegData[LED_BLINK_ALL_DATALINE];
UINT8 led_stateRegData[LED_STATE_ALL_DATALINE];


#if 0
const LED_RGB_REG_STRU RGB_regLocate[LED_NUMBER] = 
{ 
	///0XFF is not use led
	{{0XFF,0},{0XFF,1},{0XFF,1}},//LED1				
	{{0xFF,1},{0XFF,1},{0XFF,2}},//LED2  not exit
	{{1,2},{0xFF,2},{0xFF,2}},//LED3// DPI_front_1 
	{{1,3},{0xFF,3},{0xFF,3}},//LED4// DPI_front_2 
	{{1,4},{0xFF,4},{0xFF,4}},//LED5// DPI_front_3   
	{{1,5},{3,5},{5,5}},//LED6 
	{{1,7},{3,7},{5,7}},//LED7				
	{{1,6},{3,6},{5,6}},//LED8	
	{{17,0},{15,0},{13,0}},//LED9 
	{{17,1},{15,1},{13,1}},//LED10 
	{{17,2},{15,2},{13,2}},//LED11 
	{{17,3},{15,3},{13,3}},//LED12 
	{{17,4},{15,4},{13,4}},//LED13 
	{{17,5},{15,5},{13,5}},//LED14 
	{{17,6},{15,6},{11,5}},//LED15 
	{{17,7},{13,6},{11,6}},//LED16	
	{{0,0},{4,1},{6,1}},//LED17	
	//{{0xff,1},{0xff,1},{0xff,2}},//LED18  
	{{0,1},{2,1},{6,2}},//LED18  
	{{0,2},{2,2},{4,2}},//LED19 
	{{0,3},{2,3},{4,3}},//LED20 
	{{0,4},{2,4},{4,4}},//LED21 
	{{0,5},{2,5},{4,5}},//LED22	
	{{0,6},{2,6},{4,6}},//LED23  
	{{0,7},{2,7},{4,7}},//LED24  
	{{16,0},{14,0},{12,0}},//LED25
	{{16,1},{14,1},{12,1}},//LED26  
	{{16,2},{14,2},{12,2}},//LED27	
	{{16,3},{14,3},{12,3}},//LED28  
	{{16,4},{14,4},{12,4}},//LED29  
	{{16,5},{14,5},{12,5}},//LED30	
	{{16,6},{14,6},{10,5}},//LED31	  
	{{16,7},{12,6},{10,6}},//LED32	  
};


///The array is convenient data written to the PWM register
const LED_PWM_DATA_STRU pwm_data_stru[LED_LINE_NUM] = 
{
	///address£»
	///how many reg should change
	///first change index
	{0X24,8,0},//0
	{0X2C,7,1},//1
	{0X34,7,1},//2
	{0X3C,7,1},//3
	{0X44,7,1},//4
	{0X4C,3,5},//5
	{0X54,2,1},//6
	{0X5C,1,2},//7
	{0X64,0,0},//8
	{0X6C,0,0},//9
	{0X74,2,5},//10
	{0X7C,2,5},//11
	{0X84,7,0},//12
	{0X8C,7,0},//13
	{0X94,7,0},//14
	{0X9C,7,0},//15  
	{0XA4,8,0},//16
	{0XAC,8,0},//17
};
const LED_PWM_DATA_STRU pwm_data_stru[LED_LINE_NUM] = 
{
	///address£»
	///how many reg should change
	///first change index
	{0X24,8,0},//0
	{0X2C,8,0},//1
	{0X34,8,0},//2
	{0X3C,8,0},//3
	{0X44,8,0},//4
	{0X4C,8,0},//5
	{0X54,8,0},//6
	{0X5C,8,0},//7
	{0X64,8,0},//8
	{0X6C,8,0},//9
	{0X74,8,0},//10
	{0X7C,8,0},//11
	{0X84,8,0},//12
	{0X8C,8,0},//13
	{0X94,8,0},//14
	{0X9C,8,0},//15  
	{0XA4,8,0},//16
	{0XAC,8,0},//17
};


#endif
//	{{0,0},{4,1},{6,1}},//LED1 \
{4,1} 4 represents the address of the control register of the LED cathode in the matrix, \
and 1 represents the number of columns of the LED in the matrix.
#if 1
const LED_RGB_REG_STRU RGB_regLocate[LED_NUMBER] = 
{ 
	///0XFF is not use led
	{{0,0},{4,1},{6,1}},//LED1		profile	
	{{0,1},{2,1},{6,2}},//LED2  not exit
	{{0,2},{2,2},{4,2}},//LED3
	{{1,1},{3,1},{7,2}},//LED4  R G B
	{{0,3},{2,3},{4,3}},//LED5
	{{0,4},{2,4},{4,4}},//LED6
	{{0,5},{2,5},{4,5}},//LED7 
	{{0,6},{2,6},{4,6}},//LED8				
	{{0,7},{2,7},{4,7}},//LED9	
	
	{{16,0},{14,0},{12,0}},//LED10   
	{{16,1},{14,1},{12,1}},//LED11  
	{{16,2},{14,2},{12,2}},//LED12 
	{{16,3},{14,3},{12,3}},//LED13 
	{{16,4},{14,4},{12,4}},//LED14 
	{{16,5},{14,5},{12,5}},//LED15 
	{{16,6},{14,6},{10,5}},//LED16 
	{{16,7},{12,6},{10,6}},//LED17	
	
	{{1,0},{5,1},{7,1}},//LED18  
	{{1,4},{3,4},{5,4}},//LED19	
	{{1,2},{3,2},{5,2}},//LED20  
	{{1,3},{3,3},{5,3}},//LED21  

//	{{17,0},{15,0},{13,0}},//LED9   R(CB9, C9.9) G(CB8, C8.9) B(CB7, C7.9) 0x11h,0 
//	{{17,1},{15,1},{13,1}},//LED10  R(CB9, C9.10)

};

//const LED_PWM_DATA_STRU pwm_data_stru[LED_LINE_NUM] = 
//{
//	///address£»
//	///how many reg should change
//	///first change index
//	{0X24,8,0},//0
//	{0X2C,8,0},//1
//	{0X34,8,0},//2
//	{0X3C,8,0},//3
//	{0X44,8,0},//4
//	{0X4C,8,0},//5
//	{0X54,8,0},//6
//	{0X5C,8,0},//7
//	{0X64,8,0},//8
//	{0X6C,8,0},//9
//	{0X74,8,0},//10
//	{0X7C,8,0},//11
//	{0X84,8,0},//12
//	{0X8C,8,0},//13
//	{0X94,8,0},//14
//	{0X9C,8,0},//15  
//	{0XA4,8,0},//16
//	{0XAC,8,0},//17
//};

/////The array is convenient data written to the PWM register
const LED_PWM_DATA_STRU pwm_data_stru[LED_LINE_NUM] = 
{
	///address£»
	///how many reg should change
	///first change index
	{0X24,8,0},//0
	{0X2C,5,0},//1
	{0X34,7,1},//2
	{0X3C,4,1},//3
	{0X44,7,1},//4
	{0X4C,4,1},//5
	{0X54,2,1},//6
	{0X5C,2,1},//7
	{0X64,0,0},//8
	{0X6C,0,0},//9
	{0X74,2,5},//10
	{0X7C,0,0},//11
	{0X84,7,0},//12
	{0X8C,0,0},//13
	{0X94,7,0},//14
	{0X9C,0,0},//15  
	{0XA4,8,0},//16
	{0XAC,0,0},//17
};
#endif

void Led_RegData_SearchLocation(UINT16 offSet_index,UINT8 data,UINT8 *reg_data,UINT8 data_type)
{
#define LED_INVALUE_GROUP	0XFF
	UINT8 regGroup_cnt = 0,regMask = 0;
	LED_REG_STRU *locate;
	
	locate =  (LED_REG_STRU *)((LED_REG_STRU *)RGB_regLocate + offSet_index);

	regGroup_cnt = locate->regGroupCnt;
	regMask = locate->bitLoation;
	
	if (regGroup_cnt == LED_INVALUE_GROUP)
	{
		return;
	}

	if (data_type != LED_MODIFY_PWM)
	{
		if (data)
		{
			reg_data[regGroup_cnt] |= (0x01 << regMask);
		}	

	}else if (data_type == LED_MODIFY_PWM)
	{
		reg_data[regGroup_cnt*8+regMask] = data;
	}
}
//
//void I2C_SparateSend_StateReg(UINT8 sendType,UINT8 *regData,UINT8 *sendIndex)
//{
//	#define LED_SPERATE_WRITE_TEM_CNT 15
//	#define LED_BIT_SINGLE_SEND_CNT 6
//
//	UINT8 base_regAdder = 0;
//	UINT8 ii,offSet;
//	UINT8 dataBuffer[LED_SPERATE_WRITE_TEM_CNT];
//
//
//	Tool_Clear_ArrayData(dataBuffer,0,(LED_SPERATE_WRITE_TEM_CNT - 1));
//
//	switch(sendType)
//	{
//	case LED_MODIFY_STATE: base_regAdder = LED_CONTROL_REG_ADDER;break;
//	case LED_MODIFY_BLINK: base_regAdder = LED_BLINKCONTROL_REG_ADDER;break;
//	default:break;
//	}
//	
//	dataBuffer[0] = base_regAdder+(*sendIndex) ;
//	offSet = (*sendIndex) ;
//
//	for (ii = 0;ii < LED_BIT_SINGLE_SEND_CNT;ii++)
//	{
//		dataBuffer[1+ii] = regData[offSet+ii];
//	}
//
//	(*sendIndex) += LED_BIT_SINGLE_SEND_CNT;
//
//	LedControl_WriteArray(dataBuffer,(LED_BIT_SINGLE_SEND_CNT+1));
//
//	if ((*sendIndex) >= LED_BLINK_ALL_DATALINE)
//	{
//		led_sendType &= ~sendType;
//		(*sendIndex) = 0;
//	}		
//}

void Led_I2C_SeparateSend_StructData(void)
{
	UINT8 adder = 0,writeCnt = 0,firstCnt = 0,ii;
	UINT8 dataBuffer[LED_SPERATE_WRITE_TEM_CNT];

	if (led_sendType & LED_MODIFY_STATE)
	{
		//I2C_SparateSend_StateReg(LED_MODIFY_STATE,led_stateRegData,&led_stateSend_index);

	}else if (led_sendType & LED_MODIFY_BLINK)
	{
		//I2C_SparateSend_StateReg(LED_MODIFY_BLINK,led_blinkRegData,&led_blinkSend_index);

	}else if (led_sendType & LED_MODIFY_PWM)
	{
		for (ii = led_pwmSend_index;ii < LED_LINE_NUM;ii++)
		{
			adder = pwm_data_stru[ii].firstAdder;
			writeCnt = pwm_data_stru[ii].count;
			firstCnt = pwm_data_stru[ii].mask;

			if (writeCnt)
			{
				led_pwmSend_index = ii;
				break;
			}
		}

		dataBuffer[0] = adder + firstCnt;

		for (ii = 0;ii < writeCnt;ii++)
		{
			dataBuffer[ii+1] = led_pwmRegData[led_pwmSend_index*8+firstCnt+ii];
		}

		if (writeCnt)
		{
			LedControl_WriteArray(LED_DRIVER_SLAVEADDR,dataBuffer,(writeCnt+1));	
			//Tool_USB_ArrayData(dataBuffer,(writeCnt+1));
		}

		led_pwmSend_index++;


		if (led_pwmSend_index >= LED_LINE_NUM)
		{
			led_sendType &= ~LED_MODIFY_PWM;
		}
	}
}


void Led_I2C_SendAllStructData(UINT8 dataType,UINT8 *regData)
{
	#define LED_WRITE_TEM_CNT 60

	UINT8 ii,kk,dataLine_cnt;
	UINT8 dataBuffer[LED_WRITE_TEM_CNT];
	UINT8 adder, writeCnt,firstCnt;

	for (ii = 0;ii < LED_WRITE_TEM_CNT;ii++)
	{
		dataBuffer[ii] = 0;
	}

	if (dataType != LED_MODIFY_PWM)
	{
		if (dataType == LED_MODIFY_STATE)
		{
			dataBuffer[0] = LED_CONTROL_REG_ADDER;

		}else if (dataType == LED_MODIFY_BLINK)
		{
			dataBuffer[0] = LED_BLINKCONTROL_REG_ADDER;
		}

		for (ii = 0;ii < LED_BLINK_ALL_DATALINE;ii++)
		{
			dataBuffer[ii+1] = regData[ii];
		}

		LedControl_WriteArray(LED_DRIVER_SLAVEADDR,dataBuffer,(LED_BLINK_ALL_DATALINE+1));

	}else if (dataType == LED_MODIFY_PWM)
	{
		for (dataLine_cnt = 0;dataLine_cnt < LED_LINE_NUM ;dataLine_cnt++)
		{
			adder = pwm_data_stru[dataLine_cnt].firstAdder;
			writeCnt = pwm_data_stru[dataLine_cnt].count;
			firstCnt = pwm_data_stru[dataLine_cnt].mask;

			if (!writeCnt)
			{
				continue;
			}

			dataBuffer[0] = adder + firstCnt;

			for (kk = 0;kk < writeCnt;kk++)
			{
				dataBuffer[kk+1] = regData[dataLine_cnt*8+firstCnt+kk];
			}

			LedControl_WriteArray(LED_DRIVER_SLAVEADDR,dataBuffer,(writeCnt+1));		

		}
	}
}


#if 0
void Led_Structure_And_Electronic_Require2(UINT8 ledCnt,UINT8 color,UINT8* data)   //adjust led brightness
{
	UINT16 operateData;


	///LED7
	if (ledCnt == 6)
	{
		if (color == 0)
		{
			operateData = (UINT16)(*data) * 45 / 100;	
			(*data) = (UINT8)operateData;
		}

		if (color == 2)
		{
			operateData = (UINT16)(*data) * 75 / 100;	
			(*data) = (UINT8)operateData;
		}
	}



	///LED17
	if (ledCnt == 16)
	{
		if (color == 0)
		{
			operateData = (UINT16)(*data) * 35 / 100;	
			(*data) = (UINT8)operateData;
		}

		if (color == 1)
		{
			operateData = (UINT16)(*data) * 45 / 100;	
			(*data) = (UINT8)operateData;
		}

		if (color == 2)
		{
			operateData = (UINT16)(*data) * 35 / 100;	
			(*data) = (UINT8)operateData;
		}
	}



	///LED27
	if (ledCnt == 26)
	{
		if (color == 0)
		{
			operateData = (UINT16)(*data) * 40 / 100;	
			(*data) = (UINT8)operateData;
		}

		if (color == 1)
		{
			operateData = (UINT16)(*data) * 35 / 100;	
			(*data) = (UINT8)operateData;
		}

		if (color == 2)
		{
			operateData = (UINT16)(*data) * 45 / 100;	
			(*data) = (UINT8)operateData;
		}
	}


	if (ledCnt == 25)
	{
		if (color == 0)
		{
			operateData = (UINT16)(*data) * 45 / 100;	
			(*data) = (UINT8)operateData;
		}
	}


	if (ledCnt == 24)
	{
		if (color == 1)
		{
			operateData = (UINT16)(*data) * 90 / 100;	
			(*data) = (UINT8)operateData;
		}

		if (color == 2)
		{
			operateData = (UINT16)(*data) * 90 / 100;	
			(*data) = (UINT8)operateData;
		}
	}

	///all led green down 75%
	if (color == 1)//green
	{
		operateData = (UINT16)(*data) * 75 / 100;	

		(*data) = (UINT8)operateData;
	}
}
#endif


void Led_UpdateMacrix(LED_RGB_STRU *stru,UINT8 dataType,UINT8 *regData)
{
	UINT16 ledCnt,color,offset = 0;//,data;
	UINT8 *addr;
    UINT8 ii,ledIndex = 0,data;
	UINT16 nn = LED_BLINK_ALL_DATALINE;

	switch(dataType)
	{
	case  LED_MODIFY_PWM:
		offset = 2;//2
		nn = LED_PWM_ALL_DATALINE;
		break;
	case  LED_MODIFY_BLINK:
		offset = 1;
		nn = LED_BLINK_ALL_DATALINE;
		break;
	case  LED_MODIFY_STATE:
		offset = 0;
		nn = LED_BLINK_ALL_DATALINE;
		break;
	default:
		break;
	}	

	for (ii = 0;ii < nn;ii++)
	{
		regData[ii] = 0;
	}
	
	///calc the first address
	addr = (UINT8*)((UINT8*)stru + offset);

	///according the ledCnt and color to find data 
	for (ledCnt = 0;ledCnt < LED_NUMBER;ledCnt++)
	{
		for (color = 0;color < 3;color++)///3 color
		{
			data = *addr;

			if (dataType == LED_MODIFY_PWM)
			{
//				Led_Structure_And_Electronic_Require2(ledCnt,color,&data);   
			}

			///according the ledCnt and color to find address and locate the data  
			Led_RegData_SearchLocation(ledIndex,data,regData,dataType);

					
			///for next address
			addr = (UINT8*)(addr + 3);

			ledIndex++;
		}
	}
}

#if 0
void Led_Structure_And_Electronic_Require1(LED_RGB_STRU *stru) 
{
	//UINT16 operateData;

	stru[6].red.pwmReg = stru[17].red.pwmReg;
	stru[6].green.pwmReg = stru[17].green.pwmReg;
	stru[6].blue.pwmReg = stru[17].blue.pwmReg;

	/*
	///electronic inquire LED7 = LED18 
	operateData = (UINT16)stru[17].red.pwmReg * 45 / 100;
	stru[6].red.pwmReg = (UINT8)operateData;

	stru[6].green.pwmReg = stru[17].green.pwmReg;
    
	operateData = (UINT16)stru[17].blue.pwmReg * 75 / 100;
	stru[6].blue.pwmReg = (UINT8)operateData;


	///16
	operateData = (UINT16)stru[16].red.pwmReg * 35 / 100;
	stru[16].red.pwmReg = (UINT8)operateData;

	operateData = (UINT16)stru[16].green.pwmReg * 45 / 100;
	stru[16].green.pwmReg = (UINT8)operateData;

	operateData = (UINT16)stru[16].blue.pwmReg * 35 / 100;
	stru[16].blue.pwmReg = (UINT8)operateData;

	///26
	operateData = (UINT16)stru[26].red.pwmReg * 40 / 100;
	stru[26].red.pwmReg = (UINT8)operateData;

	operateData = (UINT16)stru[26].green.pwmReg * 35 / 100;
	stru[26].green.pwmReg = (UINT8)operateData;

	operateData = (UINT16)stru[26].blue.pwmReg * 45 / 100;
	stru[26].blue.pwmReg = (UINT8)operateData;

	///25
	operateData = (UINT16)stru[25].red.pwmReg * 45 / 100;
	stru[25].red.pwmReg = (UINT8)operateData;


	///24
	operateData = (UINT16)stru[24].green.pwmReg * 90 / 100;
	stru[24].green.pwmReg = (UINT8)operateData;
	operateData = (UINT16)stru[24].blue.pwmReg * 90 / 100;
	stru[24].blue.pwmReg = (UINT8)operateData;

	*/
}
#endif


void Led_Write_StructData(LED_RGB_STRU *stru,UINT8 * regData,UINT8 dataType,UINT8 sendType)
{
	///according to electronic inquire,do some deal
//	Led_Structure_And_Electronic_Require1(stru);
	
///Update the data to the data array, ready to send
	Led_UpdateMacrix(stru,dataType,regData);

	if (sendType == LED_ALL_SEND)
	{
		Led_I2C_SendAllStructData(dataType,regData);

	}else if (sendType == LED_SEPARATE_SEND)
	{
		led_sendType |= dataType;

		switch(dataType)
		{
		case  LED_MODIFY_STATE:	led_stateSend_index = 0;break;
		case  LED_MODIFY_BLINK:	led_blinkSend_index = 0;break;
		case  LED_MODIFY_PWM:	led_pwmSend_index = 0;break;
		default:break;			
		}
	}		
}


void Led_Write_PictureDisplay(UINT8 slaveAdr,UINT8 frame)
{
	UINT8 data = 0;

	LedControl_Read(slaveAdr,POINT_TO_PAGE9,LED_PICTURE_DISPLAY_REG,&data,1);

	data &= ~0x07;
	data |= frame;

	data = frame;
	
	LedControl_Writebyte(slaveAdr,LED_PICTURE_DISPLAY_REG,data);
}

void Led_Write_Page(UINT8 slaveAdr,Point_Page page)
{
	LedControl_Writebyte(slaveAdr,LED_DRIVER_CMD_REG,page);
}

void Led_Write_Shutdown(UINT8 slaveAdr,BOOL flag)
{
	UINT8 data = 0;

	LedControl_Read(slaveAdr,POINT_TO_PAGE9,LED_SHUTDOWN_REG,&data,1);

	if (flag == LED_SHUTDOWN_MODE)
	{
		data &= ~0x01;
	}else
	{
		data |= 0x01;
	}

	LedControl_Writebyte(slaveAdr,LED_SHUTDOWN_REG,data);
}


void Led_Write_Mode(UINT8 slaveAdr,BOOL mode)
{
	UINT8 data = 0;
	
	LedControl_Read(slaveAdr,POINT_TO_PAGE9,LED_CONFIGURATION_REG,&data,1);

	if (mode == LED_PICTURE_MODE)
	{
		data &= ~(0x03 << 3);
	}else
	{
		data |= (0x01 << 3);
	}
    
    LedControl_Writebyte(slaveAdr,LED_CONFIGURATION_REG,data);
}



void Led_Write_GCC(UINT8 slaveAdr,UINT8 dat)
{
	UINT8 data;

	LedControl_Read(slaveAdr,POINT_TO_PAGE9,LED_GCC_REG,&data,1);

	LedControl_Writebyte(slaveAdr,LED_GCC_REG,dat);
}


void Led_Write_AudioSync(UINT8 slaveAdr,UINT8 dat)
{
	UINT8 data;

	LedControl_Read(slaveAdr,POINT_TO_PAGE9,LED_AudioSync_REG,&data,1);

	LedControl_Writebyte(slaveAdr,LED_AudioSync_REG,dat);
}



//void Led_Lightness_Init(void)
//{
//	PWM_1_Start();
//}

//void Led_Lightness_Adjust(UINT8 class)
//{
//	PWM_1_WriteCompare(class);
//}


void Led_Reg_ClearDefault(UINT8 slaverAdr)
{
	UINT16 ii;
	UINT8 dataTmp[LED_PWM_ALL_DATALINE+1];

	///copy for razer 3731 init
	for (ii = 0;ii < (POINT_TO_PAGE8+1);ii++)
	{
		Led_Write_Page(slaverAdr,ii);

		///control reg
		dataTmp[0] = LED_BLINKCONTROL_REG_ADDER;
		Tool_Clear_ArrayData(dataTmp,1,LED_BLINK_ALL_DATALINE);
		LedControl_WriteArray(slaverAdr,dataTmp,(LED_BLINK_ALL_DATALINE+1));

		///control reg
		dataTmp[0] = LED_CONTROL_REG_ADDER;
		Tool_Clear_ArrayData(dataTmp,1,LED_STATE_ALL_DATALINE);
		LedControl_WriteArray(slaverAdr,dataTmp,(LED_STATE_ALL_DATALINE+1));

		///pwm reg
		dataTmp[0] = LED_PWM_REG_ADDER;
		Tool_Clear_ArrayData(dataTmp,1,LED_PWM_ALL_DATALINE);
		LedControl_WriteArray(slaverAdr,dataTmp,(LED_PWM_ALL_DATALINE+1));
	}
	
	Led_Write_Page(slaverAdr,POINT_TO_PAGE9);

	LedControl_Writebyte(slaverAdr,0,0);
	LedControl_Writebyte(slaverAdr,1,0);
	LedControl_Writebyte(slaverAdr,2,0);
	LedControl_Writebyte(slaverAdr,3,0);
	LedControl_Writebyte(slaverAdr,5,0);
	LedControl_Writebyte(slaverAdr,6,0);
	LedControl_Writebyte(slaverAdr,8,0);
	LedControl_Writebyte(slaverAdr,9,0);
	LedControl_Writebyte(slaverAdr,10,1);
	LedControl_Writebyte(slaverAdr,11,0);
	LedControl_Writebyte(slaverAdr,12,0);
	
	LedControl_Writebyte(slaverAdr,0xC2,0x40);
	
	Led_Write_GCC(slaverAdr,255);
	Led_Write_Page(slaverAdr,POINT_TO_PAGE1);
}




void Led_Init(void)
{
	UINT8 ii;

	LedControl_Initial();

	Led_Reg_ClearDefault(LED_DRIVER_SLAVEADDR);
	//Led_Reg_ClearDefault(LED_DRIVER2_SLAVEADDR);

	///Init every Led init data
	for (ii = 0;ii < LED_NUMBER;ii++)
	{
		led_stru[ii].red.ledState = 1;
		led_stru[ii].red.blinkCtrl = 0;
		led_stru[ii].red.pwmReg = 0;
		led_stru[ii].green.ledState = 1;
		led_stru[ii].green.blinkCtrl = 0;
		led_stru[ii].green.pwmReg = 0;
		led_stru[ii].blue.ledState = 1;
		led_stru[ii].blue.blinkCtrl = 0;
		led_stru[ii].blue.pwmReg = 0;
	}

	Led_Write_Page(LED_DRIVER_SLAVEADDR,POINT_TO_PAGE1);
	//Led_Write_Page(LED_DRIVER2_SLAVEADDR,POINT_TO_PAGE1);

	Led_Write_StructData(led_stru,led_blinkRegData,LED_MODIFY_BLINK,LED_ALL_SEND);
	Led_Write_StructData(led_stru,led_pwmRegData,LED_MODIFY_PWM,LED_ALL_SEND);
	Led_Write_StructData(led_stru,led_stateRegData,LED_MODIFY_STATE,LED_ALL_SEND);
 
//	  gloabSetting.settingMode.brightness = 0XCC;
//		gloabSetting.settingMode.LE_value.mul_interface=false;
//	
//	for (ii = 0;ii < LE_Group_Num;ii++)
//	{
//		///1-40   slow->quick
//		gloabSetting.settingMode.LE_value.typeCode[ii].type = LE_G_CODE_Waves;
//		gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.waves.speed = 20;
//		gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.waves.effectIndex = 0;
//		gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.waves.direct = 1;
//		gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.waves.transparency = 100;

		///1-10   slow->quick
//		gloabSetting.settingMode.LE_value.typeCode[ii].type = LE_G_CODE_Breathe;
//		gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.breathe.speed = 15;
//		gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.breathe.customer_mask = 0x0003;
//		gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.breathe.rgb0 = blasoul_RGB;
//		gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.breathe.rgb1 = blasoul_RGB;
//		gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.staticLed.transparency = 100;

//		///1-10   slow->quick
//		 gloabSetting.settingMode.LE_value.typeCode[ii].type = LE_G_CODE_Reaction;
//		 gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.reaction.random = FALSE;
//		 gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.reaction.two = FALSE;
//		 gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.reaction.rgb0 = blasoul_RGB;
//		 gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.reaction.speed = 7;
//		 gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.staticLed.transparency = 100;

//		///1-10   slow->quick
//		gloabSetting.settingMode.LE_value.typeCode[ii].type = LE_G_CODE_ColorCycle;
//		gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.colorCycle.speed = 7;
//		gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.waves.transparency = 100;

//   gloabSetting.settingMode.LE_value.typeCode[ii].type = LE_G_CODE_Static;
//   gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.staticLed.rgb = blasoul_RGB;
//   gloabSetting.settingMode.LE_value.typeCode[ii].typeValue.staticLed.transparency = 100;
//	}

	
}

void Led_ShutDown(void)
{
	Led_Write_Shutdown(LED_DRIVER_SLAVEADDR,LED_SHUTDOWN_MODE);
	//Led_Write_Shutdown(LED_DRIVER2_SLAVEADDR,LED_SHUTDOWN_MODE);
}

void Led_PowerUp(void)
{
	Led_Write_Shutdown(LED_DRIVER_SLAVEADDR,LED_NORMAL_OPRATE_MODE);
	//Led_Write_Shutdown(LED_DRIVER2_SLAVEADDR,LED_NORMAL_OPRATE_MODE);
}

void Led_Adjust_Brightness(UINT8 gcc)
{
	Led_Write_GCC(LED_DRIVER_SLAVEADDR,gcc);

	Led_Write_Page(LED_DRIVER_SLAVEADDR,POINT_TO_PAGE1);
}


void Led_Check_EFT(void)
{
	static UINT16 checkReg_cycle = 0;
	static UINT8 checkRegTrg = 0;	
	//UINT8 data0,data1;

	checkReg_cycle++;
	if (checkReg_cycle > 3000)
	{
		checkReg_cycle = 0;
	
		checkRegTrg++;
		if(checkRegTrg > 2)
		{
			checkRegTrg = 0;
		}
		
		switch(checkRegTrg)
		{
		case 0:
			Led_Write_Shutdown(LED_DRIVER_SLAVEADDR,LED_NORMAL_OPRATE_MODE);

			Led_Write_Page(LED_DRIVER_SLAVEADDR,POINT_TO_PAGE1);
			break;
		case 1:
//			if (!LE_enterOff.put_off)
//			{
//				Led_Adjust_Brightness(gloabSetting.settingMode.brightness);
//			}
			
			break;
		case 2:
			Led_Write_AudioSync(LED_DRIVER_SLAVEADDR,0);
			Led_Write_Page(LED_DRIVER_SLAVEADDR,POINT_TO_PAGE1);
			break;
		default:
			break;
		}


		//Led_Write_GCC(LED_DRIVER_SLAVEADDR,255);

		//Led_Write_Page(LED_DRIVER_SLAVEADDR,POINT_TO_PAGE1);

		//LedControl_Read(LED_DRIVER_SLAVEADDR,POINT_TO_PAGE9,LED_SHUTDOWN_REG,&data0,1);

		//	LedControl_Read(LED_DRIVER_SLAVEADDR,POINT_TO_PAGE9,LED_GCC_REG,&data1,1);
	}
}

#endif

//--------------------------------------------------------------------------
//
// This module implements an interface to the Avago optical chip
// using an SPI port on the PSoC.
//
//--------------------------------------------------------------------------
//
// Copyright 2003-2006, Cypress Semiconductor Corporation.
//
// This software is owned by Cypress Semiconductor Corporation (Cypress)
// and is protected by and subject to worldwide patent protection (United
// States and foreign), United States copyright laws and international 
// treaty provisions. Cypress hereby grants to licensee a personal, 
// non-exclusive, non-transferable license to copy, use, modify, create 
// derivative works of, and compile the Cypress Source Code and derivative 
// works for the sole purpose of creating custom software in support of 
// licensee product to be used only in conjunction with a Cypress integrated 
// circuit as specified in the applicable agreement. Any reproduction, 
// modification, translation, compilation, or representation of this 
// software except as specified above is prohibited without the express 
// written permission of Cypress.
//
// Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND,EXPRESS OR IMPLIED, 
// WITH REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
// Cypress reserves the right to make changes without further notice to the
// materials described herein. Cypress does not assume any liability arising
// out of the application or use of any product or circuit described herein.
// Cypress does not authorize its products for use as critical components in
// life-support systems where a malfunction or failure may reasonably be
// expected to result in significant injury to the user. The inclusion of
// Cypress�?product in a life-support systems application implies that the
// manufacturer assumes all risk of such use and in doing so indemnifies
// Cypress against all charges.
//
// Use may be limited by and subject to the applicable Cypress software
// license agreement.
//
//--------------------------------------------------------------------------


//--------------------------------------
// Included files
//--------------------------------------
#include "gloab.h"
#include "optical_3325.h"
#include "device.h"
#include "timer.h"
#include "GPIO.h"
//#include "port.h"
#include "spi.h"
//#include "isr.h"
#include "main.h"
#include "uart.h"
#include "wwdt.h"
//#include "led.h"
//#include "driver_handle.h"

const UINT8 PowerUpInitialTable3327[15*2]=					
{
	0x20,0x40,
	0x7F,0x01,
	0x1A,0x50,
	0x6F,0xFF,
	0x6C,0x3B,
	0x6D,0x2B,
	0x7A,0x4F,
	0x49,0x40,	
	0x7f,0x00,
	0x4A,0x16,
	0x4D,0x53,
	0X1B,0x21,
	0X62,0x16,
	0X1A,0x11,
	0X7F,0x03,
};

const unsigned char dpi_value_array[79] = {
	RESOLUTION_200,RESOLUTION_300,RESOLUTION_400,RESOLUTION_500,RESOLUTION_600,RESOLUTION_700,RESOLUTION_800,RESOLUTION_900,
	RESOLUTION_1000,RESOLUTION_1100,RESOLUTION_1200,RESOLUTION_1300,RESOLUTION_1400,RESOLUTION_1500,RESOLUTION_1600,RESOLUTION_1700,
	RESOLUTION_1800,RESOLUTION_1900,RESOLUTION_2000,RESOLUTION_2100,RESOLUTION_2200,RESOLUTION_2300,RESOLUTION_2400,RESOLUTION_2500,
	RESOLUTION_2600,RESOLUTION_2700,RESOLUTION_2800,RESOLUTION_2900,RESOLUTION_3000,RESOLUTION_3100,RESOLUTION_3200,RESOLUTION_3300,
	RESOLUTION_3400,RESOLUTION_3500,RESOLUTION_3600,RESOLUTION_3700,RESOLUTION_3800,RESOLUTION_3900,RESOLUTION_4000,RESOLUTION_4100,
	RESOLUTION_4200,RESOLUTION_4300,RESOLUTION_4400,RESOLUTION_4500,RESOLUTION_4600,RESOLUTION_4700,RESOLUTION_4800,RESOLUTION_4900,
	RESOLUTION_5000,RESOLUTION_5100,RESOLUTION_5200,RESOLUTION_5300,RESOLUTION_5400,RESOLUTION_5500,RESOLUTION_5600,RESOLUTION_5700,
	RESOLUTION_5800,RESOLUTION_5900,RESOLUTION_6000,RESOLUTION_6100,RESOLUTION_6200,RESOLUTION_3900,RESOLUTION_4000,RESOLUTION_4100,
	RESOLUTION_4100,RESOLUTION_4200,RESOLUTION_4300,RESOLUTION_4300,RESOLUTION_4400,RESOLUTION_4400,RESOLUTION_4500,RESOLUTION_4600,
	RESOLUTION_4600,RESOLUTION_4700,RESOLUTION_4800,RESOLUTION_4800,RESOLUTION_4900,RESOLUTION_4900,RESOLUTION_5000
};

unsigned char 	optical_initilized;
OPTICAL_REPORT optical_report={0,0,0,0};

OPTICAL_FLOAT_REPORT x_data, y_data;
OPTICAL_DECIMAL_REPORT x_decimal, y_decimal;
unsigned char a_decimal_count=0, b_decimal_count=0;
UINT8 optical_double_flag;

#ifdef ENABLE_OPTICAL_SENSOR

INT8 Dyn_SM_PTH1; //Threshold of new ASM
INT8 Dyn_SM_NTH1;
INT8 Dyn_SM_PTH2;
INT8 Dyn_SM_NTH2;
unsigned char ASM_Flag = 0x00;
INT16 SumX=0;
INT16 SumY=0;
INT16 PreX=0;
INT16 PreY=0;
unsigned char SQUAL;

unsigned char power_reset_flag;
unsigned char optical_configration;

void TimerDelayMsec(uint32_t t_count)
{
	DrvDelay(t_count *10);//delay *10 = 1ms
}

void optical_init(void)
{
	UINT8 temp, i;
	optical_initilized = 1;
	
	//if (power_reset_flag)
	{
		//power_reset_flag = 0;
		optical_spi_put( REG_POWER_UP_RESET, RESET_VALUE ); 
	}
	NCS_ON();//(MOUSE_OPTICAL_nCS_MASK);
	TimerDelayMsec(1);
	NCS_OFF();//(MOUSE_OPTICAL_nCS_MASK);
	TimerDelayMsec(1);

	optical_spi_put( REG_POWER_UP_RESET, POWER_UP_VALUE );  

	nsAPI_SSP_Delay1us(50);

	TimerDelayMsec(60);

	nsAPI_SSP_Delay1us(50);

	optical_spi_get( REG_MOTION );
	optical_spi_get( REG_DELTA_X_LO );
	optical_spi_get( REG_DELTA_X_HI);
	optical_spi_get( REG_DELTA_Y_LO );
	optical_spi_get( REG_DELTA_Y_HI);
    
	for(i = 0; i < 15; i++)
	{
		optical_spi_put(PowerUpInitialTable3327[i << 1], PowerUpInitialTable3327[(i << 1) + 1]);
	}
	
	optical_spi_get(0x1c);
	
	i = optical_spi_get(0x1a);
	
	temp = ((i >> 1) + 0x23) ^ 0x5a;
	
	optical_spi_put(0x1B, temp);
	
	optical_spi_put(0x7F, 0x00);
	
//	optical_spi_put(0x20, 0x20);
	
	while(optical_spi_get(REG_PRODUCT_ID) != PRODUCT_ID);
}

//--------------------------------------------------------------------------
// optical_get_report
//--------------------------------------------------------------------------
#if 0
unsigned char optical_get_report( W_n_B_CONVERT *x, W_n_B_CONVERT *y)
{
	W_n_B_CONVERT x_delta,y_delta;
	unsigned char check_id_count;
//	static unsigned char ensure_count;
//	UINT8 tempBurst[16];
//	UINT8 motion;
	
//	optical_report->x_lo = 0;		
//	optical_report->y_lo = 0;
//	optical_report->x_hi = 0;		
//	optical_report->y_hi = 0;	

	if (optical_spi_get(REG_PRODUCT_ID) != PRODUCT_ID)   //read ID reset
	{
		NCS_ON();
		TimerDelayMsec(1);
		NCS_OFF();
		TimerDelayMsec(1);
		check_id_count = 15;
		while ((optical_spi_get(REG_PRODUCT_ID) != PRODUCT_ID) && check_id_count)
		{
			check_id_count--;
			optical_power_rest();
		}
		return 0;
	}
//	SPI_Burst_Read(REG_BURST_MOTION_READ,tempBurst,16);
//	motion = tempBurst[Burst_Motion_Index] & MOTION_ST;
// if(!MOTION_PIN_STATUS)
 {
//	 DrvGPIOOutput(0,14,1) ;
	if ((optical_spi_get(REG_MOTION)) & MOTION_ST)
//	 if(motion&MOTION_ST)
	{
		x_delta.b.lo = optical_spi_get( REG_DELTA_X_LO );	//x.b.lo;
		x_delta.b.hi = optical_spi_get( REG_DELTA_X_HI);	//x.b.hi;		
		y_delta.b.lo = optical_spi_get( REG_DELTA_Y_LO );	//y.b.lo;
		y_delta.b.hi = optical_spi_get( REG_DELTA_Y_HI);	//y.b.hi;

		(*x).w = x_delta.w;
		(*y).w = y_delta.w;

//		optical_report->x_lo = x.b.lo;
//		optical_report->x_hi = x.b.hi;		
//		optical_report->y_lo = y.b.lo;
//		optical_report->y_hi = y.b.hi;

		return 1;
	}
 }
	return 0;
}
#endif
#if 1
unsigned char optical_get_report( W_n_B_CONVERT *x, W_n_B_CONVERT *y)
{
	W_n_B_CONVERT x_delta,y_delta;
	//unsigned char check_id_count;
	  int16_t X=0, Y=0; 
	  unsigned int a=0, b=0;
	  unsigned char a_decimal=0, b_decimal=0;
	  float X_temp=0, Y_temp=0;
	UINT8 burst_motion, i;
	UINT8 sensor_data[12];

#if 0
	if (optical_spi_get(REG_PRODUCT_ID) != PRODUCT_ID)   //read ID reset
	{
		NCS_ON();
		TimerDelayMsec(1);
		NCS_OFF();
		TimerDelayMsec(1);
		check_id_count = 15;
		while ((optical_spi_get(REG_PRODUCT_ID) != PRODUCT_ID) && check_id_count)
		{
			check_id_count--;
			optical_power_rest();
		}
		return 0;
	}
#endif
	
	burst_motion = BURST_MOTION;
	NCS_OFF();

	nsAPI_SSP_Delay1us(1);
	
	SSP_Send(Optical_SPI, &burst_motion, 1);	//12us
	
	nsAPI_SSP_Delay1us(5);

	for(i = 0; i < 12; i++)
	{
		SSP_Receive(Optical_SPI,&sensor_data[i],1);		//12us
//		nsAPI_SSP_Delay1us(2);
	}	

	NCS_ON();
	nsAPI_SSP_Delay1us(5);
	
	if (sensor_data[0] & MOTION_ST)
	{
		x_delta.b.lo = sensor_data[2];	//x.b.lo;
		x_delta.b.hi = sensor_data[3];	//x.b.hi;		
		y_delta.b.lo = sensor_data[4];	//y.b.lo;
		y_delta.b.hi = sensor_data[5];	//y.b.hi;
		
//     #if (ENABLE_UART)
//		 InitUART(9600) ;
//		 xprintf("lo=%d\r\n",x_delta.b.lo);
//		 		TimerDelayMsec(1);
//		 xprintf("hi=%d\r\n",x_delta.b.hi);
//		 		TimerDelayMsec(1);
//		 #endif
		
		 X= ((x_delta.b.hi<<8)|(x_delta.b.lo));
		 Y= ((y_delta.b.hi<<8)|(y_delta.b.lo));	 
		
#if 0
     if((X>=0)&&(X<=27306))	 
	   {
			 X=(float)X*(1.2);
//				 TimerDelayMsec(2);
//			 #if (ENABLE_UART)
//			 InitUART(9600) ;
//			 PrintFloat(X);
//			 TimerDelayMsec(1);
//			 #endif
			 a=(unsigned int)X;
			 a_decimal=(X-a)*10;
			 if((a_decimal==1)||(a_decimal==3)||(a_decimal==5)||(a_decimal==7)) //因为浮点数计算的小数位会有误差，所以这样可以消除误差值，如0.19=0.2
			 {
					a_decimal+=1;
			 }
			 a_decimal_count+= a_decimal;       //累加小数点后的数据
			 if(a_decimal_count>10)          //如果超过10 ，则坐标值加1，超过10的数再存放在累加变量里面。
			 {
					a= a+1;
					a_decimal_count= a_decimal_count%10;
			 }
			 x_delta.b.lo= a&0x00FF;
			 x_delta.b.hi= a>>8;
		 }
#endif 		 
		 if(optical_double_flag)
		 {
			 if(((X<0)&&(X>=(-27306)))||((X>=0)&&(X<=27306)))
			 {
					X_temp= abs(X);
					X_temp=((float)(X_temp))*(1.6);
					a=(unsigned int)X_temp;       //取整数位
					a_decimal=(X_temp-a)*10;      //取小数点后的数再乘10变成整数
					if((a_decimal==1)||(a_decimal==3)||(a_decimal==5)||(a_decimal==7)) //因为浮点数计算的小数位会有误差，所以这样可以消除误差值，如0.19=0.2
					{
						 a_decimal+=1;
					}
					a_decimal_count+= a_decimal; //累加小数点后的数据
					if(a_decimal_count>10)   //如果超过10 ，则坐标值加1，超过10的数再存放在累加变量里面。
					{
						a= a+1;
						a_decimal_count= a_decimal_count%10;
					}
					if(X<0)   //如果是负数，则将结果取反。
					{
						 a= -a;
					}
					x_delta.b.lo= a&0x00FF;
					x_delta.b.hi= a>>8;
			 }

			 if(((Y<0)&&(Y>=(-27306)))||((Y>=0)&&(Y<=27306)))	
			 {
					 Y_temp= abs(Y);
					 Y_temp=((float)(Y_temp))*(1.6);
				 
					 b=(unsigned int)Y_temp;
					 b_decimal=(Y_temp-b)*10; 
				 
					 if((b_decimal==1)||(b_decimal==3)||(b_decimal==5)||(b_decimal==7))
					 {
						 b_decimal+=1;
					 }
					 b_decimal_count+= b_decimal;
					 if(b_decimal_count>10)
					 {
							b= b+1;
							b_decimal_count= b_decimal_count%10;
					 }				 
					 if(Y<0)
					 {
							b= -b;
					 }
					 y_delta.b.lo= b&0x00FF;
					 y_delta.b.hi= b>>8;	 
			 }
		 }
			 
		(*x).w = x_delta.w;
		(*y).w = y_delta.w;

		return 1;
	}

	return 0;
}
#endif

void OP_3325_Packet(W_n_B_CONVERT * x,W_n_B_CONVERT *y,UINT8 *buffer)
{
	buffer[2] = (*x).b.lo;
	buffer[3] = (*x).b.hi;
	buffer[4] = (*y).b.lo;
	buffer[5] = (*y).b.hi;

	///// clear optical data
	(*x).w = 0;
	(*y).w = 0;
}

/*
unsigned char optical_get_report( OPTICAL_REPORT* optical_report)
{
	W_n_B_CONVERT x,y;
	
	unsigned char i,mydata = 0xee;
	unsigned char recebuff[6];

	optical_report->x_lo = 0;		
	optical_report->y_lo = 0;
	optical_report->x_hi = 0;		
	optical_report->y_hi = 0;	

	if (optical_spi_get(REG_MOTION) & MOTION_ST)
	{
		//optical_spi_get(REG_BURST_MOTION_READ);

		port1_drive_off(MOUSE_OPTICAL_nCS_MASK);

		PRT1DM0 &= ~0x02;
		PRT1DM1 &= ~0x02;
		asm("nop");
		asm("nop");
	
		// Write the address
		SPIM_Radio_TX_BUFFER_REG = REG_BURST_MOTION_READ;
		while(!(SPIM_Radio_bReadStatus() & SPIM_Radio_SPIM_SPI_COMPLETE));
		mydata = SPIM_Radio_RX_BUFFER_REG;

		PRT1DM0 |= 0x02;
		PRT1DM1 |= 0x02;

		TimerDelay10usec();
		TimerDelay10usec();
		TimerDelay10usec();
		TimerDelay10usec();

		for (i=0; i<6; i++)
		{
			SPIM_Radio_TX_BUFFER_REG = 0xFF;
			while(!(SPIM_Radio_bReadStatus() & SPIM_Radio_SPIM_SPI_COMPLETE));
			recebuff[i] = SPIM_Radio_RX_BUFFER_REG;
		}
		
		optical_report->x_lo = recebuff[2];	//x.b.lo;
		optical_report->x_hi = recebuff[3];	//x.b.hi;		
		optical_report->y_lo = recebuff[4];	//y.b.lo;
		optical_report->y_hi = recebuff[5];	//y.b.hi;

		PRT1DM0 &= ~0x02;
		PRT1DM1 &= ~0x02;

		port1_drive_on(MOUSE_OPTICAL_nCS_MASK);
		
		return 1;
	}

	return 0;
}*/

void optical_power_rest(void)
{
	//power_reset_flag = 1;
	optical_init();
}

void optical_set_dpi(UINT8 optical_configration)
{
	UINT8 repeat_cont = 200;
	UINT8 temp;
//	if(dpi_fast_swtich_setting)
//	{
//		optical_configration = gloab_setting.dpi_value_fast_swtich ;
//	}
//	else 
//	{
//		if(dpi_setting==DPI_SETTING_1)
//		{
//			//500cpi
//		//没有驱动情况下 optical_configration =gloab_setting.dpi_value_lev1 或gloab_setting.dpi_value_lev2
//		// 有驱动的情况下 会根据Host_to_Device_Buff[PARAM_1]; 	Host_to_Device_Buff[PARAM_2]; 	gloab_setting.dpi_value_lev3 = Host_to_Device_Buff[PARAM_3];
//		// 的值 设置optical_configration 

//			optical_configration = gloab_setting.dpi_value_lev1;

//		}else if(dpi_setting==DPI_SETTING_2)
//		{
//			//1500cpi
//			optical_configration = gloab_setting.dpi_value_lev2;
//		}else if(dpi_setting==DPI_SETTING_3)
//		{
//			//3000cpi
//			optical_configration = gloab_setting.dpi_value_lev3;
//		}
//		else if (dpi_setting==DPI_SETTING_4)
//		{
//			//5000cpi
//			optical_configration = gloab_setting.dpi_value_lev4;
//		}
//	}
	
	while(repeat_cont --)
	{
		wwdt_feed();
		optical_spi_put( REG_RESOLUTION , optical_configration );   //set  cpi
		nsAPI_SSP_Delay1us(50);
//		TimerDelay100usec();
		temp =  optical_spi_get( REG_RESOLUTION );
		nsAPI_SSP_Delay1us(50);
//		TimerDelay100usec();
		if(temp == optical_configration)
			break;

	}
}
//--------------------------------------------------------------------------
// optical_power_down
//--------------------------------------------------------------------------
void optical_power_down(void)
{
	optical_spi_put( REG_SHUTDOWN, SHUTDOWN_VALUE);
	nsAPI_SSP_Delay1us(50);
//	TimerDelay50usec();
	
	optical_initilized = 0;
}
#endif

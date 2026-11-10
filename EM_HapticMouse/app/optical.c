#include "device.h"
//#include "../inc/optical.h"
#include "../inc/usbhw.h"
#include "../inc/hiduser.h"
#include "GPIO.h"
#include "SPI.h"
#include "custom.h"
#include "operation_flash.h"
//#include "timer.h"
//#include "Port.h"
 #include "pc_communicate.h" 

#if 0
#define DRIVER_RESET_OFF DrvGPIOOutput(1,19,0);	// Reset pin lo
#define DRIVER_RESET_ON  DrvGPIOOutput(1,19,1);	// Reset pin hi
#define DRIVER_NCS_ON    DrvGPIOOutput(0,20,1);	// 
#define DRIVER_NCS_OFF   DrvGPIOOutput(0,20,0);	// 

//const unsigned char tracking_SROM[];
BOOL optical_had_data;
BOOL optical_packet;

UINT8 pixThresh_back;
UINT8 min_sq_back;

UINT8 pix_thresh;
UINT8 min_sq;

OP_liftCut_Stru OP_liftCut_run = {FALSE,FALSE,FALSE};

UINT8 * sendBuf;
uint8_t Timer_add=0;
//#define OP_Debug_Flag	

void OP_Debug(uint32_t dat)
{
#ifdef OP_Debug_Flag

	Tool_USB_32bitsData(dat);

#endif
}

// By Martin 2017.11.13
void TimerDelayMsec(uint32_t t_count)
{
	DrvDelay(t_count *10);//delay *10 = 1ms
}

void Optical_HWReset(void)
{
	DRIVER_RESET_OFF;
    TimerDelayMsec(30);      
	DRIVER_RESET_ON;
    TimerDelayMsec(80);            
}

void Perform_SROM_Download(const UINT8 *srom_addr,UINT16 len,UINT8 checkID)
{
	UINT8  regTmp;

	regTmp = Read_Sensor(Config2);
	regTmp &= ~Rest_En;
	Write_Sensor(Config2,regTmp);

	Write_Sensor(SROM_ENABLE,0x1d);

	TimerDelayMsec(20);

	Write_Sensor(SROM_ENABLE,0x18);
		
	SPI_Burst_Write(SROM_Load_Burst,srom_addr,len);

	regTmp = Read_Sensor(SROM_ID);
	if (regTmp != checkID)
	{
//		while (1)
//		{
//      if(Timer_add>=10)
//		 {
//		   Timer_add=0;
//			 return;
//		 }
//		}
	}

	Write_Sensor(Config2,0x00);
}


void Sensor_PowerUp(void)
{
	UINT8 id = 0;
//	SpiModule_Start();  
	Optical_HWReset();
	DRIVER_NCS_ON;
	TimerDelayMsec(1);
	DRIVER_NCS_OFF;
  TimerDelayMsec(1);
	Write_Sensor(Power_Up_Reset ,0X5A);
	TimerDelayMsec(1);
  	
	while (id != Product_ID_Value)
	{ 
		id = Read_Sensor(Product_ID);
	}

	TimerDelayMsec(60);

	Read_Sensor(Burst_Delta_X_L_Index);
	Read_Sensor(Burst_Delta_X_H_Index);
	Read_Sensor(Burst_Delta_Y_L_Index);
	Read_Sensor(Burst_Delta_Y_H_Index);
	Read_Sensor(Burst_SQUAL_Index);
}

void Sensor_Shutdown(void)
{
	Write_Sensor(Shutdown,0XB6);
}

void Optical_Init(void)
{
	UINT8 regTmp;
	
	Optical_HWReset();
	
	Sensor_PowerUp();

	Perform_SROM_Download(tracking_SROM,Get_SROM_0X04_Size(),TRACKING_SROM_VER);   //Get_SROM_0X04_Size()??????
          
	///Setting X Y CPI independent
	regTmp = Read_Sensor(Config2);
	regTmp |= Rpt_Mod;
	Write_Sensor(Config2,regTmp);   
	
	Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 
}




void OP_Set_LiftOff(void)
{
	Write_Sensor(PIXEL_THRESHOLD_ADDR, OP_PixThresh_Init);     //gloabSetting.settingMode.pixThresh); 
	Write_Sensor(MIN_SQ_RUN_ADDR, OP_Min_Sq_Init);             //gloabSetting.settingMode.min_sq);   

	Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 
}

void Op_read(void)
{
	//UINT8 a,b;

	//a = Read_Sensor(PIXEL_THRESHOLD_ADDR);
	//b = Read_Sensor(MIN_SQ_RUN_ADDR);
}


void Optical_Stop(void)
{
    //SpiModule_Stop();
    //Optical_Interrupt_Stop();
}


void Optical_Check_ID(void)
{
	static UINT16  checkIdCycle = 0;
	UINT8 checkCnt = 30;

	checkIdCycle++;

	if (checkIdCycle > 5000)
	{
		checkIdCycle = 0;
		while (Read_Sensor(Product_ID) != PRODUCT_ID)
		{
			TimerDelayMsec(10);
			if (checkCnt)checkCnt--;
			if (!checkCnt)
			{
				while(1);
			}
		}
		Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 

	}
}

BOOL Optical_Read(W_TO_B_CONVERT *x,W_TO_B_CONVERT *y)
{
	UINT8 motion;
	UINT8 ret=0; 
	UINT8 tempBurst[6];
	W_TO_B_CONVERT x_delta,y_delta;
	
	if(!MOTION_PIN_STATUS)
	{
		
		//Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 
		SPI_Burst_Read(Motion_Burst,tempBurst,6);
		motion = tempBurst[Burst_Motion_Index] & MOTION_DETECT;

		if(motion & MOTION_DETECT)
		{          
			x_delta.b.lo = tempBurst[Burst_Delta_X_L_Index];
			x_delta.b.hi = tempBurst[Burst_Delta_X_H_Index];
			y_delta.b.lo = tempBurst[Burst_Delta_Y_L_Index];
			y_delta.b.hi = tempBurst[Burst_Delta_Y_H_Index];

			(*x).w += x_delta.w;
		  (*y).w += y_delta.w;			
			
			ret = 1;

			optical_had_data = TRUE;
						
		}
		else
		{
			ret =  0;
		} 
	}

	return ret;
}

BOOL Optical_Scan(void)
{
	BOOL result = FALSE;

	if (optical_had_data)
	{
		optical_had_data = FALSE;

		result = TRUE;
	}
	return result;
}

void Optical_LiftCutOff_Cal_Scan(void)
{
	UINT8 cal_status;
	static UINT8 dealCycle = 0;

	if (!OP_liftCut_run.start)
	{
		return;
	}

	if (dealCycle)dealCycle--;
	
	if (!dealCycle)
	{
		dealCycle = 20;
	}else 
	{
		return;
	}

	// Loop to wait for calibration to end
	cal_status = Read_Sensor(LiftCuttoff_Tune1);

//	Optical_LiftCutOff_Result(cal_status);

	if ((cal_status == CAL_SUCCESS_1) || (cal_status == CAL_SUCCESS_2))
	{
		// Done calibration
		OP_liftCut_run.identify_desktop = TRUE;

		//		debug!!!!!!!!!!!!!S
		OP_Debug(0x4444);
		///		debug!!!!!!!!!!!!!E
	}

	if (cal_status == CAL_FAIL)
	{
		// Enable Calibration
		//Write_Sensor(LiftCuttoff_Tune1, RUN_CAL_EN);

		Optical_Init();

		Custom_Set_DPI(gloabSetting.settingMode.dpi_gear,FALSE,FALSE,FALSE);

		OP_HeightRunning_Clear();

		OP_liftCut_run.start = FALSE;

		//			debug!!!!!!!!!!!!!S
		OP_Debug(0x5555);
		///			debug!!!!!!!!!!!!!E

		return;
	}


	if (!OP_liftCut_run.identify_desktop)
	{
		Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 

		//			debug!!!!!!!!!!!!!S
		OP_Debug(0x6666);
		///			debug!!!!!!!!!!!!!E

		return;
	}

	// Stop Calibration
	Write_Sensor(LiftCuttoff_Tune1, RUN_DISABLE);
		
	// Delay for 30ms
	TimerDelayMsec(1);

	// read RPTH[6:0] (0x65) and write to Pixel Threshold (0x2c)
	pix_thresh = Read_Sensor(LCO_TUNE2_ADDR) & 0x7F;
	// read RMSQ[7:0] (0x41) and write to Min_SQ_Run (0x2B)
	min_sq = Read_Sensor(LCO_TUNE3_ADDR);

	OP_liftCut_run.start = FALSE;

	Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 

	return;
}

void Optical_Packet(W_TO_B_CONVERT * x,W_TO_B_CONVERT *y,UINT8 *buffer)
{
	buffer[2] = (*x).b.lo;
	buffer[3] = (*x).b.hi;
	buffer[4] = (*y).b.lo;
	buffer[5] = (*y).b.hi;

	///// clear optical data
	(*x).w = 0;
	(*y).w = 0;
}


void Optical_Set_DPI(UINT8 x,UINT8 y)
{
	if ((x < 0x78) && (y < 0x78))
	{
	  Write_Sensor(Resolution_L,y);
		Write_Sensor(Config5_L,x);

//		Write_Sensor(Config1,y);
//		Write_Sensor(Config5,x);
	}
}

#if 1

BOOL OP_LiftCutOff_Enter(void)
{
	Sensor_PowerUp();

	// Download SROM 0x81, return fail if download was not successful
	Perform_SROM_Download(liftCutOff_calibration_SROM,Get_SROM_0X81_Size(),LiftCutOff_SROM_VER);

	// Delay for 30ms
	TimerDelayMsec(30);

	Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 

	//	debug!!!!!!!!!!!!!S
	OP_Debug(0x1111);
	//	debug!!!!!!!!!!!!!E

	return TRUE;
}

void OP_LiftCutOff_Quit(void)
{
	OP_liftCut_run.start = FALSE;

	Optical_Init();	

	Custom_Set_DPI(gloabSetting.settingMode.dpi_gear,FALSE,FALSE,FALSE);

	OP_HeightRunning_Clear();

	Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 
}

void OP_LiftCutOff_Start(void)
{
	// Enable Calibration
	Write_Sensor(LiftCuttoff_Tune1, RUN_CAL_EN);

	OP_liftCut_run.start = TRUE;
	OP_liftCut_run.identify_desktop = FALSE;

	OP_HeightRunning_Set();

	Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 

	//			debug!!!!!!!!!!!!!S
	OP_Debug(0x2222);
	///			debug!!!!!!!!!!!!!E
}

void OP_LiftCutOff_Stop(void)
{
	Write_Sensor(LiftCuttoff_Tune1, RUN_DISABLE);

	OP_liftCut_run.start = FALSE;
	OP_liftCut_run.identify_desktop = FALSE;	

	OP_HeightRunning_Clear();

	Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 


}

void OP_LiftCutOff_Determine(void)
{
	Sensor_PowerUp();
	// Download SROM 0x81, return fail if download was not successful
	Perform_SROM_Download(tracking_SROM, Get_SROM_0X04_Size(),TRACKING_SROM_VER);

	///back the last value
	pixThresh_back = Read_Sensor(PIXEL_THRESHOLD_ADDR);
	min_sq_back = Read_Sensor(MIN_SQ_RUN_ADDR);

	Write_Sensor(PIXEL_THRESHOLD_ADDR, pix_thresh);
	Write_Sensor(MIN_SQ_RUN_ADDR, min_sq);

	Custom_Set_DPI(gloabSetting.settingMode.dpi_gear,FALSE,FALSE,FALSE);

	///store new value
	gloabSetting.settingMode.pixThresh = pix_thresh;
	gloabSetting.settingMode.min_sq = min_sq;

	OperateFlash_Store_ModeSetting(gloabSetting.setting.currentMode);

	OP_liftCut_run.start = FALSE;

	OP_HeightRunning_Clear();

	Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 

	receive_buff[0] = OP_HEIGHT_LiftCutOff;
	receive_buff[1] = Op_Height_Get_Set;
	receive_buff[2] = Op_Height_Return;
	receive_buff[3] = pix_thresh;
	receive_buff[4] = min_sq;

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, receive_buff, TRANSFER_LENGTH); 	


	//	debug!!!!!!!!!!!!!S
	OP_Debug(0x3333);
	//	debug!!!!!!!!!!!!!E
}

void OP_LiftCutOff(UINT8 type)
{
	switch(type)
	{
	case OP_H_Enter:
		OP_LiftCutOff_Enter();
		break;
	case OP_H_Start:
		OP_LiftCutOff_Start();
		break;
	case OP_H_Stop:
		OP_LiftCutOff_Stop();
		break;
	case OP_H_Quit:
		OP_LiftCutOff_Quit();
		break;
	case OP_H_Determine:
		OP_LiftCutOff_Determine();
		break;
	default:
		break;
	}
}	

#endif
void OP_HeightRunning_Set(void)
{
	OP_liftCut_run.running = TRUE;
}

BOOL OP_HeightRunning_Get(void)
{
	return OP_liftCut_run.running;
}

void OP_HeightRunning_Clear(void)
{
	OP_liftCut_run.running = FALSE;
}

void Optical_LiftCutOff_Set(UINT8 pix_thresh,UINT8 min_sq)
{
	Write_Sensor(PIXEL_THRESHOLD_ADDR, pix_thresh);
	Write_Sensor(MIN_SQ_RUN_ADDR, min_sq);

	gloabSetting.settingMode.pixThresh = pix_thresh;
	gloabSetting.settingMode.min_sq = min_sq;

	OperateFlash_Store_ModeSetting(gloabSetting.setting.currentMode);
}

#endif 


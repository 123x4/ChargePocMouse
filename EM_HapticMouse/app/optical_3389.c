#include "gloab.h"
//#include "main.h"
#include "device.h"
#include "optical_3389.h"
#include "timer.h"
#include "custom.h"
#include "GPIO.h"
#include "pc_communicate.h"
#include "operation_flash.h"


#ifdef OP_3389_Enable  

#define DRIVER_RESET_OFF DrvGPIOOutput(1,19,0);	// Reset pin lo
#define DRIVER_RESET_ON  DrvGPIOOutput(1,19,1);	// Reset pin hi
#define DRIVER_NCS_ON    DrvGPIOOutput(0,20,1);	// 
#define DRIVER_NCS_OFF   DrvGPIOOutput(0,20,0);	// 

BOOL optical_had_data;

BOOL optical_packet;

UINT8 pixThresh_back;
UINT8 min_sq_back;

UINT8 pix_thresh;
UINT8 min_sq;
uint8_t sensor_timer=0;

OP_liftCut_Stru OP_liftCut_run = {FALSE,FALSE,FALSE};

UINT8 * sendBuf;

void TimerDelayMsec(uint32_t t_count)
{
	DrvDelay(t_count *10);//delay *10 = 1ms
}

void OP_3389_PowerUp(void)
{
	UINT8 id = 0;

	SpiModule_Start();  

	DRIVER_NCS_ON;
	TimerDelayMsec(1);
	DRIVER_NCS_OFF;
	
//	while (id != Product_ID_Value)
//	{
//		id = Read_Sensor(Product_ID);

//		TimerDelayMsec(2);
//	}
	
	Write_Sensor(Power_Up_Reset,0X5A);

	//Write_Sensor(Power_Up_Reset,0X5A);

	TimerDelayMsec(50);

	Read_Sensor(Burst_Delta_X_L_Index);
	Read_Sensor(Burst_Delta_X_H_Index);
	Read_Sensor(Burst_Delta_Y_L_Index);
	Read_Sensor(Burst_Delta_Y_H_Index);
	Read_Sensor(Burst_SQUAL_Index);
}


void OP_3389_Perform_SROM_Download(const UINT8 *srom_addr,UINT16 len,UINT8 checkID)
{
	UINT8  regTmp;
	//regTmp = Read_Sensor(Config2);
	//regTmp &= ~Rest_En;
	//Write_Sensor(Config2,regTmp);
#ifndef RUN_ALL_SROM
SROM_LOAD: 	 
#endif
	Write_Sensor(Config2,0X00);
	
	Write_Sensor(SROM_ENABLE,0x1d);

	TimerDelayMsec(10);//TimerDelayMsec(20);

	Write_Sensor(SROM_ENABLE,0x18);
	
  TimerDelayMsec(1);
	
	SPI_Burst_Write(SROM_Load_Burst,srom_addr,len);
  
	TimerDelayMsec(1);
	
	regTmp = Read_Sensor(SROM_ID);
	
	if (regTmp != checkID)
	{
		
#ifndef RUN_ALL_SROM 
		goto SROM_LOAD;
#endif		
//		while (1)
//		{

//		}
	}

	Write_Sensor(Config2,0x00);
}

void OP_3389_Init(void)
{
	UINT8 regTmp = 0;
	UINT8 readCnt = 0;
  uint8_t clockcounter=0;
	OP_3389_PowerUp();

	OP_3389_Perform_SROM_Download(OP_3389_SROM_0X05,OP_3389_SROM_0X05_GetSize(),OP_3389_SROM_Ver);

	///3389 add
	Write_Sensor(0x3D,0x80);

	while ((++readCnt < 60) && (regTmp != 0XC0))
	{
			for(clockcounter = 0; clockcounter < 95; clockcounter++)	{				//1ms interval from osciloscope
		nsAPI_SSP_Delay1us(9);					
		}	
		regTmp = Read_Sensor(0X3D);

	}

	Write_Sensor(0X3D,0X00);  
	Write_Sensor(0X10,0X20);  
	

	///Setting X Y CPI independent
	Write_Sensor(0X10,0X04);  
//	regTmp = Read_Sensor(Config2);
//	regTmp |= Rpt_Mod;
//	Write_Sensor(Config2,regTmp);   
//	Write_Sensor(0X11,0X0D);    
//	Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 
//	OP_3389_Set_DPI(DPI_2400,DPI_2400);  
}

void OP_3389_Hardware_Rest(void)
{
	DRIVER_RESET_OFF;
	TimerDelayMsec(10);      
	DRIVER_RESET_ON;
	TimerDelayMsec(50);            
}


void OP_3389_Shutdown(void)
{
	Write_Sensor(Shutdown,0XB6);
}



void OP_3389_Check_ID(void)
{
	static UINT16  checkIdCycle = 0;
	UINT8 checkCnt = 30;

	checkIdCycle++;

	if (checkIdCycle > 5000)
	{
		checkIdCycle = 0;
		while (Read_Sensor(Product_ID) != Product_ID_Value)
		{
			TimerDelayMsec(10);
			if (checkCnt)checkCnt--;
			if (!checkCnt)
			{
				while(1);
			}
		}
		Write_Sensor(Motion_Burst,0x01); //Not this sentence x y has a value of 0 
	}
}

BOOL OP_3389_Read(W_TO_B_CONVERT *x,W_TO_B_CONVERT *y)
{
	UINT8 motion;
	UINT8 ret=0; 
	UINT8 tempBurst[6];
	W_TO_B_CONVERT x_delta,y_delta;
  
	if(!MOTION_PIN_STATUS)
	{
		//Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 
//		DrvGPIOOutput(0,23,1) ;
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

BOOL OP_3389_Scan(void)
{
	BOOL result = FALSE;

	if (optical_had_data)
	{
		optical_had_data = FALSE;

		result = TRUE;
	}
	return result;
}


void OP_3389_Packet(W_TO_B_CONVERT * x,W_TO_B_CONVERT *y,UINT8 *buffer)
{
	buffer[2] = (*x).b.lo;
	buffer[3] = (*x).b.hi;
	buffer[4] = (*y).b.lo;
	buffer[5] = (*y).b.hi;

	///// clear optical data
	(*x).w = 0;
	(*y).w = 0;
}


void OP_3389_Set_DPI(UINT16 x,UINT16 y)
{
	UINT8 l,h;

	if ((x <= R_CPI_MAX) && (y <= R_CPI_MAX))
	{
		h = (UINT8)(x >> 8);
		l = (UINT8)x;
		Write_Sensor(Config5_H,h);
		Write_Sensor(Config5_L,l);

		h = (UINT8)(y >> 8);
		l = (UINT8)y;
		Write_Sensor(Resolution_H,h);
		Write_Sensor(Resolution_L,l);
	}
}


//#define OP_Debug_Flag	
void OP_Debug(UINT32 dat)
{
#ifdef OP_Debug_Flag

	Tool_USB_32bitsData(dat);

#endif
}

void OP_Set_LiftOff(void)
{
	Write_Sensor(RawData_Threshold,gloabSetting.settingMode.pixThresh); 
	Write_Sensor(Min_SQ_Run,gloabSetting.settingMode.min_sq);   

	Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 
}


BOOL OP_LiftCutOff_Enter(void)
{
	UINT8 gear=0;
	UINT16 dpi_x = 0;//buffer[4];
	UINT16 dpi_y = 0;// buffer[5];
	gear=gloabSetting.settingMode.dpi_gear;
	dpi_x = gloabSetting.settingMode.dpi_class[gear].dpi_x;
	dpi_y = gloabSetting.settingMode.dpi_class[gear].dpi_y;

//OP_3389_Perform_SROM_Download(OP_3389_SROM_0X05,OP_3389_SROM_0X05_GetSize(),OP_3389_SROM_Ver);
	OP_3389_Init();
//	TimerDelayMsec(40);
	OP_3389_Set_DPI(dpi_x,dpi_y);
	return TRUE;
}

void OP_LiftCutOff_Quit(void)
{
	OP_liftCut_run.start = FALSE;

	OP_HeightRunning_Clear();

	Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 
}

void OP_LiftCutOff_Start(void)
{
	TimerDelayMsec(35);
	// Enable Calibration
	Write_Sensor(LiftCuttoff_Cla1, RUN_CAL_EN);

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
	Write_Sensor(LiftCuttoff_Cla1, RUN_DISABLE);

	OP_liftCut_run.start = FALSE;
	OP_liftCut_run.identify_desktop = FALSE;	

	OP_HeightRunning_Clear();

	Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 


}

void OP_LiftCutOff_Determine(void)
{
	////start!!!!!
	// Stop Calibration
	Write_Sensor(LiftCuttoff_Cla1, RUN_DISABLE);//!!!!!!!!!!!!!!!!!

	// Delay for 1ms
	TimerDelayMsec(1);

	// read RPTH[6:0] (0x65) and write to Pixel Threshold (0x2c)
	pix_thresh = Read_Sensor(LiftCutoff_Cal2) & 0x7F;
	// read RMSQ[7:0] (0x41) and write to Min_SQ_Run (0x2B)
	min_sq = Read_Sensor(LiftCutoff_Cal3);

	////end!!!!!


	///back the last value
	pixThresh_back = Read_Sensor(RawData_Threshold);
	min_sq_back = Read_Sensor(Min_SQ_Run);

	Write_Sensor(RawData_Threshold, pix_thresh);
	Write_Sensor(Min_SQ_Run, min_sq);

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



///The typical phenomenon of high regulation is that 
///black paper can move and white paper cannot move.!!!!!!!!!!!!!!!!!
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
	cal_status = Read_Sensor(LiftCuttoff_Cla1);

	Optical_LiftCutOff_Result(cal_status);

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
	  Write_Sensor(LiftCuttoff_Cla1, RUN_DISABLE);//!!!!!!!!!!!!!!!!!
		TimerDelayMsec(1);
		
		OP_HeightRunning_Clear();

		OP_liftCut_run.start = FALSE;
		
    Optical_LiftCutOff_Set(pixThresh_back,min_sq_back);
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

	OP_liftCut_run.start = FALSE;

	Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 

	return;
}

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
	Write_Sensor(RawData_Threshold, pix_thresh);
	Write_Sensor(Min_SQ_Run, min_sq);

	gloabSetting.settingMode.pixThresh = pix_thresh;
	gloabSetting.settingMode.min_sq = min_sq;

	OperateFlash_Store_ModeSetting(gloabSetting.setting.currentMode);
}

#endif

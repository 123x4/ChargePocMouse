#include "../inc/scroll.h"
#include "my_board.h"

//void nsAPI_WheelEventMointor(void)
WHEEL_SCAN_DATA wheelScan_stru;

BOOL Scroll_Scan(void)
{
//  static bool wheel_flag = false;
	BOOL wheel_up = FALSE;
	BOOL wheel_down = FALSE;
	UINT32 wheel_temp = 0;
	static UINT8 wheel1_time_delay = 0;
	static UINT32  last_wheel1_status = 0;
  uint8_t ScrollData[4] = {0x34,0x0b,0x38,0x07};//down xx 11 01 00,xx 00 10 11 |#~#| up xx 11 10 00,xx 00 01 11
  static  uint8_t PreWheelInput = 0;
  static  uint8_t ShiftWheel = 0;
	bool result = FALSE;
//  bool Z_P,Z_N;
  uint8_t i,wheel_value_ZA,wheel_value_ZB;
  uint8_t wheelInput = 0;
//  int8_t returnValue = 0;
  wheel_value_ZA = DrvGPIORead(ZA_PORT, ZA_PIN);
	wheel_value_ZB = DrvGPIORead(ZB_PORT, ZB_PIN);
// Rconfig pulldowns in case modified by scroll_wheel_config_sleep()
 
//  WHEEL_STROBE_OUT = 0;

  wheelInput = (((uint8_t)wheel_value_ZA << 1) | wheel_value_ZB);

	if((wheelInput^PreWheelInput)&0x03)
	{
		PreWheelInput =(wheelInput&0x03);
		ShiftWheel <<= 2;
		ShiftWheel |=PreWheelInput;
	
		for(i=0;i<4;i++)
		{
			if(ScrollData[i]==(ShiftWheel&0x3f))
			{	  
//				 wheel_flag=true;
			if((i==0)||(i==1))
			    {
//						returnValue	=-1;
				 Main_Mouse_SCROLL_UP();
					wheel_up = TRUE;
//						Main_Mouse_SCROLL_DN();
//						wheel_down = TRUE;
//				  P1DIR &= ~0x08;		
//					P1CON = 0x03;
//					LED_RED = 0; 
				}
			else
				{
//					returnValue	= 1;
//				Main_Mouse_SCROLL_UP();
//				wheel_up = TRUE;
					Main_Mouse_SCROLL_DN();
					wheel_down = TRUE;
//				P1DIR &= ~0x08;		
//				P1CON = 0x03;
//				LED_RED = 0; 
				}
			break;	
			}
		}
	}
	
	if (wheel_up == TRUE)
	{

		wheelScan_stru.wheel_status |= WHEEL_UP_REPORT_BIT; 
		wheelScan_stru.wheel_status &=  (~WHEEL_DOWN_REPORT_BIT);

	}else if (wheel_down == TRUE)
	{


		wheelScan_stru.wheel_status |= WHEEL_DOWN_REPORT_BIT; 
		wheelScan_stru.wheel_status  &= (~WHEEL_UP_REPORT_BIT);

	}else 
	{
		wheel1_time_delay++;
		if (wheel1_time_delay > 0) //3   4ms
		{
			wheel1_time_delay = 0;
			wheelScan_stru.wheel_status &= ~(WHEEL_DOWN_REPORT_BIT | WHEEL_UP_REPORT_BIT);
		}	
	}

	wheel_temp = (wheelScan_stru.wheel_status & (WHEEL_DOWN_REPORT_BIT | WHEEL_UP_REPORT_BIT));


	if (last_wheel1_status != wheel_temp)
	{
		last_wheel1_status = wheel_temp;

		result = TRUE; 

	}

	return result;

//  return returnValue;
} 

void Main_Mouse_SCROLL_UP(void)
{
    g_Main_Setting.MouseInReport[GIdx0] =0x01;
    g_Main_Setting.MouseInReport[GIdx6] = 0x01; //WHEEL_SCROLL_UP
//    g_Repot_Process.Mouse_Report_In = 1;
}
void Main_Mouse_SCROLL_DN(void)
{
    g_Main_Setting.MouseInReport[GIdx0] =0x01;
    g_Main_Setting.MouseInReport[GIdx6] = 0xFF; //WHEEL_SCROLL_DN
//    g_Repot_Process.Mouse_Report_In = 1;
}

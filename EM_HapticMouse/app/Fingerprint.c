#if 0
#include "../inc/Fingerprint.h"



void Finger_Check_ID(void)
{
	static UINT16  checkIdCycle = 0;
//	UINT8 checkCnt = 30;

	checkIdCycle++;

	if (checkIdCycle > 10) //5000
	{
		checkIdCycle = 0;
		Read_Finger(0x01);
//		while (Read_Finger(0x01) != M4_ID1)
//		{
//			TimerDelayMsec(10);
//			if (checkCnt)checkCnt--;
////			if (!checkCnt)
////			{
////				while(1);
////			}
//		}
//		if(Read_Finger(0x01) == M4_ID1)
//		{
		Write_Finger(0x28,0x01);
//		}
//		Write_Sensor(Motion_Burst,0x01);   //Not this sentence x y has a value of 0 
	}
}
#endif

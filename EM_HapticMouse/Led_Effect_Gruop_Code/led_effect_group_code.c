#include "led_effect_group_code.h"
#include "color_cycle_group.h"
#include "breathe_group.h"
#include "waves_group.h"
#include "reaction_group.h"
#include "static_group.h"
#include "waves_designer_group.h"
#include "GPIO.h"
const RGB_STRU blasoul_RGB = {0xBC,0X0C,0X51};
#if 0
const UINT8 LE_Group0[LE_Group0_LedNum] = {16,17,18,19,20,21};

const UINT8 LE_Group1[LE_Group1_LedNum] = {26,27,28,29,30,31};

const UINT8 LE_Group2[LE_Group2_LedNum] = {22,5,23};

const UINT8 LE_Group3[LE_Group3_LedNum] = {8,9,10,11,12,13,14,15};

const UINT8 LE_Group4[LE_Group4_LedNum] = {24};

const UINT8 LE_Group5[LE_Group5_LedNum] = {7};

const UINT8 LE_Group6[LE_Group6_LedNum] = {25};

const UINT8 LE_Group_LedNum[LE_Group_Num] = 
{
	LE_Group0_LedNum,
	LE_Group1_LedNum,
	LE_Group2_LedNum,
	LE_Group3_LedNum,
	LE_Group4_LedNum,
	LE_Group5_LedNum,
	LE_Group6_LedNum,
};

const UINT8 LE_All_Group[] = 
{
	16,17,18,19,20,21,26,27,28,29,30,31,22,5,23,8,9,10,11,12,13,14,15,24,7,25
};
#endif

//const UINT8 LE_Group0[LE_Group0_LedNum] = {1,2,4,5,6,7,8,18,19,};  //U Shape

//const UINT8 LE_Group1[LE_Group1_LedNum] = {9,10,11,12,13,14,15,16,20}; //Roller LED 

//const UINT8 LE_Group2[LE_Group2_LedNum] = {3}; //Logo LED

//const UINT8 LE_Group3[LE_Group3_LedNum] = {17};  //Profile LED

const UINT8 LE_Group0[LE_Group0_LedNum] = {1,2,4,5,6,7,8,18,19,9,10,11,20,12,13,14,15,16};  //U Shape

const UINT8 LE_Group1[LE_Group1_LedNum] = {3}; //Roller LED 

const UINT8 LE_Group2[LE_Group2_LedNum] = {17}; //Logo LED

//const UINT8 LE_Group3[LE_Group3_LedNum] = {17};  //Profile LED

const UINT8 LE_Group_LedNum[LE_Group_Num] = 
{
	LE_Group0_LedNum,
	LE_Group1_LedNum,
	LE_Group2_LedNum,
//	LE_Group3_LedNum,
//	LE_Group4_LedNum,
//	LE_Group5_LedNum,
//	LE_Group6_LedNum,
};

const UINT8 LE_All_Group[] = 
{
1,2,4,5,6,7,8,18,19,9,10,11,20,12,13,14,15,16,3,17 
};

LE_Group_location  LE_Group_Array[LE_Group_Num];

///for flash store and driver communicate
LE_Group_storeValue LE_G_sV;

///for FW running
LE_Group_Stru LE_G_rV;

BOOL LE_G_actionEvent;

BOOL mul_interface;
BOOL LE_G_Start;


void LedEffect_Group_Start(void)
{
	LE_G_Start = TRUE;
}


void LedEffect_Group_Stop(void)
{
	LE_G_Start = FALSE;
}

UINT8 LE_AllGroup_Size_Get(void)
{
	return sizeof(LE_All_Group);
}

void LightEffect_ActionEvent_Set(void)
{
	LE_G_actionEvent = TRUE;
}


BOOL LightEffect_ActionEvent_Get(void)
{
	return LE_G_actionEvent;
}

void LightEffect_ActionEvent_Clear(void)
{
	LE_G_actionEvent = FALSE;
}


void Led_EffectCode_Group_Pointer_Init(void)
{
	LE_Group_Array[0].groupAdr = LE_Group0;
	LE_Group_Array[0].ledNum = LE_Group0_LedNum;

	LE_Group_Array[1].groupAdr = LE_Group1;
	LE_Group_Array[1].ledNum = LE_Group1_LedNum;

	LE_Group_Array[2].groupAdr = LE_Group2;
	LE_Group_Array[2].ledNum = LE_Group2_LedNum;

//	LE_Group_Array[3].groupAdr = LE_Group3;
//	LE_Group_Array[3].ledNum = LE_Group3_LedNum;

//	LE_Group_Array[4].groupAdr = LE_Group4;
//	LE_Group_Array[4].ledNum = LE_Group4_LedNum;

//	LE_Group_Array[5].groupAdr = LE_Group5;
//	LE_Group_Array[5].ledNum = LE_Group5_LedNum;

//	LE_Group_Array[6].groupAdr = LE_Group6;
//	LE_Group_Array[6].ledNum = LE_Group6_LedNum;
}


void Led_EffectCode_Group_Assign(LE_Group_storeValue sV)
{
	LE_G_sV = sV;
} 

void Led_EffectCode_Group_Generate(void)
{
	UINT8 ii;

	RamMalloc_Init();

	mul_interface = LE_G_sV.mul_interface;

	for (ii = 0;ii < LE_Group_Num;ii++)
	{
		switch(LE_G_sV.typeCode[ii].type)
		{
		case  LE_G_CODE_ColorCycle:
			ColorCycle_Group_Generate(LE_G_sV.typeCode[ii].typeValue,&(LE_G_rV.groupArray[ii]));
			break;
		case  LE_G_CODE_Breathe:
			Breathe_Group_Generate(LE_G_sV.typeCode[ii].typeValue,&(LE_G_rV.groupArray[ii]));
			break;
		case  LE_G_CODE_Waves:
			//Waves_Group_Generate(LE_G_sV.typeCode[ii].typeValue,&(LE_G_rV.groupArray[ii]));
			Waves_Designer_Group_Generate(LE_G_sV.typeCode[ii].typeValue,&(LE_G_rV.groupArray[ii]),ii);
			break;
		case  LE_G_CODE_Reaction:
			Reaction_Group_Generate(LE_G_sV.typeCode[ii].typeValue,&(LE_G_rV.groupArray[ii]));
			break;
		case  LE_G_CODE_Static:
			Static_Group_Generate(LE_G_sV.typeCode[ii].typeValue,&(LE_G_rV.groupArray[ii]));
			break;
		default:
			break;
		}
	}
}


void Led_EffectCode_Group_Start(void)
{
	UINT8 ii;

	for (ii = 0;ii < LE_Group_Num;ii++)
	{
		switch(LE_G_sV.typeCode[ii].type)
		{
		case  LE_G_CODE_ColorCycle:
			ColorCycle_Group_Start(LE_G_sV.typeCode[ii].typeValue,&(LE_G_rV.groupArray[ii]));
			break;
		case  LE_G_CODE_Breathe:
			Breathe_Group_Start(LE_G_sV.typeCode[ii].typeValue,&(LE_G_rV.groupArray[ii]));
			break;
		case  LE_G_CODE_Waves:
			//Waves_Group_Start(LE_G_sV.typeCode[ii].typeValue,&(LE_G_rV.groupArray[ii]),ii);
			break;
		case  LE_G_CODE_Reaction:
			Reaction_Group_Start(LE_G_sV.typeCode[ii].typeValue,&(LE_G_rV.groupArray[ii]));
			break;
		case  LE_G_CODE_Static:
			Static_Group_Start(LE_G_sV.typeCode[ii].typeValue,&(LE_G_rV.groupArray[ii]));
			break;
		default:
			break;
		}	
	}

	LedEffect_Group_Start();
}

void Led_EffectCode_Group_GenetateStart(void)
{
	Led_EffectCode_Group_Generate();

	Led_EffectCode_Group_Start();
}

void LE_Group_AssignTo_LedStru(UINT8 groupIndex,RGB_STRU rgb)
{
	UINT8 ii;
	UINT8 ledIndex;

	for (ii = 0;ii < LE_Group_Array[groupIndex].ledNum;ii++)
	{
		ledIndex = LE_Group_Array[groupIndex].groupAdr[ii];

		LightEffect_AssignedLedStru(ledIndex,rgb);
	}
}

void Led_EffectCode_Group_Scan(void)
{
	UINT8 ii,kk;
	UINT8 ledIndex;
	RGB_STRU rgb;
	RGB_Event rgbEvent;
	Waves_Designer_Ret_Stru retWaves;
	Waves_Designer_Stru *wave;
	
	if (!LE_G_Start)
	{
		return;
	}

	retWaves.had = FALSE;
//  DrvGPIOOutput(0,23,0);   //for debug
	for (ii = 0;ii < LE_Group_Num;ii++)
	{
		rgbEvent.had = FALSE;
		
		switch(LE_G_sV.typeCode[ii].type)
		{
		case  LE_G_CODE_ColorCycle:
			rgbEvent = ColorCycle_Group_Scan((&LE_G_rV.groupArray[ii]));

			break;
		case  LE_G_CODE_Breathe:
			rgbEvent = Breathe_Group_Scan((&LE_G_rV.groupArray[ii]));

			break;
		case  LE_G_CODE_Waves:
			//retWaves = Waves_Group_Scan((&LE_G_rV.groupArray[ii]),ii);

			retWaves =  Waves_Designer_Group_Scan((&LE_G_rV.groupArray[ii]));

			break;
		case  LE_G_CODE_Reaction:
			rgbEvent = Reaction_Group_Scan((&LE_G_rV.groupArray[ii]));

			break;
		case  LE_G_CODE_Static:
			rgbEvent = Static_Group_Scan((&LE_G_rV.groupArray[ii]));
		
			break;
		default:
			break;
		}	
	
// if (!mul_interface && (LE_G_sV.typeCode[0].type != LE_G_CODE_Waves ))   
		if (!mul_interface && (LE_G_sV.typeCode[ii].type != LE_G_CODE_Waves ))//if no in wave and in one class interface
		{
			if (rgbEvent.had)
			{
//				for (kk = 0;kk < LE_AllGroup_Size_Get();kk++) LE_Group_LedNum[ii];
				for (kk = 0;kk < LE_Group_LedNum[ii];kk++)          //modified 20190109
				{
					if(ii==0)
					{
					ledIndex = LE_Group0[kk];//LE_All_Group[kk];
          }
					else if(ii==1)
					{
					 ledIndex = LE_Group1[kk];
					}
					else if(ii==2)
					{
					 ledIndex = LE_Group2[kk];
					}
					LightEffect_AssignedLedStru(ledIndex,rgbEvent.rgb);
				}

				LightEffect_Chang();
			}
//			break;   //modified 20190109
		}
		else //if in mul interface or in wave
		{
			if (LE_G_sV.typeCode[ii].type == LE_G_CODE_Waves)
			{
				wave = (Waves_Designer_Stru *)(LE_G_rV.groupArray[ii].runValue);
//        DrvGPIOOutput(0,23,0);
				if (retWaves.had)
				{
					for (kk = 0;kk < LE_Group_LedNum[ii];kk++)
					{
						ledIndex = retWaves.runDat[kk].ledIndex;
						rgb.red = retWaves.runDat[kk].rgb.red >> 8;
						rgb.green = retWaves.runDat[kk].rgb.green >> 8;
						rgb.blue = retWaves.runDat[kk].rgb.blue >> 8;
//            DrvGPIOOutput(0,23,0);
						rgb = Led_Effect_Transparency(wave->transparency,rgb);
//            DrvGPIOOutput(0,23,1);
						LightEffect_AssignedLedStru(ledIndex,rgb);
           
					}

					LightEffect_Chang();
				}
//         DrvGPIOOutput(0,23,1);
			}else
			{
				if (rgbEvent.had)
				{

					LE_Group_AssignTo_LedStru(ii,rgbEvent.rgb);

					LightEffect_Chang();
				}	
			}	
		}	
	}
//	DrvGPIOOutput(0,23,1); // For debug
	LightEffect_ActionEvent_Clear();
}

void Led_EffectCode_Group_Stop(void)
{
	UINT8 ii;

	for (ii = 0;ii < LE_Group_Num;ii++)
	{
		switch(LE_G_sV.typeCode[ii].type)
		{
		case  LE_G_CODE_ColorCycle:
			ColorCycle_Group_Stop(&(LE_G_rV.groupArray[ii]));
			break;
		case  LE_G_CODE_Breathe:
			Breathe_Group_Stop(&(LE_G_rV.groupArray[ii]));
			break;
		case  LE_G_CODE_Waves:
			Waves_Designer_Group_Stop(&(LE_G_rV.groupArray[ii]));
			break;
		case  LE_G_CODE_Reaction:
			Reaction_Group_Stop(&(LE_G_rV.groupArray[ii]));
			break;
		case  LE_G_CODE_Static:
			Static_Group_Stop(&(LE_G_rV.groupArray[ii]));
			break;
		default:
			break;
		}	
	}
}


RGB_STRU Led_EffectCode_Random_Color(void)
{
	UINT8 ii;
	UINT8 val;
	Ramdom_Dat threeMask[3] = {{FALSE,0},{FALSE,255},{FALSE,0xAA}};
	UINT8 resultVal[3];
	RGB_STRU rgb;

	for (ii = 0;ii < 3;ii++)
	{
		while (1)
		{
			val = Tool_Rand(0,2);

			if (threeMask[val].flag == FALSE)
			{
				threeMask[val].flag = TRUE;

				resultVal[ii] = threeMask[val].dat;

				break;
			}
		}
	}


	for (ii = 0;ii < 3;ii++)
	{
		if (resultVal[ii] == 0xAA)
		{
			val = Tool_Rand(0,255);

			resultVal[ii] = val;
		}
	}

	rgb.red = resultVal[0];
	rgb.green = resultVal[1];
	rgb.blue = resultVal[2];

	return rgb;
}


void Led_EffectCode_Update_Random_Color(RGB_STRU *rgb)
{
	*rgb = Led_EffectCode_Random_Color();
}

void Led_EffectCode_Update_Random_Color_Test(void)
{
//  RGB_STRU rgb;
//	rgb = Led_EffectCode_Random_Color();
}

//#pragma arm section code = "RAMCODE"
RGB_STRU Led_Effect_Transparency(UINT8 transparency,RGB_STRU rgb)
{
	RGB_STRU tmp;
//DrvGPIOOutput(0,23,0);
  tmp.red =  rgb.red; //* transparency / 100;
	tmp.green = rgb.green; //* transparency / 100;
	tmp.blue = rgb.blue; //* transparency / 100;
//	tmp.red =  (rgb.red * transparency )/100;
//	tmp.green = (rgb.green * transparency )/100;
//	tmp.blue = (rgb.blue * transparency)/100;
	if(tmp.red<120)
	{
	  if(tmp.green>120)
		{
		 tmp.blue=tmp.blue*(0.3);
		}
		if(tmp.green<120)
		{
		 tmp.red=tmp.red*(0.6);
		}
	}
//	else if(tmp.red>120)
//	{
//			if(tmp.green>120)
//		{
//		 tmp.blue=tmp.blue*(0.6);
//		}
//	}
//DrvGPIOOutput(0,23,1);   
    return tmp;
}
//#pragma arm section

RGB_Bigger_STRU Led_Effect_Bigger_Transparency(UINT8 transparency,RGB_Bigger_STRU rgb)
{
	RGB_Bigger_STRU tmp;

	tmp.red =  (UINT32)rgb.red * transparency / 100;
	tmp.green = (UINT32)rgb.green * transparency / 100;
	tmp.blue =(UINT32) rgb.blue * transparency / 100;

	//tmp.red =  rgb.red * transparency / 100;
	//tmp.green = rgb.green * transparency / 100;
	//tmp.blue = rgb.blue * transparency / 100;

	return tmp;
}

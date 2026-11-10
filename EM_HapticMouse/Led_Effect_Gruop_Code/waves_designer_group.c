#include "waves_designer_group.h"
#include "GPIO.h"
#if 0
const UINT8 WD_GroupInit_Index[LE_Group_Num] = 
{
	0,6,12,15,23,24,25
};


const Waves_TimerLine wd_timerLine_0[DesignerWave_TimerLine_0_Num] = 
{
	{{255,0,0},0},		
	{{255,0,255},200},
	{{0,0,255},400},	 
	{{0,255,255},600},
	{{0,255,0},800},	
	{{255,255,0},1000},
	{{255,255,0},1100},
	{{255,0,0},1300},		
};

const Waves_Run_Init wd_0_Group[DW_UseLed_Num] = 
{
	///group0
	{21,{0,32,255},426},
	{20,{76,0,255},340},
	{19,{185,0,255},226},
	{18,{255,0,216},170},
	{17,{255,0,108},86},
	{16,{255,0,0},0},

	///group1
	{31,{0,32,255},426},
	{30,{76,0,255},340},
	{29,{185,0,255},226},
	{28,{255,0,216},170},
	{27,{255,0,108},86},
	{26,{255,0,0},0},

	///group2
	{23,{255,0,0},0},
	{5,{255,0,0},0},
	{22,{255,0,0},0},

	///group3
	{8,{0,249,255},596},
	{9,{0,140,255},500},
	{10,{0,32,255},426},
	{11,{76,0,255},340},
	{12,{185,0,255},226},
	{13,{255,0,216},170},
	{14,{255,0,108},86},
	{15,{255,0,0},0},

	///group4
	{24,{0,32,255},426},

	///group5
	{7,{255,0,0},0},

	///group6
	{25,{255,0,0},0},
};


///wave effect 1
const Waves_TimerLine wd_timerLine_1[DesignerWave_TimerLine_1_Num] = 
{
/*	{{255,0,0},0},		
	{{0,128,255},240},
	{{255,0,0},345},	
	{{255,0,0},700},	*/	

	{{255,0,0},0},		
	{{0,128,255},240},
	{{0,128,255},400},
	{{255,0,0},450},	
	{{255,0,0},700},	
};

const Waves_Run_Init wd_1_Group[DW_UseLed_Num] = 
{
	///group0
	{21,{75,90,180},171},
	{20,{111,72,144},137},
	{19,{147,54,108},102},
	{18,{183,36,72},68},
	{17,{219,18,36},34},
	{16,{255,0,0},0},

	///group1
	{31,{75,90,180},171},
	{30,{111,72,144},137},
	{29,{147,54,108},102},
	{28,{183,36,72},68},
	{27,{219,18,36},34},
	{26,{255,0,0},0},


	///group2
	{23,{255,0,0},0},
	{5,{255,0,0},0},
	{22,{255,0,0},0},

	///group3
	{8,{0,128,255},240},
	{9,{39,108,216},205},
	{10,{75,90,180},171},
	{11,{111,72,144},137},
	{12,{147,54,108},102},
	{13,{183,36,72},68},
	{14,{219,18,36},34},
	{15,{255,0,0},0},


	///group4
	{24,{75,90,180},171},

	///group5
	{7,{255,0,0},0},

	///group6
	{25,{255,0,0},0},
};


///wave effect 2£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡
const Waves_TimerLine wd_timerLine_2[DesignerWave_TimerLine_2_Num] = 
{
	{{255,0,128},0},		
	{{0,0,255},240},
	{{0,0,255},650},
	{{0,255,255},750},	
	{{0,255,255},1100},	
	{{255,0,128},1200},	 
	{{255,0,128},1400},	 
};

const Waves_Run_Init wd_2_Group[DW_UseLed_Num] = 
{
	///group0
	{21,{75,0,218},171},
	{20,{111,0,200},137},
	{19,{147,0,182},102},
	{18,{183,0,164},68},	
	{17,{219,0,146},34},
	{16,{255,0,128},0},

	///group1
	{31,{75,0,218},171},
	{30,{111,0,200},137},
	{29,{147,0,182},102},
	{28,{183,0,164},68},	
	{27,{219,0,146},34},
	{26,{255,0,128},0},

	///group2
	{23,{255,0,128},0},
	{5,{255,0,128},0},
	{22,{255,0,128},0},

	///group3
	{8,{0,0,255},240},
	{9,{39,0,236},205},
	{10,{75,0,218},171},
	{11,{111,0,200},137},
	{12,{147,0,182},102},
	{13,{183,0,164},68},	
	{14,{219,0,146},34},
	{15,{255,0,128},0},

	///group4
	{24,{75,0,218},171},

	///group5
	{7,{255,0,128},0},

	///group6
	{25,{255,0,128},0},
};

///wave effect 2£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡


///wave effect 3
const Waves_TimerLine wd_timerLine_3[DesignerWave_TimerLine_3_Num] = 
{
	{{255,0,0},0},		
	{{255,128,0},240},
	{{255,128,0},450},
	{{255,0,0},550},	
	{{255,0,0},850},	
};

const Waves_Run_Init wd_3_Group[DW_UseLed_Num] = 
{
	///group0
	{21,{255,90,0},171},
	{20,{255,72,0},137},
	{19,{255,54,0},102},
	{18,{255,36,0},68},
	{17,{255,18,0},34},
	{16,{255,0,0},0},

	///group1
	{31,{255,90,0},171},
	{30,{255,72,0},137},
	{29,{255,54,0},102},
	{28,{255,36,0},68},
	{27,{255,18,0},34},
	{26,{255,0,0},0},


	///group2
	{23,{255,0,0},0},
	{5,{255,0,0},0},
	{22,{255,0,0},0},

	///group3		
	{8,{255,128,0},240},
	{9,{255,108,0},205},
	{10,{255,90,0},171},
	{11,{255,72,0},137},
	{12,{255,54,0},102},
	{13,{255,36,0},68},
	{14,{255,18,0},34},
	{15,{255,0,0},0},


	///group4
	{24,{255,90,0},171},

	///group5
	{7,{255,0,0},0},

	///group6
	{25,{255,0,0},0},
};
#endif

const UINT8 WD_GroupInit_Index[LE_Group_Num] = 
{
0,18,19
};

#if 1
const Waves_TimerLine wd_timerLine_0[DesignerWave_TimerLine_0_Num] = 
{
	{{255,0,0},0},		
	{{255,0,255},200},
	{{0,0,255},400},	 
	{{0,255,255},600},
	{{0,255,0},800},	
	{{255,255,0},1000},
	{{255,255,0},1100},
	{{255,0,0},1300},		
};

const Waves_Run_Init wd_0_Group[DW_UseLed_Num] = 
{
/*
	///group0
	{1,{0,128,255},596},
	{2,{0,140,255},500},
	{4,{0,32,255},426},
	{5,{76,0,255},340},
	{6,{185,0,255},226},
	{7,{255,0,216},170},
	{8,{255,0,152},120},
	{18,{255,0,108},86},
	{19,{255,0,0},0},

	{16,{0,128,255},596},
	{15,{0,140,255},500},
	{14,{0,32,255},426},
	{13,{76,0,255},340},
	{12,{185,0,255},226},
	{20,{255,0,216},170},
	{11,{255,0,152},120},
	{10,{255,0,108},86},
	{9,{255,0,0},0},
	
	{3,{0,32,255},426},
	
	{17,{0,32,255},426},
*/
	{1,{0,255,128},700},
	{2,{0,128,255},596},
	{4,{0,134,255},548},
	{5,{0,140,255},500},
	{6,{0,86,255},463},
	{7,{0,32,255},426},
	{8,{38,0,255},383},	
	{18,{76,0,255},340},
	{19,{131,0,255},283},
	{9,{185,0,255},226},
	{10,{255,0,252},198},
	{11,{255,0,216},170},
	{20,{255,0,185},145},
	{12,{255,0,153},120},
	{13,{255,0,131},103},
	{14,{255,0,109},86},
	{15,{255,0,55},43},
	{16,{255,0,0},0},
	
	{3,{0,32,255},426},
	
	{17,{0,32,255},426},	




/*
	{1,{0,255,128},700},
	{2,{0,128,255},596},
	{4,{0,134,255},548},
	{5,{0,140,255},500},
	{6,{0,86,255},463},
	{7,{0,32,255},426},
	{8,{38,0,255},383},	
	{18,{76,0,255},340},
	{19,{131,0,255},283},
	{9,{185,0,255},226},
	{10,{220,0,230},198},
	{11,{255,0,216},170},
	{20,{255,0,184},145},
	{12,{255,0,152},120},
	{13,{255,0,130},103},
	{14,{255,0,108},86},
	{15,{255,0,54},43},
	{16,{255,0,0},0},
	
	{3,{0,32,255},426},
	
	{17,{0,32,255},426},	
*/
/*
//...........//
	{1,{0,255,128},700},
	{2,{0,128,255},596},
	{4,{0,134,255},548},
	{5,{0,140,255},500},
	{6,{0,86,255},463},
	{7,{0,32,255},426},
	{8,{38,0,255},383},	
	{18,{76,0,255},340},
	{19,{131,0,255},283},
	{9,{185,0,255},226},
	{10,{220,0,230},198},
	{11,{255,0,216},170},
	{20,{255,0,184},145},
	{12,{255,0,152},120},
	{13,{255,0,130},103},
	{14,{255,0,108},86},
	{15,{255,0,54},43},
	{16,{255,0,0},0},
	
	{3,{0,32,255},426},
	
	{17,{0,32,255},426},	
//	{16,{0,128,255},596},
//	{15,{0,140,255},500},
//	{14,{0,32,255},426},
//	{13,{76,0,255},340},
//	{12,{185,0,255},226},
//	{20,{255,0,216},170},
//	{11,{255,0,152},120},
//	{10,{255,0,108},86},
//	{9,{255,0,0},0},
	

	
//..........//	
*/
	
//	{17,{76,0,255},340},
	
//	{9,{76,0,255},340},
//	{1,{0,32,255},426},
//	{2,{76,0,255},340},
//	{4,{185,0,255},226},
//	{5,{255,0,216},170},
//	{6,{255,0,108},86},
//	{7,{255,0,0},0},
	
	///group1
//	{16,{0,32,255},426},
//	{15,{76,0,255},340},
//	{14,{185,0,255},226},
//	{13,{255,0,216},170},
//	{12,{255,0,108},86},
//	{20,{255,0,0},0},   //change the time and RGB

	///group2
//	{14,{76,0,255},86},   //change the time and RGB
//	{15,{185,0,255},170},   //change the time and RGB
//	{16,{0,0,255},226},   //change the time and RGB

	///group3
//	{8,{0,128,255},596},
//	{18,{0,140,255},500},
//	{19,{0,32,255},426},
//	{9,{76,0,255},340},
//	{10,{185,0,255},226},
//	{11,{255,0,216},170},
//	{3,{255,0,108},86},
//	{17,{255,0,0},0},

	///group4
//	{20,{0,32,255},426},

//	///group5
//	{21,{255,0,0},0},

//	///group6
//	{25,{255,0,0},0},
};

#if 0
///wave effect 1
const Waves_TimerLine wd_timerLine_1[DesignerWave_TimerLine_1_Num] = 
{
/*	{{255,0,0},0},		
	{{0,128,255},240},
	{{255,0,0},345},	
	{{255,0,0},700},	*/	


	{{255,0,0},0},		
	{{0,128,255},240},
	{{0,128,255},400},
	{{255,0,0},450},	
	{{255,0,0},700},	
};

const Waves_Run_Init wd_1_Group[DW_UseLed_Num] = 
{
	///group0

//	{0,{75,90,180},171},
	{1,{111,72,144},137},
	{2,{147,54,108},102},
	{3,{183,36,72},68},
	{4,{219,18,36},34},
	{5,{255,0,0},0},

	///group1
	{6,{75,90,180},171},
	{7,{111,72,144},137},
	{8,{147,54,108},102},
	{9,{183,36,72},68},
	{10,{219,18,36},34},
	{11,{75,90,180},171},   //change the time


	///group2
	{12,{111,72,144},137},  //change the time
	{13,{147,54,108},102},  //change the time
	{14,{183,36,72},68},  //change the time 

	///group3
	{15,{0,128,255},240},
	{16,{39,108,216},205},
	{17,{75,90,180},171},
	{18,{111,72,144},137},
	{19,{147,54,108},102},
	{20,{183,36,72},68},
	
//	{14,{219,18,36},34},
//	{15,{255,0,0},0},


//	///group4
//	{24,{75,90,180},171},

//	///group5
//	{7,{255,0,0},0},

//	///group6
//	{25,{255,0,0},0},
};


///wave effect 2£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡
const Waves_TimerLine wd_timerLine_2[DesignerWave_TimerLine_2_Num] = 
{
	{{255,0,128},0},		
	{{0,0,255},240},
	{{0,0,255},650},
	{{0,255,255},750},	
	{{0,255,255},1100},	
	{{255,0,128},1200},	 
	{{255,0,128},1400},	 
};

const Waves_Run_Init wd_2_Group[DW_UseLed_Num] = 
{
	///group0
//	{0,{75,0,218},171},
	{1,{111,0,200},137},
	{2,{147,0,182},102},
	{3,{183,0,164},68},	
	{4,{219,0,146},34},
	{5,{255,0,128},0},  

	///group1
	{6,{75,0,218},171},
	{7,{111,0,200},137},
	{8,{147,0,182},102},
	{9,{183,0,164},68},	
	{10,{219,0,146},34},
	{11,{255,0,128},171},  //change the time 

	///group2
	{12,{255,0,128},137},  //change the time 
	{13,{255,0,128},102},  //change the time
	{14,{255,0,128},68},  //change the time 

	///group3
	{15,{0,0,255},240},
	{16,{39,0,236},205},
	{17,{75,0,218},171},
	{18,{111,0,200},137},
	{19,{147,0,182},102},
	{20,{183,0,164},68},	
//	{14,{219,0,146},34},
//	{15,{255,0,128},0},

//	///group4
//	{24,{75,0,218},171},

//	///group5
//	{7,{255,0,128},0},

//	///group6
//	{25,{255,0,128},0},
};

///wave effect 2£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡


///wave effect 3
const Waves_TimerLine wd_timerLine_3[DesignerWave_TimerLine_3_Num] = 
{
	{{255,0,0},0},		
	{{255,128,0},240},
	{{255,128,0},450},
	{{255,0,0},550},	
	{{255,0,0},850},	
};

const Waves_Run_Init wd_3_Group[DW_UseLed_Num] = 
{
	///group0
//	{0,{255,90,0},171},
	{1,{255,72,0},137},
	{2,{255,54,0},102},
	{3,{255,36,0},68},
	{4,{255,18,0},34},
	{5,{255,0,0},0},

	///group1
	{6,{255,90,0},171},
	{7,{255,72,0},137},
	{8,{255,54,0},102},
	{9,{255,36,0},68},
	{10,{255,18,0},34},
	{11,{255,90,0},171},     //change the time


	///group2
	{12,{255,72,0},137},   //change the time 
	{13,{255,54,0},102},   //change the time
	{14,{255,36,0},68},   //change the time

	///group3		
	{15,{255,128,0},240},
	{16,{255,108,0},205},
	{17,{255,90,0},171},
	{18,{255,72,0},137},
	{19,{255,54,0},102},
	{20,{255,36,0},68},
//	{14,{255,18,0},34},
//	{15,{255,0,0},0},


//	///group4
//	{24,{255,90,0},171},

//	///group5
//	{7,{255,0,0},0},

//	///group6
//	{25,{255,0,0},0},
};
#endif
#endif




//////////////////////////////////////////
const Waves_TimerLine * wd_timerLine[DW_Effect_Num] = 
{
	wd_timerLine_0,
//	wd_timerLine_1,
//	wd_timerLine_2,
//	wd_timerLine_3,
};

const Waves_Run_Init * wd_Group[DW_Effect_Num] = 
{
	wd_0_Group,
//	wd_1_Group,
//	wd_2_Group,
//	wd_3_Group,
};

const UINT8 wd_timerLine_Num[DW_Effect_Num] = 
{
	DesignerWave_TimerLine_0_Num,
//	DesignerWave_TimerLine_1_Num,
//	DesignerWave_TimerLine_2_Num,
//	DesignerWave_TimerLine_3_Num,
};


void Calcute_NextTimerLine_Index(BOOL dirct,UINT16 time,Waves_TimerLine *timerLine,UINT8 timerLine_num,Waves_Designer_Led_Stru *wave)
{
	INT8 ii;
	UINT8 timerIndex = timerLine_num - 1;

	if (dirct)//left
	{
		///search the next index
		for (ii = timerIndex;ii > 0;ii-- )
		{
			if (timerLine[ii].timer < time)
			{
				break;
			}	
		}

		if (ii < 0)
		{
			ii = timerIndex - 1;
			
			wave->timer = timerLine[timerIndex].timer;
		}

	}else
	{
		for (ii = 0;ii < timerLine_num;ii++)
		{
			if (timerLine[ii].timer > time)
			{
				break;
			}	
		}

		if (ii >= timerLine_num)
		{
			ii = 1;
			wave->timer = timerLine[0].timer;
		}
	}

	wave->timerLine_index = ii;
}

UINT16 Calculate_Speed(UINT8 distance,UINT16 changeCnt)
{
	UINT16 speed;
	UINT16 bigDistance;

	bigDistance = (UINT16)distance << 8;

	if (bigDistance)
	{
		speed = bigDistance / changeCnt;
		if (bigDistance % changeCnt != 0)
		{
			speed += 1;
		}
	}else
	{
		speed = 0;
	}

return speed;
}

void  Waves_Designer_Calculate_Speed(Waves_TimerLine *storeTimerLine,UINT8 timerlIne_num,UINT8 oneDealTmer,RGB_Bigger_STRU *speed)
{
	UINT16 changeTimer;
	UINT16 changeCnt;
	Waves_TimerLine from;
	Waves_TimerLine to;
	RGB_Bigger_STRU runSpeed;
	RGB_STRU distance;
	UINT8 ii;

	for (ii = 0;ii < (timerlIne_num - 1);ii++)
	{
		changeTimer = storeTimerLine[ii+1].timer - storeTimerLine[ii].timer;

		///calculate change count
		changeCnt = changeTimer/oneDealTmer;

		if (changeTimer % oneDealTmer != 0)
		{
			changeCnt += 1;
		}

		from = storeTimerLine[ii];

		to = storeTimerLine[ii+1];

		///calculate color distance
		distance.red = (from.RGB.red > to.RGB.red) ? (from.RGB.red - to.RGB.red) : (to.RGB.red - from.RGB.red) ;
		distance.green = (from.RGB.green > to.RGB.green) ? (from.RGB.green - to.RGB.green) : (to.RGB.green - from.RGB.green) ;
		distance.blue = (from.RGB.blue > to.RGB.blue) ? (from.RGB.blue - to.RGB.blue) : (to.RGB.blue - from.RGB.blue);

		///according color distance and change count calculate the speed
		runSpeed.red = Calculate_Speed(distance.red,changeCnt);
		runSpeed.green = Calculate_Speed(distance.green,changeCnt);
		runSpeed.blue = Calculate_Speed(distance.blue,changeCnt);

		speed[ii] = runSpeed;
	}
}


void Waves_Designer_Group_Generate(LE_Group_Value_Uin gV,Group_Property* gp,UINT8 groupNum)
{
	MALLOC_STRU mallocValue = {NULL,FALSE};
	Waves_Designer_Stru * waves;
	Waves_TimerLine * timerLine = NULL;
	Waves_Run_Init *initFrom = NULL;
	Waves_Run_Init *all_initFrom = NULL; 
	UINT8 ii;

	///init start status
	mallocValue = RamMalloc_Assign(DesignerWaves_LEN);

	if (!mallocValue.hadSpace)
	{
		return;
	}

	gp->type = LE_G_CODE_Waves;
	gp->runValue = mallocValue.address;
	gp->runValue_len = DesignerWaves_LEN;

	waves = (Waves_Designer_Stru *)(gp->runValue);

	waves->start = TRUE;
	waves->dirct = gV.waves.direct;
	waves->cycle = 0;
	waves->speedFrame = gV.waves.speed;
	waves->ledNum = LE_Group_LedNum[groupNum];
	waves->transparency =  gV.waves.transparency;

	///init timerLine
	if (gV.waves.effectIndex >= DW_Effect_Num)
	{
		return;
	}

	timerLine =(Waves_TimerLine *) wd_timerLine[gV.waves.effectIndex];
	all_initFrom = (Waves_Run_Init *)wd_Group[gV.waves.effectIndex];
	waves->lineNum = wd_timerLine_Num[gV.waves.effectIndex];

	mallocValue = RamMalloc_Assign(DesignerWaves_TimerLine_LEN * (waves->lineNum));

	if (!mallocValue.hadSpace)
	{
		return;
	}

	waves->timerLine = (Waves_TimerLine *)mallocValue.address;

	for (ii = 0;ii < waves->lineNum;ii++)
	{
		waves->timerLine[ii] = timerLine[ii];
	}

	///init timer line speed
	mallocValue = RamMalloc_Assign(DesignerWaves_Speed_LEN * (waves->lineNum - 1));

	if (!mallocValue.hadSpace)
	{
		return;
	}

	waves->speed = (RGB_Bigger_STRU *)mallocValue.address;

	//Waves_Designer_Calculate_Speed(waves->timerLine,waves->lineNum,LIGHT_EFFECT_SCAN_SPEED,waves->speed);
	Waves_Designer_Calculate_Speed(waves->timerLine,waves->lineNum,waves->speedFrame,waves->speed);

	//Tool_USB_Test_ArrayData()

	///init every led ram init  rgb value
	mallocValue = RamMalloc_Assign(DesignerWaves_led_LEN * waves->ledNum);

	if (!mallocValue.hadSpace)
	{
		return;
	}

	waves->runDat= (Waves_Designer_Led_Stru *)mallocValue.address;

	initFrom = &all_initFrom[WD_GroupInit_Index[groupNum]];
	
	for (ii = 0;ii < waves->ledNum;ii++)
	{
		waves->runDat[ii].ledIndex = initFrom[ii].index;
		waves->runDat[ii].timer = initFrom[ii].timer;

		waves->runDat[ii].rgb.red = initFrom[ii].RGB.red << 8;
		waves->runDat[ii].rgb.green = initFrom[ii].RGB.green << 8;
		waves->runDat[ii].rgb.blue = initFrom[ii].RGB.blue << 8;
	}

	///init first timerline index
	for (ii = 0;ii < waves->ledNum;ii++)
	{
		Calcute_NextTimerLine_Index(waves->dirct ,waves->runDat[ii].timer,waves->timerLine,waves->lineNum,&(waves->runDat[ii]));
	}
}



BOOL Led_Bigger_RGB_GradientData(RGB_Bigger_STRU *from,RGB_STRU to,RGB_Bigger_STRU speed)
{
	BOOL result = FALSE,one = FALSE,two = FALSE,three = FALSE;
	RGB_Bigger_STRU biggerTo;
	UINT16 offSet;

	biggerTo.red = (UINT16)to.red << 8;
	biggerTo.green = (UINT16)to.green << 8;
	biggerTo.blue = (UINT16)to.blue << 8;

	///I don't know why there's a problem with pointer processing below, 
	///so I can only write separately. !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (biggerTo.red > from->red)
	{
		offSet = ((biggerTo.red - from->red) > speed.red	)? speed.red : (biggerTo.red - from->red);
		from->red += offSet;

	}else if (biggerTo.red < from->red)
	{
		offSet = ((from->red - biggerTo.red) > speed.red	)? speed.red : (from->red - biggerTo.red);
		from->red -= offSet;

	}else
	{
		one = TRUE;
	}
	////G
	if (biggerTo.green > from->green)
	{
		offSet = ((biggerTo.green - from->green) > speed.green	)? speed.green : (biggerTo.green - from->green);
		from->green += offSet;

	}else if (biggerTo.green < from->green)
	{
		offSet = ((from->green - biggerTo.green) > speed.green	)? speed.green : (from->green - biggerTo.green);
		from->green -= offSet;

	}else
	{
		two = TRUE;
	}

	///B
	if (biggerTo.blue > from->blue)
	{
		offSet = ((biggerTo.blue - from->blue) > speed.blue	)? speed.blue : (biggerTo.blue - from->blue);
		from->blue += offSet;

	}else if (biggerTo.blue < from->blue)
	{
		offSet = ((from->blue - biggerTo.blue) > speed.blue	)? speed.blue : (from->blue - biggerTo.blue);
		from->blue -= offSet;

	}else
	{
		three = TRUE;
	}

	if (one && two && three)
	{
		result = TRUE;
	}

	return result;
}


void LightEffect_Waves_Algorithm(Waves_Designer_Stru *wave,Waves_Designer_Led_Stru *stru)
{	
	BOOL changeComplete = FALSE;
	INT8 timerLine_index;
	RGB_Bigger_STRU target_speed;
	Waves_TimerLine * timerLine = wave->timerLine; 
	RGB_Bigger_STRU *speed = wave->speed;
	UINT8 lineNum = wave->lineNum;

	Waves_TimerLine target_timerLine = timerLine[stru->timerLine_index];

	if (wave->dirct)///left
	{
		//stru->timer = (stru->timer - LIGHT_EFFECT_SCAN_SPEED > 0) ? 
		//	(stru->timer - LIGHT_EFFECT_SCAN_SPEED) : 0;

		stru->timer = (stru->timer - wave->speedFrame > 0) ? 
			(stru->timer - wave->speedFrame) : 0;

		target_speed = speed[stru->timerLine_index];
		
		changeComplete = Led_Bigger_RGB_GradientData(&(stru->rgb),target_timerLine.RGB,target_speed);

		if (changeComplete)
		{			
			if ((stru->timer) <= (target_timerLine.timer))
			{
				stru->timer = target_timerLine.timer;//new!!!!!!!!

				timerLine_index = stru->timerLine_index;
				timerLine_index--;
				if (timerLine_index < 0)
				{
					timerLine_index = lineNum - 2;

					stru->timer = timerLine[lineNum-1].timer;				
				}

				stru->timerLine_index = timerLine_index;	
			}
		}
	}else//right
	{
		//stru->timer = (stru->timer + LIGHT_EFFECT_SCAN_SPEED > timerLine[lineNum-1].timer) ? 
		//	timerLine[lineNum-1].timer : (stru->timer + LIGHT_EFFECT_SCAN_SPEED);

		stru->timer = (stru->timer + wave->speedFrame > timerLine[lineNum-1].timer) ? 
			timerLine[lineNum-1].timer : (stru->timer + wave->speedFrame);

		target_speed = speed[stru->timerLine_index - 1];

		changeComplete = Led_Bigger_RGB_GradientData(&(stru->rgb),target_timerLine.RGB,target_speed);

		if (changeComplete)
		{
			if ((stru->timer) >= (target_timerLine.timer))
			{
				stru->timer = target_timerLine.timer;//new!!!!!!!!

				///find next target timer
				timerLine_index = stru->timerLine_index;

				timerLine_index++;

				///if target signTimer overflow sum
				if (timerLine_index > (lineNum - 1))
				{
					timerLine_index = 1;

					//stru->timer = timerLine[timerLine_index].timer;

					stru->timer = 0;
				}

				stru->timerLine_index = timerLine_index;		
			}
		}
	}
}


void Waves_Designer_Group_Stop(Group_Property* gp)
{
	Waves_Designer_Stru *wave = (Waves_Designer_Stru *)gp->runValue;

	wave->start = FALSE;
}

Waves_Designer_Ret_Stru  Waves_Designer_Group_Scan(Group_Property* gp)
{
	Waves_Designer_Stru *wave = (Waves_Designer_Stru *)gp->runValue;
	Waves_Designer_Ret_Stru ret;
	UINT8 ii;

	UINT8 ledNum = wave->ledNum;

	ret.had = FALSE;
    
    if (!wave->start)
	{
		return ret;
	}

	if (wave->cycle)
	{
		wave->cycle--;
		return ret;
	}else
	{
		//wave->cycle = wave->speedFrame;
		wave->cycle = 30;
	}
	
//DrvGPIOOutput(0,23,0); 
	for (ii = 0;ii < ledNum;ii++)
	{

		
		LightEffect_Waves_Algorithm(wave,&(wave->runDat[ii]));
		
		//wave->runDat[ii].rgb = Led_Effect_Bigger_Transparency(wave->transparency,wave->runDat[ii].rgb);
	}
//DrvGPIOOutput(0,23,1); 
	
	ret.had = TRUE;

	ret.runDat = wave->runDat;

	return ret;
}

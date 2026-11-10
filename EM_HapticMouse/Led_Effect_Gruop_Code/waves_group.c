#include "waves_group.h"


//#define LE_Waves_enable 

#ifdef LE_Group_Waves_enable


const RGB_STRU waves_sevenColor_board[LE_G_Waves_Board_0] = 
{
	{255,0,0},		
	{255,0,255},
	{0,0,255},	
	{0,255,255},	
	{0,255,0},	
	{255,255,0},
};
/// Seven color L
const LE_G_Waves_Led_Stru LE_Group0_Waves_0[LE_Group0_LedNum] =
{
	{16,{255,0,0},1},
	{17,{255,0,80},1},
	{18,{255,0,160},1},
	{19,{255,0,240},1},
	{20,{190,0,255},2},
	{21,{110,0,255},2}
};

const LE_G_Waves_Led_Stru LE_Group1_Waves_0[LE_Group1_LedNum] = 
{
	{26,{255,0,0},1},
	{27,{255,0,80},1},
	{28,{255,0,160},1},
	{29,{255,0,240},1},
	{30,{190,0,255},2},
	{31,{110,0,255},2}
};

const LE_G_Waves_Led_Stru LE_Group2_Waves_0[LE_Group2_LedNum] = 
{
	{22,{0,0,0},0},
	{5,{0,0,0},0},
	{23,{0,0,0},0}
};

const LE_G_Waves_Led_Stru LE_Group3_Waves_0[LE_Group3_LedNum] = 
{
	{15,{0,50,255},3},
	{14,{30,0,255},2},
	{13,{110,0,255},2},
	{12,{190,0,255},2},
	{11,{255,0,240},1},
	{10,{255,0,160},1},
	{9,{255,0,80},1},	
	{8,{255,0,0},1}
};

const LE_G_Waves_Led_Stru LE_Group4_Waves_0[LE_Group4_LedNum] = 
{
	{24,{110,0,255},2},
};

const LE_G_Waves_Led_Stru LE_Group5_Waves_0[LE_Group5_LedNum] = 
{
	{7,{255,0,0},1}
};

const LE_G_Waves_Led_Stru LE_Group6_Waves_0[LE_Group6_LedNum] = 
{
	{25,{110,0,255},2}
};


const LE_G_Waves_Led_Stru * LE_WaveInit_0[LE_Group_Num] = 
{
	LE_Group0_Waves_0,
	LE_Group1_Waves_0,
	LE_Group2_Waves_0,
	LE_Group3_Waves_0,
	LE_Group4_Waves_0,
	LE_Group5_Waves_0,
	LE_Group6_Waves_0
};
/// End Seven color L



/// Seven color  R
const LE_G_Waves_Led_Stru LE_Group0_Waves_1[LE_Group0_LedNum] =
{
	{16,{255,0,0},5},
	{17,{255,0,80},0},
	{18,{255,0,160},0},
	{19,{255,0,240},0},
	{20,{190,0,255},1},
	{21,{110,0,255},1}
};

const LE_G_Waves_Led_Stru LE_Group1_Waves_1[LE_Group1_LedNum] = 
{
	{26,{255,0,0},5},
	{27,{255,0,80},0},
	{28,{255,0,160},0},
	{29,{255,0,240},0},
	{30,{190,0,255},1},
	{31,{110,0,255},1}
};

const LE_G_Waves_Led_Stru LE_Group2_Waves_1[LE_Group2_LedNum] = 
{
	{22,{0,0,0},0},
	{5,{0,0,0},0},
	{23,{0,0,0},0}
};

const LE_G_Waves_Led_Stru LE_Group3_Waves_1[LE_Group3_LedNum] = 
{
	{15,{0,50,255},2},
	{14,{30,0,255},1},
	{13,{110,0,255},1},
	{12,{190,0,255},1},
	{11,{255,0,240},0},
	{10,{255,0,160},0},
	{9,{255,0,80},0},
	{8,{255,0,0},5}    
};

const LE_G_Waves_Led_Stru LE_Group4_Waves_1[LE_Group4_LedNum] = 
{
	{24,{110,0,255},0},
};

const LE_G_Waves_Led_Stru LE_Group5_Waves_1[LE_Group5_LedNum] = 
{
	{7,{255,0,0},0}
};

const LE_G_Waves_Led_Stru LE_Group6_Waves_1[LE_Group6_LedNum] = 
{
	{25,{110,0,255},0}
};

const LE_G_Waves_Led_Stru * LE_WaveInit_1[LE_Group_Num] = 
{
	LE_Group0_Waves_1,
	LE_Group1_Waves_1,
	LE_Group2_Waves_1,
	LE_Group3_Waves_1,
	LE_Group4_Waves_1,
	LE_Group5_Waves_1,
	LE_Group6_Waves_1
};

///End Seven color R



const RGB_STRU waves_Blasoul_board[LE_G_Waves_Board_2] = 
{
	{0,0,0},
	{255,0,0},	
};



/// Blasoul color L
const LE_G_Waves_Led_Stru LE_Group0_Waves_2[LE_Group0_LedNum] =
{
	{16,{0,0,0},0},
	{17,{128,0,0},0},
	{18,{192,0,0},0},
	{19,{255,0,0},0},
	{20,{192,0,0},1},
	{21,{128,0,0},1}
};

const LE_G_Waves_Led_Stru LE_Group1_Waves_2[LE_Group1_LedNum] = 
{
	{26,{64,0,0},0},
	{27,{128,0,0},0},
	{28,{192,0,0},0},
	{29,{255,0,0},0},
	{30,{192,0,0},1},
	{31,{128,0,0},1}
};

const LE_G_Waves_Led_Stru LE_Group2_Waves_2[LE_Group2_LedNum] = 
{
	{22,{0,0,0},0},
	{5,{0,0,0},0},
	{23,{0,0,0},0}
};

const LE_G_Waves_Led_Stru LE_Group3_Waves_2[LE_Group3_LedNum] = 
{
	{15,{64,0,0},0},
	{14,{128,0,0},0},
	{13,{192,0,0},0},
	{12,{255,0,0},0},
	{11,{255,0,0},0},
	{10,{192,0,0},1},
	{9,{128,0,0},1},	
	{8,{64,0,0},1}
};

const LE_G_Waves_Led_Stru LE_Group4_Waves_2[LE_Group4_LedNum] = 
{
	{24,{0,0,0},2},
};

const LE_G_Waves_Led_Stru LE_Group5_Waves_2[LE_Group5_LedNum] = 
{
	{7,{0,0,0},1}
};

const LE_G_Waves_Led_Stru LE_Group6_Waves_2[LE_Group6_LedNum] = 
{
	{25,{0,0,0},2}
};


const LE_G_Waves_Led_Stru * LE_WaveInit_2[LE_Group_Num] = 
{
	LE_Group0_Waves_2,
	LE_Group1_Waves_2,
	LE_Group2_Waves_2,
	LE_Group3_Waves_2,
	LE_Group4_Waves_2,
	LE_Group5_Waves_2,
	LE_Group6_Waves_2
};
/// End Blasoul color L




const UINT8 Wavesboard_Num[] = 
{
	LE_G_Waves_Board_0,
	LE_G_Waves_Board_1,
	LE_G_Waves_Board_2,
};

void Waves_Group_Generate(LE_Group_Value_Uin gV,Group_Property* gp)
{
	MALLOC_STRU mallocValue = {NULL,FALSE};
	LE_G_Waves_Stru * waves;

	mallocValue = RamMalloc_Assign(LE_G_Waves_LEN);

	if (!mallocValue.hadSpace)
	{
		return;
	}

	gp->type = LE_G_CODE_Waves;
	gp->runValue = mallocValue.address;
	gp->runValue_len = LE_G_Waves_LEN;

	waves = (LE_G_Waves_Stru *)(gp->runValue);

	waves->start = FALSE;
}


void Waves_Group_Start(LE_Group_Value_Uin gV,Group_Property* gp,UINT8 groupNum)
{
	LE_G_Waves_Stru * waves;
	const LE_G_Waves_Led_Stru *from = NULL;
	UINT8 ii = 0,ledNum = 0;

	waves = (LE_G_Waves_Stru *)(gp->runValue);

	waves->start = TRUE;

	///init led rgb
	switch(gV.waves.effectIndex)
	{
	case 0:
		from = LE_WaveInit_0[groupNum];
		waves->board = (RGB_STRU*)waves_sevenColor_board;
		break;
	case 1:
		from = LE_WaveInit_1[groupNum];
		waves->board = (RGB_STRU*)waves_sevenColor_board;
		break;
	case 2:
		from = LE_WaveInit_2[groupNum];
		waves->board = (RGB_STRU*)waves_Blasoul_board;
		break;
	default:
		break;
	}

	ledNum = LE_Group_LedNum[groupNum];

	for (ii = 0;ii < ledNum;ii++)
	{
		waves->runDat[ii] = from[ii];
	}

	///init run rule
	waves->colorMax = Wavesboard_Num[gV.waves.effectIndex];
	waves->speed = gV.waves.speed;
	waves->dircet = gV.waves.direct;
	waves->cycleCnt = 0;
}

void Waves_Group_Stop(Group_Property* gp)
{
	LE_G_Waves_Stru * waves; 

	waves = (LE_G_Waves_Stru *)(gp->runValue);

	waves->start = FALSE;
}

void Waves_Single_Led(RGB_STRU *board,LE_G_Waves_Led_Stru *stru,UINT8 speed,UINT8 maxBoard,BOOL dirct)
{
#define  LED_WAVES_stru (*stru)

	RGB_STRU target_RGB;

	target_RGB = board[LED_WAVES_stru.colorSign];

	if (
		(Led_GradientData(&(LED_WAVES_stru.rgb.red),target_RGB.red,speed)) & 
		(Led_GradientData(&(LED_WAVES_stru.rgb.green),target_RGB.green,speed)) &
		(Led_GradientData(&(LED_WAVES_stru.rgb.blue),target_RGB.blue,speed))
		)
	{
		if (dirct)
		{
			(LED_WAVES_stru.colorSign)++;
			if (LED_WAVES_stru.colorSign > maxBoard)
			{
				LED_WAVES_stru.colorSign = 0;
			}
		}else
		{
			(LED_WAVES_stru.colorSign)--;
			if (LED_WAVES_stru.colorSign > maxBoard)
			{
				LED_WAVES_stru.colorSign = maxBoard;
			}
		}		
	}
}


Waves_Ret_Stru Waves_Group_Scan(Group_Property* gp,UINT8 groupNum)
{
	UINT8 ii;
	LE_G_Waves_Stru * waves  = (LE_G_Waves_Stru *)(gp->runValue);
	LE_G_Waves_Led_Stru * wavesDat;
	Waves_Ret_Stru ret;

	ret.had = FALSE;

	if (waves->cycleCnt)
	{
		waves->cycleCnt--;
		return ret;
	}else
	{
		waves->cycleCnt = waves->speed;
	}

	for (ii = 0;ii < LE_Group_LedNum[groupNum];ii++)
	{
		wavesDat = &(waves->runDat[ii]);

		Waves_Single_Led(waves->board,wavesDat,waves->speed,(waves->colorMax-1),waves->dircet);
	}

	ret.had = TRUE;

	ret.runDat = waves->runDat;

	return ret;
}


#endif



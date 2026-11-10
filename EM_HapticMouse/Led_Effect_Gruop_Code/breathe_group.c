#include "breathe_group.h"
#include "led_effect.h"


UINT8  Led_Cutomer_ColorIndex_Changed(LE_G_Breathe_Stru * breathe)
{
	UINT16 mask;
	UINT8 ii;
	UINT8 ledColorIndex = 0;
	UINT8 cnt = 30;

	ii = breathe->ledColor_index;

	while (1)
	{			
		mask = (0x0001 << ii);

		if (mask & breathe->customer_mask)
		{
			ledColorIndex = ii;
			breathe->ledColor_index = ii;

			break;
		}	

		ii++;

		if (ii >= Breathe_MAX_Index)
		{
			ii = 0;
		}

		///for died
		if (cnt)cnt--;
		if (!cnt)
		{
			break;
		}
	}

	breathe->ledColor_index++;

	if (breathe->ledColor_index >= Breathe_MAX_Index)
	{
		breathe->ledColor_index = 0;
	}

	return ledColorIndex;
}


void Breathe_Group_Generate(LE_Group_Value_Uin gV,Group_Property* gp)
{
	MALLOC_STRU mallocValue = {NULL,FALSE};
	LE_G_Breathe_Stru * breathe;

	mallocValue = RamMalloc_Assign(LE_G_Breathe_LEN);

	if (!mallocValue.hadSpace)
	{
		return;
	}

	gp->type = LE_G_CODE_Breathe;
	gp->runValue = mallocValue.address;
	gp->runValue_len = LE_G_Breathe_LEN;

	breathe = (LE_G_Breathe_Stru *)(gp->runValue);

	breathe->start = FALSE;
}
	

void Breathe_Group_Start(LE_Group_Value_Uin gV,Group_Property* gp)
{
	LE_G_Breathe_Stru * breathe; 
    
	UINT8 customerIndex;

	breathe = (LE_G_Breathe_Stru *)(gp->runValue);

	breathe->start = TRUE;

	breathe->ledColor_index = 0;
	breathe->customer_mask = gV.breathe.customer_mask;

	if (gV.breathe.random)
	{
		Led_EffectCode_Update_Random_Color(&(breathe->colorBoard[0]));
		Led_EffectCode_Update_Random_Color(&(breathe->colorBoard[1]));

		breathe->random = TRUE;
	}else
	{
		breathe->colorBoard[0] = (gV.breathe.rgb0);
		breathe->colorBoard[1] = (gV.breathe.rgb1);

		breathe->random = FALSE;
	}

	customerIndex = Led_Cutomer_ColorIndex_Changed(breathe);

	breathe->rgbBack = breathe->colorBoard[customerIndex];

	breathe->frameCount = 0;
	breathe->frameCycle = gV.breathe.speed;
	breathe->frameCnt = 0;

	breathe->changedColor = TRUE;	

	breathe->transparency =  gV.breathe.transparency;
}

void Breathe_Group_Stop(Group_Property* gp)
{
	LE_G_Breathe_Stru * breathe; 

	breathe = (LE_G_Breathe_Stru *)(gp->runValue);

	breathe->start = FALSE;
}



RGB_Event Led_BreatheAlgorithm(Group_Property* gp)
{
	UINT8 ratio = 0,customerIndex;
	UINT32 redTmp = 0,greenTmp = 0,blueTmp = 0;
	RGB_Event ret;
	LE_G_Breathe_Stru * breathe = (LE_G_Breathe_Stru *)(gp->runValue); 

	ret.had = TRUE;

	if (breathe->frameCnt)breathe->frameCnt--;
	if (!(breathe->frameCnt))
	{
		breathe->frameCnt = 50;
//		breathe->frameCount++;
		breathe->frameCount += breathe->frameCycle;
		if (breathe->frameCount > 187)
		{
			breathe->frameCount = 0;
		}
	}else
	{
		ret.had = FALSE;
		return ret;
	}

	if (breathe->frameCount < 64)
	{
		ratio = 255 - 4* (breathe->frameCount);

	}else  if ((breathe->frameCount > 63) & (breathe->frameCount < 94) )
	{
		ratio = 0;

		if (breathe->changedColor == TRUE)
		{
			breathe->changedColor = FALSE;

			customerIndex = Led_Cutomer_ColorIndex_Changed(breathe);

			if (breathe->random)
			{
				Led_EffectCode_Update_Random_Color(&(breathe->colorBoard[0]));
				Led_EffectCode_Update_Random_Color(&(breathe->colorBoard[1]));

				//Tool_USB_ArrayData((UINT8*)(breathe->colorBoard),6);
			}

			breathe->rgbBack = breathe->colorBoard[customerIndex];

		}

	}else if ((breathe->frameCount > 93) & (breathe->frameCount < 158))
	{
		ratio = 255 - ( 255 - 4*(breathe->frameCount-94));

	}else if (breathe->frameCount > 157)
	{
		ratio = 255;

		breathe->changedColor = TRUE;
	}

	redTmp = (UINT32)(breathe->rgbBack.red * ratio);
	greenTmp =(UINT32)(breathe->rgbBack.green * ratio);
	blueTmp = (UINT32)(breathe->rgbBack.blue * ratio);

	redTmp >>= 8;
	greenTmp >>= 8;
	blueTmp >>= 8;

	
	ret.rgb.red = redTmp;
	ret.rgb.green = greenTmp;
	ret.rgb.blue = blueTmp;



	return ret;
}

RGB_Event Breathe_Group_Scan(Group_Property* gp)
{
	RGB_Event rec = {FALSE,FALSE,{0,0,0}};

	LE_G_Breathe_Stru * breathe  = (LE_G_Breathe_Stru *)(gp->runValue);;

	if (breathe->start)
	{	
		rec = Led_BreatheAlgorithm(gp);

		rec.rgb = Led_Effect_Transparency(breathe->transparency,rec.rgb);
	}

	return rec;
}

#include "reaction.h"
#include "custom.h"

LED_REACT_STRU led_react;

//void Led_React_Stop(void)
//{
//	led_react.flag = FALSE;
//}

//void Led_React_Start(BOOL KeyPress)
//{
//	if (KeyPress)
//	{
//		led_react.led_RGB = gloabSetting.settingMode.LE_value.typeValue.reaction.rgb0;
//		//led_react.led_RGB.green = led_setting.colorBlackboard.green;
//		//led_react.led_RGB.blue = led_setting.colorBlackboard.blue;
//		led_react.status = TRUE;
//		led_react.frameCount = 0;
//		led_react.frameCycle = gloabSetting.settingMode.LE_value.typeValue.reaction.speed;
//	}
//	else
//	{
//		led_react.led_RGB.red = 0;
//		led_react.led_RGB.green = 0;
//		led_react.led_RGB.blue = 0;
//		led_react.status = FALSE;
//	}
//	led_react.flag = TRUE;

//}


//void Reaction_Generate(LE_Group_Value_Uin gV,LED_Property_Stru* gp)
//{
//	LED_REACT_STRU * reaction;

//	gp->type = LED_MODE_REACT;
//	gp->runValue = LED_rundata_ram;
//	gp->runValue_len = Reaction_LEN;

//	reaction = (LED_REACT_STRU *)(gp->runValue);

//	reaction->flag = FALSE;

//}

void Reaction_Start(LE_Group_Value_Uin gV,LED_Property_Stru* gp)
{
	LED_REACT_STRU * reaction; 

	reaction = (LED_REACT_STRU *)(gp->runValue);

	reaction->flag = TRUE;

	reaction->phase = PHASE0;
	reaction->lightness = gV.reaction.transparency;

	reaction->led_RGB[0] = gV.reaction.rgb0;
	reaction->led_RGB[1] = gV.reaction.rgb1;

	reaction->frameCycle = gV.reaction.speed;

	reaction->frameCount = 0;

	reaction->twoColor = gV.reaction.two;	

	reaction->ramdom = gV.reaction.random;

	reaction->Trg = FALSE;

//	reaction->transparency =  gV.reaction.transparency;
}

void Reaction_Stop(LED_Property_Stru* gp)
{
	LED_REACT_STRU * reaction; 

	reaction = (LED_REACT_STRU *)(gp->runValue);

	reaction->flag = FALSE;
}

//RETURN_STRUCT Led_React_Algorithm()
//{
//	UINT8 ratio = 0;
//	UINT32 redTmp = 0,greenTmp = 0,blueTmp = 0;
//	RETURN_STRUCT result = {TRUE,TRUE};

//	if (led_react.status == FALSE)
//	{
//		result.hadData = FALSE;
//		return result;
//	}
//	if (led_react.frameCnt)led_react.frameCnt--;
//	if (!(led_react.frameCnt))
//	{
//		led_react.frameCnt = led_react.frameCycle;
//		led_react.frameCount++;
//	}else
//	{
//		result.hadData = FALSE;
//		return result;
//	}

//	if (led_react.frameCount < 64)
//	{
//		ratio = 255 - 4* led_react.frameCount;
//	}
//	else
//		ratio = 0;

//	if (!ratio)
//	{
//		led_react.status = FALSE;
//	}

//	redTmp = (UINT32)(led_react.led_RGB.red * ratio);
//	greenTmp =(UINT32)(led_react.led_RGB.green * ratio);
//	blueTmp = (UINT32)(led_react.led_RGB.blue * ratio);

//	redTmp >>= 8;
//	greenTmp >>= 8;
//	blueTmp >>= 8;


//	led_RGB_Stru.red = redTmp;
//	led_RGB_Stru.green = greenTmp;
//	led_RGB_Stru.blue = blueTmp;

//	return result;
//}

//void Led_React_Scan(void)
//{
//	RETURN_STRUCT resultData = {FALSE,FALSE};


//	if ((!led_react.flag))
//	{
//		return;
//	}

//	resultData = Led_React_Algorithm();

//	if (resultData.hadData)
//	{
//		led_pwmData_change = TRUE;
//	}
//}

RGB_Event Reaction_Algorithm(LED_Property_Stru* gp)
{
	RGB_Event rec = {FALSE,FALSE,{0,0,0}};
	UINT8 ratio = 0;
	UINT16 redTmp,greenTmp,blueTmp;
	RGB_STRU rgb;


	LED_REACT_STRU * reaction = (LED_REACT_STRU *)(gp->runValue); 

	if (reaction->frameCnt)(reaction->frameCnt)--;

	if (!(reaction->frameCnt))
	{
		//(reaction->cycle) = (reaction->speed);
		(reaction->frameCnt) = 50;

	}else
	{
		return rec;
	}

	if (reaction->phase == PHASE1)
	{
//		rec.rgb = reaction->led_RGB[0];
		if (reaction->twoColor)
		{
			if (reaction->Trg)
			{
				rec.rgb = reaction->led_RGB[0];
			}else
			{
				rec.rgb = reaction->led_RGB[1];
			}
		}else
		{
			rec.rgb = reaction->led_RGB[0];
		}

		reaction->phase = PHASE2;

	}else if (reaction->phase == PHASE2)
	{
		//reaction->frame += 1;
		reaction->frameCount += reaction->frameCycle;

		if (reaction->frameCount < 64)
		{
			ratio = 255 - 4* (reaction->frameCount);

		}else if (reaction->frameCount >= 64)
		{
			ratio =  0;

			reaction->phase = PHASE0;
		}

		if (reaction->twoColor)
		{
			if (reaction->Trg)
			{
				rgb = reaction->led_RGB[0];
			}else
			{
				rgb = reaction->led_RGB[1];
			}
		}else
		{
			rgb = reaction->led_RGB[0];
		}
		//redTmp = (UINT16)(reaction->rgb0.red * ratio);
		//greenTmp =(UINT16)(reaction->rgb0.green * ratio);
		//blueTmp = (UINT16)(reaction->rgb0.blue * ratio);

		redTmp = (UINT16)(rgb.red * ratio);
		greenTmp =(UINT16)(rgb.green * ratio);
		blueTmp = (UINT16)(rgb.blue * ratio);

		redTmp >>= 8;
		greenTmp >>= 8;
		blueTmp >>= 8;

		rec.rgb.red = redTmp;
		rec.rgb.green = greenTmp;
		rec.rgb.blue = blueTmp;	
	}

	rec.had = TRUE;

	return rec;
}


//RGB_Event Reaction_Scan(LED_Property_Stru* gp)
//{
//	RGB_Event rec = {FALSE,FALSE,{0,0,0}};

//	LED_REACT_STRU * reaction  = (LED_REACT_STRU *)(gp->runValue);;

//	if (reaction->flag)
//	{
//		if ((LightEffect_ActionEvent_Get()) )
//		{
//			reaction->frameCount = 0;
//			reaction->phase = PHASE1;
//			reaction->Trg = !reaction->Trg;

//			if (reaction->ramdom)
//			{
//				Led_EffectCode_Update_Random_Color(&(reaction->led_RGB[0]));
//				Led_EffectCode_Update_Random_Color(&(reaction->led_RGB[1]));
//			}
//		}

//		rec = Reaction_Algorithm(gp);

//		rec.rgb = Led_Effect_Transparency(rec.rgb, reaction->lightness);
//	}

//	return rec;
//}



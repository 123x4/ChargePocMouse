#include "reaction_group.h"



void Reaction_Group_Generate(LE_Group_Value_Uin gV,Group_Property* gp)
{
	MALLOC_STRU mallocValue = {NULL,FALSE};
	LE_G_Reaction_Stru * reaction;

	mallocValue = RamMalloc_Assign(LE_G_Reaction_LEN);

	if (!mallocValue.hadSpace)
	{
		return;
	}

	gp->type = LE_G_CODE_Reaction;
	gp->runValue = mallocValue.address;
	gp->runValue_len = LE_G_Reaction_LEN;

	reaction = (LE_G_Reaction_Stru *)(gp->runValue);

	reaction->start = FALSE;

}


void Reaction_Group_Start(LE_Group_Value_Uin gV,Group_Property* gp)
{
	LE_G_Reaction_Stru * reaction; 

	reaction = (LE_G_Reaction_Stru *)(gp->runValue);

	reaction->start = TRUE;

	reaction->pahse = PHASE0;

	reaction->rgb0 = gV.reaction.rgb0;
	reaction->rgb1 = gV.reaction.rgb1;

	reaction->speed = gV.reaction.speed;

	reaction->frame = 0;

	reaction->twoColor = gV.reaction.two;	

	reaction->ramdom = gV.reaction.random;

	reaction->Trg = FALSE;

	reaction->transparency =  gV.reaction.transparency;
}



void Reaction_Group_Stop(Group_Property* gp)
{
	LE_G_Reaction_Stru * reaction; 

	reaction = (LE_G_Reaction_Stru *)(gp->runValue);

	reaction->start = FALSE;
}


RGB_Event Reaction_Group_Algorithm(Group_Property* gp)
{
	RGB_Event rec = {FALSE,FALSE,{0,0,0}};
	UINT8 ratio = 0;
	UINT16 redTmp,greenTmp,blueTmp;
	RGB_STRU rgb;


	LE_G_Reaction_Stru * reaction = (LE_G_Reaction_Stru *)(gp->runValue); 

	if (reaction->cycle)(reaction->cycle)--;

	if (!(reaction->cycle))
	{
		//(reaction->cycle) = (reaction->speed);
		(reaction->cycle) = 50;

	}else
	{
		return rec;
	}

	if (reaction->pahse == PHASE1)
	{
		rec.rgb = reaction->rgb0;

		reaction->pahse = PHASE2;

	}else if (reaction->pahse == PHASE2)
	{
		//reaction->frame += 1;
		reaction->frame += (reaction->speed);

		if (reaction->frame < 64)
		{
			ratio = 255 - 4* (reaction->frame);

		}else if (reaction->frame >= 64)
		{
			ratio =  0;

			reaction->pahse = PHASE0;
		}

		if (reaction->twoColor)
		{
			if (reaction->Trg)
			{
				rgb = reaction->rgb0;
			}else
			{
				rgb = reaction->rgb1;
			}
		}else
		{
			rgb = reaction->rgb0;
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


RGB_Event Reaction_Group_Scan(Group_Property* gp)
{
	RGB_Event rec = {FALSE,FALSE,{0,0,0}};

	LE_G_Reaction_Stru * reaction  = (LE_G_Reaction_Stru *)(gp->runValue);;

	if (reaction->start)
	{
		if ((LightEffect_ActionEvent_Get()) )
		{
			reaction->frame = 0;
			reaction->pahse = PHASE1;
			reaction->Trg = !reaction->Trg;

			if (reaction->ramdom)
			{
				Led_EffectCode_Update_Random_Color(&(reaction->rgb0));
				Led_EffectCode_Update_Random_Color(&(reaction->rgb1));
			}
		}

		rec = Reaction_Group_Algorithm(gp);

		rec.rgb = Led_Effect_Transparency(reaction->transparency,rec.rgb);
	}

	return rec;
}

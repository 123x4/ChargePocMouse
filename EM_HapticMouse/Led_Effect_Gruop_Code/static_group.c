#include "static_group.h"



void Static_Group_Generate(LE_Group_Value_Uin gV,Group_Property* gp)
{
	MALLOC_STRU mallocValue = {NULL,FALSE};
	LE_G_Static_Led_Stru * staticLed;

	mallocValue = RamMalloc_Assign(LE_G_Static_LEN);	

	if (!mallocValue.hadSpace)
	{
		return;
	}

	gp->type = LE_G_CODE_Static;
	gp->runValue = mallocValue.address;
	gp->runValue_len = LE_G_Static_LEN;

	staticLed = (LE_G_Static_Led_Stru *)(gp->runValue);

	staticLed->start = FALSE;

}



void Static_Group_Start(LE_Group_Value_Uin gV,Group_Property* gp)
{
	LE_G_Static_Led_Stru * staticLed; 

	staticLed = (LE_G_Static_Led_Stru *)(gp->runValue);

	staticLed->start = TRUE;

	if (gV.staticLed.random)
	{
		Led_EffectCode_Update_Random_Color(&(staticLed->rgb));
	}else
	{
		staticLed->rgb = gV.staticLed.rgb;
	}

	staticLed->transparency =  gV.staticLed.transparency;
}

void Static_Group_Stop(Group_Property* gp)
{
	LE_G_Static_Led_Stru * staticLed; 

	staticLed = (LE_G_Static_Led_Stru *)(gp->runValue);

	staticLed->start = FALSE;
}


RGB_Event Static_Group_Scan(Group_Property* gp)
{
	RGB_Event rec = {FALSE,FALSE,{0,0,0}};

	LE_G_Static_Led_Stru * staticLed  = (LE_G_Static_Led_Stru *)(gp->runValue);;

	if (staticLed->start)
	{
		staticLed->start = FALSE;

		rec.had = TRUE;

		rec.rgb = staticLed->rgb;

		rec.rgb = Led_Effect_Transparency(staticLed->transparency,rec.rgb);
	}

	return rec;
}

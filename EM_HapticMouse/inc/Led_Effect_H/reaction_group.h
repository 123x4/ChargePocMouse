#ifndef _GROUP_REACTION_
#define _GROUP_REACTION_


#include "led_effect_group_code.h"
#include "external_configuration.h"


typedef struct
{
	BOOL start;
	UINT8 frame;
	UINT8 pahse;
	RGB_STRU rgb0;
	RGB_STRU rgb1;
	UINT8 cycle;
	UINT8 speed;
	UINT8 Trg;
	BOOL twoColor; 
	BOOL ramdom;
	UINT8 transparency;
}LE_G_Reaction_Stru;

#define LE_G_Reaction_LEN	(sizeof(LE_G_Reaction_Stru))

void Reaction_Group_Generate(LE_Group_Value_Uin gV,Group_Property* gp);
void Reaction_Group_Start(LE_Group_Value_Uin gV,Group_Property* gp);
void Reaction_Group_Stop(Group_Property* gp);
RGB_Event Reaction_Group_Scan(Group_Property* gp);

#endif

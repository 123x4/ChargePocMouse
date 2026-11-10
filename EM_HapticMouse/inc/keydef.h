#ifndef _KEYDEF_H_
#define _KEYDEF_H_


#include "gloab.h"

#define ROWS 16
#define COLUMNS 8
#define GROUP_STD_MODIFY_NUM 8

#define RES 0
#define FN		0xAD
#define WIN_L	0xB3

extern const unsigned char multifuction_table[GROUP_STD_MODIFY_NUM];
extern const unsigned char keycode_table[ROWS][COLUMNS];

extern const unsigned char mouse_table[];
extern const UINT16 consumer_a_table[];
extern const UINT16 consumer_b_table[];

#endif 

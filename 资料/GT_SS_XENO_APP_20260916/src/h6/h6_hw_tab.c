/*
 * h6_hw_tab.c
 *
 *  Created on:
 *      Author:
 */

#include <string.h>

#include "h6_hw_tab.h"
#include "hasp_regmap.h"
#include "gtry_platform.h"
#include "keyboard.h"

uint8_t chain_len_buff[HASP_CHANNL_NUM_MAX] = {16, 15, 16, 15, 14, 12, 16, 0};

uint8_t g_hasp_chain_length = HASP_CHAIN_LENGTH;
uint8_t g_hasp_key_num = HASP_KEY_NUM;
uint8_t g_key_board_num = KEY_BOARD_NUM;
uint8_t g_keyboard_type = KB_TYPE_FULL_SIZE;
uint8_t g_language_type = LANG_US;
uint8_t g_product_series = PRODUCT_XENO;

uint8_t g_space_r_l = 25;
uint8_t g_space_r_r = 67;
uint8_t g_space_g_l = 46;
uint8_t g_space_g_r = 60;
uint8_t g_space_b_l = 39;
uint8_t g_space_b_r = 53;

uint8_t Key_HID_Usage_ID[HASP_KEY_NUM] = {0};

static const uint8_t k_usage_jp65[] =
{
	KEY_0,          KEY_TILDE,  KEY_CAPS,   KEY_L_CTRL,  KEY_X,         KEY_L_WIN,   KEY_CBRACE,
	KEY_MINUS,      KEY_1,      KEY_TAB,    KEY_L_SHIFT, KEY_C,         KEY_L_ALT,   KEY_OBRACE,
	KEY_PLUS,       KEY_2,      KEY_Q,      KEY_Z,       KEY_V,         KEY_K131,    KEY_P,
	KEY_K14,        KEY_3,      KEY_W,      KEY_A,       KEY_B,         KEY_SPACE,   KEY_O,
	KEY_BACKSPACE,  KEY_4,      KEY_E,      KEY_S,       KEY_N,         KEY_K132,    KEY_K,
	KEY_PAGEUP,     KEY_5,      KEY_R,      KEY_D,       KEY_M,         KEY_R_ALT,   KEY_L,
	KEY_ENTER,      KEY_6,      KEY_T,      KEY_F,       KEY_COMMA,     STEEL,       KEY_COLON,
	KEY_PAGEDOWN,   KEY_7,      KEY_Y,      KEY_G,       KEY_DOT,       KEY_LEFT,    KEY_QUOTE,
	KEY_QUESTION,   KEY_8,      KEY_U,      KEY_H,       KEY_QUESTION,  KEY_DOWN,    KEY_K42,
	KEY_UP,         KEY_9,      KEY_I,      KEY_J,       KEY_R_SHIFT,   KEY_RIGHT,   KEY_K56,
};

static const uint8_t k_usage_jp_tkl[] =
{
	KEY_L_SHIFT,  KEY_2,         KEY_Q,         KEY_L_CTRL,  KEY_Z,        KEY_L_WIN,   KEY_F8,
	KEY_CAPS,     KEY_3,         KEY_W,         KEY_A,       KEY_X,        KEY_L_ALT,   KEY_F9,
	KEY_TAB,      KEY_4,         KEY_E,         KEY_S,       KEY_C,        KEY_K131,    KEY_F10,
	KEY_1,        KEY_5,         KEY_R,         KEY_D,       KEY_V,        KEY_SPACE,   KEY_F11,
	KEY_TILDE,    KEY_6,         KEY_T,         KEY_F,       KEY_B,        KEY_K132,    KEY_F12,
	KEY_ESC,      KEY_7,         KEY_Y,         KEY_G,       KEY_N,        KEY_KANA,    KEY_PRINT,
	KEY_F1,       KEY_8,         KEY_U,         KEY_H,       KEY_M,        KEY_R_ALT,   KEY_INSERT,
	KEY_F2,       KEY_9,         KEY_I,         KEY_J,       KEY_COMMA,    STEEL,       KEY_HOME,
	KEY_F3,       KEY_0,         KEY_O,         KEY_K,       KEY_DOT,      KEY_R_CTRL,  KEY_PAGEUP,
	KEY_F4,       KEY_MINUS,     KEY_P,         KEY_L,       KEY_QUESTION, KEY_LEFT,    KEY_PAGEDOWN,
	KEY_F5,       KEY_PLUS,      KEY_OBRACE,    KEY_COLON,   KEY_K56,      KEY_DOWN,    KEY_END,
	KEY_F6,       KEY_K14,       KEY_CBRACE,    KEY_QUOTE,   KEY_R_SHIFT,  KEY_RIGHT,   KEY_DELETE,
	KEY_F7,       KEY_BACKSPACE, KEY_ENTER,     KEY_K42,     KEY_UP,       NONE,        NONE,
};

static const uint8_t k_usage_jp96[] =
{
	KEY_ESC,   KEY_TILDE,     KEY_TAB,     KEY_CAPS,      KEY_L_SHIFT, KEY_L_CTRL,       KEY_KEYPAD_1,
	KEY_F1,    KEY_1,         KEY_Q,       KEY_A,         KEY_Z,       KEY_L_WIN,        KEY_KEYPAD_2,
	KEY_F2,    KEY_2,         KEY_W,       KEY_S,         KEY_X,       KEY_L_ALT,        KEY_KEYPAD_3,
	KEY_F3,    KEY_3,         KEY_E,       KEY_D,         KEY_C,       KEY_K131,         KEY_KEYPAD_6,
	KEY_F4,    KEY_4,         KEY_R,       KEY_F,         KEY_V,       KEY_SPACE,        KEY_KEYPAD_5,
	KEY_F5,    KEY_5,         KEY_T,       KEY_G,         KEY_B,       KEY_K132,         KEY_KEYPAD_4,
	KEY_F6,    KEY_6,         KEY_Y,       KEY_H,         KEY_N,       KEY_KANA,         KEY_KEYPAD_7,
	KEY_F7,    KEY_7,         KEY_U,       KEY_J,         KEY_M,       STEEL,            KEY_KEYPAD_8,
	KEY_F8,    KEY_8,         KEY_I,       KEY_K,         KEY_COMMA,   KEY_LEFT,         KEY_KEYPAD_9,
	KEY_F9,    KEY_9,         KEY_O,       KEY_L,         KEY_DOT,     KEY_DOWN,         KEY_KEYPAD_PLUS,
	KEY_F10,   KEY_0,         KEY_P,       KEY_COLON,     KEY_QUESTION,KEY_RIGHT,        KEY_KEYPAD_MINUS,
	KEY_F11,   KEY_MINUS,     KEY_OBRACE,  KEY_QUOTE,     KEY_K56,     KEY_KEYPAD_0,     KEY_KEYPAD_MUL,
	KEY_F12,   KEY_PLUS,      KEY_CBRACE,  KEY_K42,       KEY_R_SHIFT, KEY_KEYPAD_DOT,   KEY_KEYPAD_SLASH,
	KEY_PRINT, KEY_K14,       KEY_PAGEUP,  KEY_ENTER,     KEY_UP,      KEY_KEYPAD_ENTER, KEY_KEYPAD_NUM_LOCK_AND_CLEAR,
	KEY_HOME,  KEY_BACKSPACE, NONE,        KEY_PAGEDOWN,  KEY_END,     NONE,             KEY_DELETE,
};

static const uint8_t k_usage_jp_fs[] =
{
	KEY_ESC,			KEY_2,			    KEY_Q,		    KEY_1,	  		KEY_CAPS,		  KEY_L_CTRL,			KEY_KEYPAD_MINUS,
	KEY_F1,				KEY_3,			    KEY_W,		    KEY_TILDE,   	KEY_L_SHIFT,	  KEY_L_WIN,			KEY_KEYPAD_MUL,
	KEY_F2, 			KEY_4,			    KEY_E,		    KEY_TAB,		KEY_Z,			  KEY_L_ALT,			KEY_KEYPAD_SLASH,
	KEY_F3, 			KEY_5,			    KEY_R,		    KEY_A,			KEY_X,			  KEY_K131,				KEY_KEYPAD_NUM_LOCK_AND_CLEAR,
	KEY_F4, 			KEY_6,			    KEY_T,		    KEY_S,			KEY_C,			  KEY_SPACE,			KEY_KEYPAD_7,
	KEY_F5, 			KEY_7,			    KEY_Y,		    KEY_D,			KEY_V,			  KEY_K132,				KEY_KEYPAD_8,
	KEY_F6, 			KEY_8,			    KEY_U,		    KEY_F,			KEY_B,			  KEY_KANA,				KEY_KEYPAD_9,
	KEY_F7, 			KEY_9,			    KEY_I,		    KEY_G,			KEY_N,			  KEY_R_ALT,			KEY_KEYPAD_PLUS,
	KEY_F8, 			KEY_0,			    KEY_O,		    KEY_H,			KEY_M,			  STEEL,				KEY_KEYPAD_6,
	KEY_F9, 			KEY_MINUS,  	    KEY_P,		    KEY_J,			KEY_COMMA,		  KEY_R_CTRL,			KEY_KEYPAD_5,
	KEY_F10,			KEY_PLUS,		    KEY_OBRACE,     KEY_K,			KEY_DOT,		  KEY_LEFT,				KEY_KEYPAD_4,
	KEY_F11,			KEY_K14,  		    KEY_CBRACE,		KEY_L,			KEY_QUESTION,	  KEY_DOWN,				KEY_KEYPAD_1,
	KEY_F12,			KEY_BACKSPACE,	   	KEY_DELETE,		KEY_COLON,		KEY_K56,		  KEY_RIGHT,			KEY_KEYPAD_2,
	KEY_PRINT,  		KEY_INSERT,  	    KEY_END,        KEY_QUOTE,		KEY_R_SHIFT,	  KEY_KEYPAD_0,			KEY_KEYPAD_3,
	KEY_SCROLL_LOCK, 	KEY_HOME,		    KEY_PAGEDOWN,   KEY_K42,		KEY_UP,			  NONE,	   				KEY_KEYPAD_ENTER,
	KEY_PAUSE,			KEY_PAGEUP,    	    NONE,           KEY_ENTER,		NONE,             NONE,		   	 		KEY_KEYPAD_DOT,
};

static const uint8_t k_usage_us_65[] =
{
	KEY_0,          KEY_TILDE,  KEY_CAPS,   KEY_L_SHIFT, KEY_X,         KEY_L_CTRL,  KEY_CBRACE,
	KEY_MINUS,      KEY_1,      KEY_TAB,    KEY_Z,		 KEY_C,         KEY_L_WIN,   KEY_OBRACE,
	KEY_PLUS,       KEY_2,      KEY_Q,      KEY_A,       KEY_V,         KEY_L_ALT,   KEY_P,
	KEY_BACKSPACE,  KEY_3,      KEY_W,      KEY_S,       KEY_B,         KEY_SPACE,   KEY_O,
	KEY_DELETE,  	KEY_4,      KEY_E,      KEY_D,       KEY_N,         KEY_R_ALT,   KEY_K,
	KEY_BACKSLASH,  KEY_5,      KEY_R,      KEY_F,       KEY_M,         STEEL,   	 KEY_L,
	KEY_ENTER,      KEY_6,      KEY_T,      KEY_G,       KEY_COMMA,     KEY_LEFT,    KEY_COLON,
	KEY_PAGEUP,  	KEY_7,      KEY_Y,      KEY_H,       KEY_DOT,       KEY_DOWN,    KEY_QUOTE,
	KEY_PAGEDOWN,   KEY_8,      KEY_U,      KEY_J,       KEY_QUESTION,  KEY_RIGHT,   KEY_R_SHIFT,
	KEY_UP,         KEY_9,      KEY_I,      NONE,        NONE,   		NONE,   	 NONE,
};

static const uint8_t k_usage_us_tkl[] =
{
	KEY_CAPS,  KEY_2,         KEY_Q,         KEY_A,     KEY_Z,        KEY_L_SHIFT,  KEY_F8,
	KEY_TAB,   KEY_3,         KEY_W,         KEY_S,     KEY_X,        KEY_L_CTRL,   KEY_F9,
	KEY_1,     KEY_4,         KEY_E,         KEY_D,     KEY_C,        KEY_L_WIN,    KEY_F10,
	KEY_TILDE, KEY_5,         KEY_R,         KEY_F,     KEY_V,        KEY_L_ALT,    KEY_F11,
	KEY_ESC,   KEY_6,         KEY_T,         KEY_G,     KEY_B,        KEY_SPACE,    KEY_F12,
	KEY_F1,    KEY_7,         KEY_Y,         KEY_H,     KEY_N,        KEY_R_ALT,    KEY_PRINT,
	KEY_F2,    KEY_8,         KEY_U,         KEY_J,     KEY_M,        KEY_R_WIN,    KEY_INSERT,
	KEY_F3,    KEY_9,         KEY_I,         KEY_K,     KEY_COMMA,    KEY_MENU,     KEY_HOME,
	KEY_F4,    KEY_0,         KEY_O,         KEY_L,     KEY_DOT,      KEY_R_CTRL,   KEY_PAGEUP,
	KEY_F5,    KEY_MINUS,     KEY_P,         KEY_COLON, KEY_QUESTION, KEY_LEFT,     KEY_PAGEDOWN,
	KEY_F6,    KEY_PLUS,      KEY_OBRACE,    KEY_QUOTE, KEY_R_SHIFT,  KEY_DOWN,     KEY_END,
	KEY_F7,    KEY_BACKSPACE, KEY_CBRACE,    KEY_ENTER, KEY_UP,       KEY_RIGHT,    KEY_DELETE,
	NONE,      NONE,          KEY_BACKSLASH, NONE,      NONE,         NONE,         NONE,
};

static const uint8_t k_usage_us96[] =
{
	KEY_ESC,   KEY_TILDE,     KEY_TAB,       KEY_CAPS,      KEY_L_SHIFT, KEY_L_CTRL,       KEY_KEYPAD_1,
	KEY_F1,    KEY_1,         KEY_Q,         KEY_A,         KEY_Z,       KEY_L_WIN,        KEY_KEYPAD_6,
	KEY_F2,    KEY_2,         KEY_W,         KEY_S,         KEY_X,       KEY_L_ALT,        KEY_KEYPAD_5,
	KEY_F3,    KEY_3,         KEY_E,         KEY_D,         KEY_C,       KEY_SPACE,        KEY_KEYPAD_4,
	KEY_F4,    KEY_4,         KEY_R,         KEY_F,         KEY_V,       KEY_R_ALT,        KEY_KEYPAD_7,
	KEY_F5,    KEY_5,         KEY_T,         KEY_G,         KEY_B,       STEEL,            KEY_KEYPAD_8,
	KEY_F6,    KEY_6,         KEY_Y,         KEY_H,         KEY_N,       KEY_LEFT,         KEY_KEYPAD_9,
	KEY_F7,    KEY_7,         KEY_U,         KEY_J,         KEY_M,       KEY_DOWN,         KEY_KEYPAD_PLUS,
	KEY_F8,    KEY_8,         KEY_I,         KEY_K,         KEY_COMMA,   KEY_RIGHT,        KEY_KEYPAD_MINUS,
	KEY_F9,    KEY_9,         KEY_O,         KEY_L,         KEY_DOT,     KEY_KEYPAD_0,     KEY_KEYPAD_MUL,
	KEY_F10,   KEY_0,         KEY_P,         KEY_COLON,     KEY_QUESTION,KEY_KEYPAD_DOT,   KEY_KEYPAD_SLASH,
	KEY_F11,   KEY_MINUS,     KEY_OBRACE,    KEY_QUOTE,     KEY_R_SHIFT, KEY_KEYPAD_ENTER, KEY_KEYPAD_NUM_LOCK_AND_CLEAR,
	KEY_F12,   KEY_PLUS,      KEY_CBRACE,    KEY_ENTER,     KEY_UP,      KEY_KEYPAD_3,     KEY_DELETE,
	KEY_PRINT, KEY_BACKSPACE, KEY_BACKSLASH, KEY_PAGEDOWN,  KEY_END,     KEY_KEYPAD_2,     KEY_PAGEUP,
	KEY_HOME,  NONE,          NONE,          NONE,          NONE,        NONE,             NONE,
};

static const uint8_t k_usage_us_fs[] =
{
	KEY_ESC,			KEY_2,			    KEY_Q,		     KEY_1,	  		KEY_CAPS,		  KEY_L_CTRL,	 KEY_KEYPAD_MINUS,
	KEY_F1,				KEY_3,			    KEY_W,		     KEY_TILDE,  	KEY_L_SHIFT,	  KEY_L_WIN,	 KEY_KEYPAD_MUL,
	KEY_F2, 			KEY_4,			    KEY_E,		     KEY_TAB,		KEY_Z,			  KEY_L_ALT,	 KEY_KEYPAD_SLASH,
	KEY_F3, 			KEY_5,			    KEY_R,		     KEY_A,			KEY_X,			  KEY_SPACE,	 KEY_KEYPAD_NUM_LOCK_AND_CLEAR,
	KEY_F4, 			KEY_6,			    KEY_T,		     KEY_S,			KEY_C,			  KEY_R_ALT,	 KEY_KEYPAD_7,
	KEY_F5, 			KEY_7,			    KEY_Y,		     KEY_D,			KEY_V,			  KEY_R_WIN,	 KEY_KEYPAD_8,
	KEY_F6, 			KEY_8,			    KEY_U,		     KEY_F,			KEY_B,			  STEEL,		 KEY_KEYPAD_9,
	KEY_F7, 			KEY_9,			    KEY_I,		     KEY_G,			KEY_N,			  KEY_R_CTRL,	 KEY_KEYPAD_PLUS,
	KEY_F8, 			KEY_0,			    KEY_O,		     KEY_H,			KEY_M,			  KEY_LEFT,		 KEY_KEYPAD_6,
	KEY_F9, 			KEY_MINUS,  	    KEY_P,		     KEY_J,			KEY_COMMA,		  KEY_DOWN,		 KEY_KEYPAD_5,
	KEY_F10,			KEY_PLUS,		    KEY_OBRACE,      KEY_K,			KEY_DOT,		  KEY_RIGHT,	 KEY_KEYPAD_4,
	KEY_F11,			KEY_BACKSPACE,      KEY_CBRACE,	 	 KEY_L,			KEY_QUESTION,	  KEY_KEYPAD_0,	 KEY_KEYPAD_1,
	KEY_F12,			KEY_INSERT,  	   	KEY_BACKSLASH,	 KEY_COLON,		KEY_R_SHIFT,	  NONE,			 KEY_KEYPAD_2,
	KEY_PRINT,  		KEY_HOME,		    KEY_DELETE,		 KEY_QUOTE,		KEY_UP,	  		  NONE,			 KEY_KEYPAD_3,
	KEY_SCROLL_LOCK, 	KEY_PAGEUP,         KEY_END,         KEY_ENTER,		NONE,			  NONE,	   		 KEY_KEYPAD_ENTER,
	KEY_PAUSE,			NONE,    	        KEY_PAGEDOWN,    NONE,			NONE,             NONE,		   	 KEY_KEYPAD_DOT,
};

static const uint8_t k_usage_uk_65[] =
{
	KEY_0,          KEY_TILDE,  KEY_CAPS,   KEY_L_SHIFT, KEY_X,         KEY_L_CTRL,  KEY_CBRACE,
	KEY_MINUS,      KEY_1,      KEY_TAB,    KEY_SLASH,	 KEY_C,         KEY_L_WIN,   KEY_OBRACE,
	KEY_PLUS,       KEY_2,      KEY_Q,      KEY_Z,       KEY_V,         KEY_L_ALT,   KEY_P,
	KEY_BACKSPACE,  KEY_3,      KEY_W,      KEY_A,       KEY_B,         KEY_SPACE,   KEY_O,
	KEY_DELETE,  	KEY_4,      KEY_E,      KEY_S,       KEY_N,         KEY_R_ALT,   KEY_K,
	KEY_ENTER, 		KEY_5,      KEY_R,      KEY_D,       KEY_M,         STEEL,   	 KEY_L,
	KEY_PAGEUP,     KEY_6,      KEY_T,      KEY_F,       KEY_COMMA,     KEY_LEFT,    KEY_COLON,
	KEY_PAGEDOWN,  	KEY_7,      KEY_Y,      KEY_G,       KEY_DOT,       KEY_DOWN,    KEY_QUOTE,
	KEY_UP,   		KEY_8,      KEY_U,      KEY_H,       KEY_QUESTION,  KEY_RIGHT,   KEY_K42,
	NONE,         	KEY_9,      KEY_I,      KEY_J,       NONE,   		NONE,   	 KEY_R_SHIFT,
};

static const uint8_t k_usage_uk_tkl[] =
{
	KEY_CAPS,  KEY_2,         KEY_Q,         KEY_A,             KEY_SLASH, 	 	KEY_L_SHIFT,  KEY_F8,
	KEY_TAB,   KEY_3,         KEY_W,         KEY_S,             KEY_Z,          KEY_L_CTRL,   KEY_F9,
	KEY_1,     KEY_4,         KEY_E,         KEY_D,             KEY_X,          KEY_L_WIN,    KEY_F10,
	KEY_TILDE, KEY_5,         KEY_R,         KEY_F,             KEY_C,          KEY_L_ALT,    KEY_F11,
	KEY_ESC,   KEY_6,         KEY_T,         KEY_G,             KEY_V,          KEY_SPACE,    KEY_F12,
	KEY_F1,    KEY_7,         KEY_Y,         KEY_H,             KEY_B,          KEY_R_ALT,    KEY_PRINT,
	KEY_F2,    KEY_8,         KEY_U,         KEY_J,             KEY_N,          KEY_R_WIN,    KEY_INSERT,
	KEY_F3,    KEY_9,         KEY_I,         KEY_K,             KEY_M,          KEY_MENU,     KEY_HOME,
	KEY_F4,    KEY_0,         KEY_O,         KEY_L,             KEY_COMMA,      KEY_R_CTRL,   KEY_PAGEUP,
	KEY_F5,    KEY_MINUS,     KEY_P,         KEY_COLON,         KEY_DOT,        KEY_LEFT,     KEY_PAGEDOWN,
	KEY_F6,    KEY_PLUS,      KEY_OBRACE,    KEY_QUOTE,         KEY_QUESTION,   KEY_DOWN,     KEY_END,
	KEY_F7,    KEY_BACKSPACE, KEY_CBRACE,    KEY_BACKSLASH, 	KEY_R_SHIFT,    KEY_RIGHT,    KEY_DELETE,
	NONE,      NONE,          KEY_ENTER, 	 NONE,        	    KEY_UP,         NONE,         NONE,
};

static const uint8_t k_usage_uk96[] =
{
	KEY_ESC,   KEY_TILDE,     KEY_TAB,       KEY_CAPS,      KEY_L_SHIFT,  KEY_L_CTRL,       KEY_KEYPAD_1,
	KEY_F1,    KEY_1,         KEY_Q,         KEY_A,         KEY_SLASH,    KEY_L_WIN,        KEY_KEYPAD_6,
	KEY_F2,    KEY_2,         KEY_W,         KEY_S,         KEY_Z,        KEY_L_ALT,        KEY_KEYPAD_5,
	KEY_F3,    KEY_3,         KEY_E,         KEY_D,         KEY_X,        KEY_SPACE,        KEY_KEYPAD_4,
	KEY_F4,    KEY_4,         KEY_R,         KEY_F,         KEY_C,        KEY_R_ALT,        KEY_KEYPAD_7,
	KEY_F5,    KEY_5,         KEY_T,         KEY_G,         KEY_V,        STEEL,            KEY_KEYPAD_8,
	KEY_F6,    KEY_6,         KEY_Y,         KEY_H,         KEY_B,        KEY_LEFT,         KEY_KEYPAD_9,
	KEY_F7,    KEY_7,         KEY_U,         KEY_J,         KEY_N,        KEY_DOWN,         KEY_KEYPAD_PLUS,
	KEY_F8,    KEY_8,         KEY_I,         KEY_K,         KEY_M,        KEY_RIGHT,        KEY_KEYPAD_MINUS,
	KEY_F9,    KEY_9,         KEY_O, 		 KEY_L,         KEY_COMMA,    KEY_KEYPAD_0,     KEY_KEYPAD_MUL,
	KEY_F10,   KEY_0,         KEY_P, 		 KEY_COLON,     KEY_DOT,      KEY_KEYPAD_DOT,   KEY_KEYPAD_SLASH,
	KEY_F11,   KEY_MINUS,     KEY_OBRACE, 	 KEY_QUOTE,     KEY_QUESTION, KEY_KEYPAD_ENTER, KEY_KEYPAD_NUM_LOCK_AND_CLEAR,
	KEY_F12,   KEY_PLUS,      KEY_CBRACE,    KEY_K42,     	KEY_R_SHIFT,  KEY_KEYPAD_3,     KEY_DELETE,
	KEY_PRINT, KEY_BACKSPACE, KEY_ENTER,     KEY_PAGEDOWN,  KEY_UP,       KEY_KEYPAD_2,     KEY_PAGEUP,
	KEY_HOME,  NONE,          NONE,			 NONE,          KEY_END,      NONE,             NONE,
};

static const uint8_t k_usage_uk_fs[] =
{
	KEY_ESC,			KEY_2,			    KEY_Q,		     KEY_1,	  		KEY_CAPS,		  KEY_L_CTRL,	 KEY_KEYPAD_MINUS,
	KEY_F1,				KEY_3,			    KEY_W,		     KEY_TILDE,   	KEY_L_SHIFT,	  KEY_L_WIN,	 KEY_KEYPAD_MUL,
	KEY_F2, 			KEY_4,			    KEY_E,		     KEY_TAB,		KEY_SLASH,	  	  KEY_L_ALT,	 KEY_KEYPAD_SLASH,
	KEY_F3, 			KEY_5,			    KEY_R,		     KEY_A,			KEY_Z,			  KEY_SPACE,	 KEY_KEYPAD_NUM_LOCK_AND_CLEAR,
	KEY_F4, 			KEY_6,			    KEY_T,		     KEY_S,			KEY_X,			  KEY_R_ALT,	 KEY_KEYPAD_7,
	KEY_F5, 			KEY_7,			    KEY_Y,		     KEY_D,			KEY_C,			  KEY_R_WIN,	 KEY_KEYPAD_8,
	KEY_F6, 			KEY_8,			    KEY_U,		     KEY_F,			KEY_V,			  STEEL,		 KEY_KEYPAD_9,
	KEY_F7, 			KEY_9,			    KEY_I,		     KEY_G,			KEY_B,			  KEY_R_CTRL,	 KEY_KEYPAD_PLUS,
	KEY_F8, 			KEY_0,			    KEY_O,		     KEY_H,			KEY_N,			  KEY_LEFT,		 KEY_KEYPAD_6,
	KEY_F9, 			KEY_MINUS,  	    KEY_P,		     KEY_J,			KEY_M,			  KEY_DOWN,		 KEY_KEYPAD_5,
	KEY_F10,			KEY_PLUS,		    KEY_OBRACE,      KEY_K,			KEY_COMMA,		  KEY_RIGHT,	 KEY_KEYPAD_4,
	KEY_F11,			KEY_BACKSPACE,      KEY_CBRACE,	 	 KEY_L,			KEY_DOT,		  KEY_KEYPAD_0,	 KEY_KEYPAD_1,
	KEY_F12,			KEY_INSERT,  	    KEY_DELETE,		 KEY_COLON,		KEY_QUESTION,	  NONE,			 KEY_KEYPAD_2,
	KEY_PRINT,  		KEY_HOME,		    KEY_END,      	 KEY_QUOTE,		KEY_R_SHIFT,	  NONE,			 KEY_KEYPAD_3,
	KEY_SCROLL_LOCK, 	KEY_PAGEUP,         KEY_PAGEDOWN,    KEY_BACKSLASH,	KEY_UP,	  		  NONE,	   		 KEY_KEYPAD_ENTER,
	KEY_PAUSE,			NONE,    	        NONE,            KEY_ENTER,		NONE,             NONE,		   	 KEY_KEYPAD_DOT,
};

//const layout_cfg_t k_layouts[] =
//{
//	{ k_usage_jp65,   sizeof(k_usage_jp65),   10,  70,  70,  {10, 10, 10, 10, 10, 10, 10, 0}, 25, 46, 18, 39, 11, 32 },
//	{ k_usage_jp_tkl, sizeof(k_usage_jp_tkl), 13,  91,  89,  {13, 13, 13, 13, 13, 12, 12, 0}, 11, 53, 32, 46, 25, 39 },
//	{ k_usage_jp96,   sizeof(k_usage_jp96),   15, 105, 103,  {15, 15, 14, 15, 15, 14, 15, 0}, 39, 60, 32, 53, 25, 46 },
//	{ k_usage_jp_fs,  sizeof(k_usage_jp_fs),  16, 112, 108,  {16, 16, 15, 16, 15, 14, 16, 0}, 25, 67, 46, 60, 39, 53 },
//	{ k_usage_us_65,  sizeof(k_usage_us_65),  10,  70,  66,  {10, 10, 10,  9,  9,  9,  9, 0}, 25, 46, 18, 39, 11, 32 },
//	{ k_usage_us_tkl, sizeof(k_usage_us_tkl), 13,  91,  85,  {12, 12, 13, 12, 12, 12, 12, 0}, 11, 53, 32, 46, 25, 39 },
//	{ k_usage_us96,   sizeof(k_usage_us96),   15, 105,  99,  {15, 14, 14, 14, 14, 14, 14, 0}, 39, 60, 32, 53, 25, 46 },
//	{ k_usage_us_fs,  sizeof(k_usage_us_fs),  16, 112, 104,  {16, 15, 16, 15, 14, 12, 16, 0}, 25, 67, 46, 60, 39, 53 },
//	{ k_usage_uk_65,  sizeof(k_usage_uk_65),  10,  70,  67,  { 9, 10, 10, 10,  9,  9, 10, 0}, 25, 46, 18, 39, 11, 32 },
//	{ k_usage_uk_tkl, sizeof(k_usage_uk_tkl), 13,  91,  86,  {12, 12, 13, 12, 13, 12, 12, 0}, 18, 53, 39, 53, 32, 46 },
//	{ k_usage_uk96,   sizeof(k_usage_uk96),   15, 105, 100,  {15, 14, 14, 14, 15, 14, 14, 0}, 46, 67, 39, 60, 32, 53 },
//	{ k_usage_uk_fs,  sizeof(k_usage_uk_fs),  16, 112, 105,  {16, 15, 15, 16, 15, 12, 16, 0}, 32, 74, 53, 67, 46, 60 },
//};
/*--------------------------------------------------------------------+
 * Layout configuration array
 *
 * Index = language_type * 4 + keyboard_type
 *   JP: [0]=65, [1]=TKL, [2]=96,  [3]=FS
 *   US: [4]=65, [5]=TKL, [6]=96,  [7]=FS
 *   UK: [8]=65, [9]=TKL, [10]=96, [11]=FS
 *--------------------------------------------------------------------*/
const layout_cfg_t k_layouts[] = {
	/* ---- JP (LANG_JP = 2, base = 2*4 = 8 ¡ú remapped to 0) ---- */
	/* [0] JP_65 */
	{ k_usage_jp65,     sizeof(k_usage_jp65),   JP65_MAX_CHAIN_LENGTH,    JP65_MAX_HASP_NUM,    JP65_KEY_BOARD_NUM,
	  JP65_CHAIN_LEN,   JP65_R_L,   JP65_R_R,   JP65_G_L,    JP65_G_R,    JP65_B_L,    JP65_B_R },
	/* [1] JP_TKL */
	{ k_usage_jp_tkl,   sizeof(k_usage_jp_tkl), JP_TKL_MAX_CHAIN_LENGTH,  JP_TKL_MAX_HASP_NUM,  JP_TKL_KEY_BOARD_NUM,
	  JP_TKL_CHAIN_LEN, JP_TKL_R_L, JP_TKL_R_R, JP_TKL_G_L,  JP_TKL_G_R,  JP_TKL_B_L,  JP_TKL_B_R },
	/* [2] JP_96 */
	{ k_usage_jp96,     sizeof(k_usage_jp96),   JP96_MAX_CHAIN_LENGTH,    JP96_MAX_HASP_NUM,    JP96_KEY_BOARD_NUM,
	  JP96_CHAIN_LEN,   JP96_R_L,   JP96_R_R,   JP96_G_L,    JP96_G_R,    JP96_B_L,    JP96_B_R },
	/* [3] JP_FS */
	{ k_usage_jp_fs,     sizeof(k_usage_jp_fs),   JP_FS_MAX_CHAIN_LENGTH,   JP_FS_MAX_HASP_NUM,   JP_FS_KEY_BOARD_NUM,
	  JP_FS_CHAIN_LEN,  JP_FS_R_L,  JP_FS_R_R,  JP_FS_G_L,   JP_FS_G_R,   JP_FS_B_L,   JP_FS_B_R },

	/* ---- US (LANG_US = 0, base = 0*4 = 0 ¡ú remapped to 4) ---- */
	/* [4] US_65 */
	{ k_usage_us_65,   sizeof(k_usage_us_65),   US_65_MAX_CHAIN_LENGTH,   US_65_MAX_HASP_NUM,  US_65_KEY_BOARD_NUM,
	  US_65_CHAIN_LEN, US_65_R_L, US_65_R_R, 	US_65_G_L,  US_65_G_R,    US_65_B_L,  US_65_B_R },
	/* [5] US_TKL */
	{ k_usage_us_tkl,   sizeof(k_usage_us_tkl), US_TKL_MAX_CHAIN_LENGTH,  US_TKL_MAX_HASP_NUM,  US_TKL_KEY_BOARD_NUM,
	  US_TKL_CHAIN_LEN, US_TKL_R_L, US_TKL_R_R, US_TKL_G_L,  US_TKL_G_R,  US_TKL_B_L,  US_TKL_B_R },
	/* [6] US_96 */
	{ k_usage_us96,     sizeof(k_usage_us96),   US96_MAX_CHAIN_LENGTH,    US96_MAX_HASP_NUM,    US96_KEY_BOARD_NUM,
	  US96_CHAIN_LEN,   US96_R_L,   US96_R_R,   US96_G_L,    US96_G_R,    US96_B_L,    US96_B_R },
	/* [7] US_FS */
	{ k_usage_us_fs,     sizeof(k_usage_us_fs),   US_FS_MAX_CHAIN_LENGTH,   US_FS_MAX_HASP_NUM,   US_FS_KEY_BOARD_NUM,
	  US_FS_CHAIN_LEN,  US_FS_R_L,  US_FS_R_R,  US_FS_G_L,   US_FS_G_R,   US_FS_B_L,   US_FS_B_R },

	/* ---- UK (LANG_UK = 1, base = 1*4 = 4 ¡ú remapped to 8) ---- */
	/* [8]  UK_65 */
	{ k_usage_uk_65,     sizeof(k_usage_uk_65), UK_65_MAX_CHAIN_LENGTH,   UK_65_MAX_HASP_NUM,   UK_65_KEY_BOARD_NUM,
	  UK_65_CHAIN_LEN,  UK_65_R_L,  UK_65_R_R,  UK_65_G_L,   UK_65_G_R,   UK_65_B_L,   UK_65_B_R },
	/* [9]  UK_TKL */
	{ k_usage_uk_tkl,   sizeof(k_usage_uk_tkl), UK_TKL_MAX_CHAIN_LENGTH,  UK_TKL_MAX_HASP_NUM,  UK_TKL_KEY_BOARD_NUM,
	  UK_TKL_CHAIN_LEN, UK_TKL_R_L, UK_TKL_R_R, UK_TKL_G_L,  UK_TKL_G_R,  UK_TKL_B_L,  UK_TKL_B_R },
	/* [10] UK_96  */
	{ k_usage_uk96,     sizeof(k_usage_uk96),   UK96_MAX_CHAIN_LENGTH,    UK96_MAX_HASP_NUM,    UK96_KEY_BOARD_NUM,
	  UK96_CHAIN_LEN,   UK96_R_L,  UK96_R_R,    UK96_G_L,    UK96_G_R,    UK96_B_L,   UK96_B_R },
	/* [11] UK_FS */
	{ k_usage_uk_fs,     sizeof(k_usage_uk_fs),   UK_FS_MAX_CHAIN_LENGTH,   UK_FS_MAX_HASP_NUM,   UK_FS_KEY_BOARD_NUM,
	  UK_FS_CHAIN_LEN,  UK_FS_R_L,  UK_FS_R_R,  UK_FS_G_L,   UK_FS_G_R,   UK_FS_B_L,   UK_FS_B_R },
};

uint8_t chain_miss_buff_reg[HASP_KEY_NUM];
uint8_t chain_miss_buff_led[HASP_KEY_NUM];

void calc_chain_miss_buff_reg(void)
{
	memset(chain_miss_buff_reg, 0, sizeof(chain_miss_buff_reg));
	for (uint16_t i = 0; i < g_hasp_key_num; i++)
	{
		chain_miss_buff_reg[i] = (Key_HID_Usage_ID[i] != NONE) ? 1 : 0;
	}
}

void calc_chain_miss_buff_led(void)
{
	memset(chain_miss_buff_led, 0, sizeof(chain_miss_buff_led));
	for (uint8_t col = 0; col < HASP_CHANNL_NUM; col++)
	{
		for (uint8_t row = 0; row < g_hasp_chain_length; row++)
		{
			uint8_t rev_row = g_hasp_chain_length - 1U - row;
			chain_miss_buff_led[row * HASP_CHANNL_NUM + col] =
				chain_miss_buff_reg[rev_row * HASP_CHANNL_NUM + col];
		}
	}
}

void h6_hal_fill_crc_null_hall(void)
{
	for (uint16_t i = 0; i < g_hasp_key_num; i++)
	{
		if (chain_miss_buff_led[i] == 0U)
		{
			GTRY_HASP->TXDATA[i] = 0x5555;
		}
	}
}

#if 1
uint16_t Hall_Linearization_table[LINE_SECTION] =
{
		0   ,
		2   ,
		4   ,
		7   ,
		11  ,
		15  ,
		20  ,
		24  ,
		27  ,
		33  ,
		36  ,
		40  ,
		45  ,
		49  ,
		55  ,
		58  ,
		62  ,
		67  ,
		71  ,
		76  ,
		80  ,
		85  ,
		89  ,
		93  ,
		98  ,
		104 ,
		107 ,
		113 ,
		118 ,
		122 ,
		127 ,
		131 ,
		136 ,
		142 ,
		145 ,
		151 ,
		156 ,
		160 ,
		165 ,
		171 ,
		176 ,
		180 ,
		185 ,
		191 ,
		196 ,
		200 ,
		206 ,
		211 ,
		216 ,
		222 ,
		227 ,
		233 ,
		238 ,
		244 ,
		249 ,
		255 ,
		260 ,
		266 ,
		271 ,
		276 ,
		282 ,
		287 ,
		293 ,
		298 ,
		304 ,
		309 ,
		315 ,
		322 ,
		327 ,
		333 ,
		338 ,
		344 ,
		349 ,
		356 ,
		362 ,
		367 ,
		373 ,
		380 ,
		386 ,
		393 ,
		398 ,
		404 ,
		411 ,
		416 ,
		424 ,
		429 ,
		435 ,
		442 ,
		447 ,
		455 ,
		460 ,
		467 ,
		473 ,
		480 ,
		486 ,
		493 ,
		500 ,
		506 ,
		513 ,
		520 ,
		526 ,
		533 ,
		540 ,
		546 ,
		553 ,
		560 ,
		567 ,
		573 ,
		580 ,
		587 ,
		595 ,
		602 ,
		609 ,
		615 ,
		622 ,
		629 ,
		637 ,
		644 ,
		651 ,
		658 ,
		666 ,
		673 ,
		680 ,
		689 ,
		697 ,
		704 ,
		711 ,
		718 ,
		726 ,
		733 ,
		740 ,
		749 ,
		757 ,
		764 ,
		773 ,
		780 ,
		789 ,
		797 ,
		806 ,
		813 ,
		822 ,
		829 ,
		837 ,
		846 ,
		855 ,
		862 ,
		871 ,
		880 ,
		889 ,
		897 ,
		906 ,
		915 ,
		922 ,
		931 ,
		940 ,
		949 ,
		957 ,
		966 ,
		975 ,
		984 ,
		993 ,
		1002,
		1011,
		1020,
		1029,
		1038,
		1048,
		1058,
		1068,
		1077,
		1088,
		1097,
		1106,
		1117,
		1126,
		1135,
		1146,
		1155,
		1166,
		1175,
		1186,
		1195,
		1206,
		1217,
		1228,
		1237,
		1248,
		1258,
		1269,
		1280,
		1291,
		1302,
		1313,
		1324,
		1335,
		1346,
		1357,
		1368,
		1379,
		1389,
		1402,
		1413,
		1424,
		1437,
		1448,
		1459,
		1471,
		1484,
		1495,
		1506,
		1519,
		1531,
		1544,
		1557,
		1569,
		1580,
		1593,
		1606,
		1619,
		1631,
		1644,
		1657,
		1669,
		1682,
		1697,
		1709,
		1724,
		1737,
		1749,
		1764,
		1777,
		1790,
		1804,
		1819,
		1833,
		1848,
		1860,
		1877,
		1890,
		1906,
		1920,
		1935,
		1951,
		1966,
		1980,
		1995,
		2011,
		2026,
		2042,
		2059,
		2073,
		2090,
		2104,
		2122,
		2137,
		2153,
		2170,
		2186,
		2204,
		2221,
		2239,
		2253,
		2271,
		2290,
		2306,
		2324,
		2342,
		2361,
		2379,
		2397,
		2415,
		2433,
		2451,
		2471,
		2490,
		2508,
		2528,
		2546,
		2568,
		2588,
		2606,
		2626,
		2648,
		2668,
		2688,
		2708,
		2730,
		2752,
		2773,
		2793,
		2817,
		2837,
		2861,
		2881,
		2904,
		2928,
		2950,
		2973,
		2999,
		3021,
		3046,
		3070,
		3095,
		3119,
		3144,
		3170,
		3195,
		3223,
		3250,
		3275,
		3303,
		3330,
		3357,
		3386,
		3414,
		3443,
		3472,
		3499,
		3530,
		3561,
		3590,
		3621,
		3652,
		3684,
		3715,
		3746,
		3779,
		3812,
		3843,
		3877,
		3912,
		3946,
		3981,
		4015,
		4050,
		4086,
		4121,
		4159,
		4194,
		4232,
		4268,
		4305,
		4343,
		4381,
		4419,
		4457,
		4496,
		4534,
		4574,
		4616,
		4656,
		4694,
		4736,
		4776,
		4817,
		4857,
		4898,
		4939,
		4981,
		5023,
		5063,
		5105,
		5147,
		5190,
		5232,
		5276,
		5321,
		5361,
		5405,
		5447,
		5490,
		5534,
		5579,
		5625,
		5669,
		5714,
		5758,
		5805,
		5850,
		5896,
		5941,
		5989,
		6034,
		6080,
		6125,
		6172,
		6218,
		6263,
		6311,
		6358,
		6405,
		6452,
		6502,
		6534,
		6563,
		6592,
		6622,
		6652,
		6682,
	//	6712,
		8191,
};
#endif

const uint32_t sck_pins[HASP_CHANNL_NUM_MAX] = {HASP_CHAIN0_SCK, HASP_CHAIN1_SCK, HASP_CHAIN2_SCK, HASP_CHAIN3_SCK,
												HASP_CHAIN4_SCK, HASP_CHAIN5_SCK, HASP_CHAIN6_SCK, HASP_CHAIN7_SCK};
const uint32_t sdo_pins[HASP_CHANNL_NUM_MAX] = {HASP_CHAIN0_SDO, HASP_CHAIN1_SDO, HASP_CHAIN2_SDO, HASP_CHAIN3_SDO,
												HASP_CHAIN4_SDO, HASP_CHAIN5_SDO, HASP_CHAIN6_SDO, HASP_CHAIN7_SDO};
const uint32_t sdi_pins[HASP_CHANNL_NUM_MAX] = {HASP_CHAIN0_SDI, HASP_CHAIN1_SDI, HASP_CHAIN2_SDI, HASP_CHAIN3_SDI,
												HASP_CHAIN4_SDI, HASP_CHAIN5_SDI, HASP_CHAIN6_SDI, HASP_CHAIN7_SDI};

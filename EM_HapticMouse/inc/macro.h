#ifndef _MACRO_H_
#define _MACRO_H_


#include "gloab.h"
#include "test.h"
#include "usb.h"
#include "custom.h"
#include "operation_flash.h"



#define MACRO_NUM			80//55//80//200
#define MACRO_BLOCK_SIZE	128//300//128
//#define MACRO_SPACE			10240//16500//10240//25600//10k
#define MACRO_SPACE			2048//16500//10240//25600//10k

//#define MACRO_ERASE_BLOCK(num)						Macro_Earse_DataBlock(num,MACRO_BLOCK_SIZE)
//#define MACRO_WRITE_BLOCK_BYTES(address,p_buffer,len)	MacrodataROM_bE2Write(address,p_buffer,len,25)
//#define MACRO_READ_BLOCK_BYTES(address,p_buffer,len)	MacrodataROM_E2Read(address,p_buffer,len)

#define MACRO_ERASE_BLOCK(num)							//SPI_Flash_Erase_Sector(num)
#define MACRO_WRITE_BLOCK_BYTES(address,p_buffer,len)	OperateFlash_Write_MacroData(address,p_buffer,len)
#define MACRO_READ_BLOCK_BYTES(address,p_buffer,len)	OperateFlash_Read_MacroData(address,p_buffer,len)


#define MACRO_WRITE_HEAD_BYTES(address,p_buffer,len)	OperateFlash_Write_MacroHeart(address,p_buffer,len)
#define MACRO_READ_HEAD_BYTES(address,p_buffer,len)		OperateFlash_Read_MacroHeart(address,p_buffer,len)
			


#define MACRO_HEAD_STRU_LEN	(sizeof(MACRO_HEAD_PACKET_STRU))
#define MACRO_HEAD_SPACE ( MACRO_NUM * MACRO_HEAD_STRU_LEN )  // size 200*8==1600

#define MACRO_NAME_LENGTH	30

#define MACRO_INFOR_REPORT_MAX_LEN	56




typedef struct
{
	UINT16 id;
	UINT32 len;
	UINT16 CRC;
}MACRO_HEAD_PACKET_STRU;

typedef struct
{
	UINT16 byte_writeIndex;
	UINT8 currenBlock;
}MACRO_DOWNLOAD_STRU;

typedef struct
{
	UINT16 id;
	UINT32 reportIndex;
	UINT32 reportNum;
	UINT16 packetIndex;
	UINT16 packetNum;
	UINT8 blockIndex;
}MACRO_REPORT_STRU;

typedef struct
{
	BOOL send;
	UINT16 byteIndex;
	UINT8  blockIndex;
	UINT16 byteIndex_inBlock;
	UINT8 macroPro;
	UINT8 sendProCnt;
	MACRO_HEAD_PACKET_STRU macroHead;
}MACRO_Data_STRU;

typedef union{
	struct{
		unsigned char timer_Hi;
		unsigned char timer_Lo;
		unsigned char reserved1;
		unsigned char reserved2;
	}delay_time;
	struct{
		unsigned char key_index;
		unsigned char reserved0;
		unsigned char reserved1;
		unsigned char reserved2;
	}key;
	struct{
		unsigned char x_h;
		unsigned char x_l;
		unsigned char y_h;
		unsigned char y_l;
	}location;
}MACRO_VALUE;

typedef struct{
	unsigned char type;
	MACRO_VALUE event_value; 
}MACRO_EVENT;

typedef struct{
	UINT8 key_code;
	UINT8 key_status;
}KEYBOARD_KEY_TYPE;

enum
{
	MACRO_PACKET_START = 1,
	MACRO_PACKET_STOP = 0xff,
};

enum
{
	MACRO_INFOR_MACRO_VALUE = 1,
	MACRO_INFOR_MACRO_NAME = 2,
};

enum{
	TIMER = 1,
	KB_KEY_DOWN,
	KB_KEY_UP,
	MS_KEY_DOWN,
	MS_KEY_UP,
	MS_RELATIVE,
	MS_ABSOLUTE,
	MS_WHEEL_INC,
	MS_WHEEL_DEC,
};

#define MACRO_NAME_LENGTH	30

typedef enum{
	RP_NONE = 0,
	PR_ONCE = 1,
	RP_CONTINUE = 2,
	RP_FIREPOWER = 3,
	PR_SPEC_TIME = 4,
}GKEY_EXECUTE_MODE;

#define  MACRO_KB_KEY		0x02
#define  MACRO_MS_KEY		0x04
#define  MACRO_MS_RM		0x08	//relative move
#define  MACRO_MS_AM		0x10	//absolute move
#define  MACRO_MS_WHEEL_UD	0x20	//wheel up & down
#define  MACRO_MS_WHEEL_LR	0x80	//wheel left & right

extern MACRO_HEAD_PACKET_STRU loadingHead;

extern MACRO_EVENT macroEvent;
extern MACRO_Data_STRU macroPack;
extern UINT8 macro_event;
//UINT16 macro_delayTimer;


typedef struct{
char a;
UINT32 b;

long int h;
}TEST;

void Macro_Init_Head(void);
void Macro_Get_FreeSpace(UINT8 *buffer);
void Macro_Get_Authority(UINT8 *buffer);
void Macro_DownLoad(UINT8 *buffer);
void Macro_Get_Information(UINT8 *buffer);
void Macro_Report_Value(UINT8 *buffer);
void Macro_Report_Name(UINT8 *buffer);
void Macro_Delete(UINT8 *buffer);
void Macro_Process(void);
UINT8 Macro_Get_ReportType(void);
UINT8 Macro_Start(KEY_REGIST_T key_function,BOOL key_changed,BOOL key_status,UINT8 key_index);
void Macro_Combine_Data(UINT8 *mouseBuffer,UINT8 *keyboardBuffer);
BOOL Macro_Proceed(void);

void Macro_Keyboard(void);

#endif

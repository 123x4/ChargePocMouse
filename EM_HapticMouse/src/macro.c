#include "macro.h"
#include "usb.h"
#include "pc_communicate.h"
#include "keydef.h"
#include "test.h"
#include "custom.h"



MACRO_DOWNLOAD_STRU download_stru; 
MACRO_HEAD_PACKET_STRU loadingHead;
MACRO_REPORT_STRU reportInfor;
MACRO_Data_STRU macroPack;
MACRO_EVENT macroEvent;


UINT8 usb_keyboard_macro[8];
UINT8 usb_mouse_macro[8];

UINT8 macro_event;
BOOL macro_lastPacket;
UINT16 macro_delayTimer;

void Macro_Keyboard(void)
{
	Tool_USB_ArrayData(usb_keyboard_macro,8);
}

/*
void Macro_Earse_DataBlock(UINT8 blockId,UINT16 length)
{
	#define  MACRO_CLEAR_DATA_TEM 64
	UINT8 ii;
	UINT8 dataTmp[MACRO_CLEAR_DATA_TEM];
	UINT16 remain,writeIndex = 0;
	UINT8 writeCount;
	UINT16 flashAddress;

	for (ii = 0;ii < MACRO_CLEAR_DATA_TEM;ii++)
	{
		dataTmp[ii] = 0;
	}
	
	while (writeIndex < length)
	{
		writeCount = ((writeIndex + MACRO_CLEAR_DATA_TEM >= length) ? MACRO_CLEAR_DATA_TEM : (length - writeIndex));

		flashAddress = blockId * MACRO_BLOCK_SIZE + writeIndex;

		//MacrodataROM_bE2Write(flashAddress,dataTmp,writeCount,25);	

		writeIndex += writeCount;
	}
}
*/

void Macro_Init_Head(void)
{
	#define MACRO_HEAD_TEM_LEN	10
	UINT8 ii;
	UINT16 address = 0,writeCnt;
	MACRO_HEAD_PACKET_STRU macro_head[MACRO_HEAD_TEM_LEN];//10*8 = 40byte

	
	for (ii = 0;ii < MACRO_HEAD_TEM_LEN;ii++)
	{
		macro_head[ii].id = 0;
		macro_head[ii].len = 0;
		macro_head[ii].CRC = 0;
	}

	while (address < MACRO_HEAD_SPACE)///1280/80=16
	{	
		if ((address + (MACRO_HEAD_TEM_LEN * MACRO_HEAD_STRU_LEN)) > MACRO_HEAD_SPACE)
		{
			writeCnt = MACRO_HEAD_SPACE - address;
		}else 
		{
			writeCnt = (MACRO_HEAD_TEM_LEN * MACRO_HEAD_STRU_LEN);
		}
		MACRO_WRITE_HEAD_BYTES(address, (UINT8 *)macro_head, writeCnt);
		address += writeCnt;
	}
}


UINT8 Macro_Check_FreeSpace(void)
{
	MACRO_HEAD_PACKET_STRU macro_head;
	UINT8 freeBlock = 0;
	UINT16 adderss = 0;

	while (adderss < MACRO_HEAD_SPACE)
	{
		MACRO_READ_HEAD_BYTES(adderss, (UINT8 *)&macro_head, MACRO_HEAD_STRU_LEN);
		
		if ((macro_head.id == 0) || (macro_head.id > (CUSTOM_BUTTON_NUMBER - 2)))
		{
			freeBlock++;
		}
		adderss += MACRO_HEAD_STRU_LEN;
	}

	return freeBlock;
}

void Macro_Get_FreeSpace(UINT8 *buffer)
{
	UINT8 freeBlock_num,ii;
	UINT32 freeBytes;

	freeBlock_num = Macro_Check_FreeSpace();

	freeBytes = freeBlock_num * MACRO_BLOCK_SIZE;


	buffer[0] = MACRO_BLOCK_CHECK_SPACE;
	buffer[1] = (UINT8)(freeBytes >> 24);
	buffer[2] = (UINT8)(freeBytes >> 16);
	buffer[3] = (UINT8)(freeBytes >> 8);
	buffer[4] = (UINT8)freeBytes;

	for (ii = 5;ii < TRANSFER_LENGTH;ii++)
	{
		buffer[ii] = 0;
	}

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH);	

}

UINT8 Macro_Search_Block(UINT16 startBlock_index,UINT8 id)
{
	MACRO_HEAD_PACKET_STRU macro_head;
	UINT8 ii = 0,freeBlock_index = 0xff;
	UINT16 adderss = (startBlock_index * MACRO_HEAD_STRU_LEN);

	while (adderss < MACRO_HEAD_SPACE)
	{
		MACRO_READ_HEAD_BYTES(adderss, (UINT8 *)&macro_head, MACRO_HEAD_STRU_LEN);

		if (macro_head.id == id)
		{
			freeBlock_index = startBlock_index + ii;
			break;
		}
		adderss += MACRO_HEAD_STRU_LEN;

		ii++;
	}
	
	return freeBlock_index;
}

void Macro_Clear_Id(UINT16 idNum)
{
	MACRO_HEAD_PACKET_STRU macro_head;
	MACRO_HEAD_PACKET_STRU macroHead_empty = {0,0,0};
	UINT16 adderss = 0;

	while (adderss < MACRO_HEAD_SPACE)
	{
		MACRO_READ_HEAD_BYTES(adderss, (UINT8 *)&macro_head, MACRO_HEAD_STRU_LEN);

		if ((macro_head.id == idNum) || (macro_head.id > (CUSTOM_BUTTON_NUMBER - 2)))
		{
			MACRO_WRITE_HEAD_BYTES(adderss, (UINT8 *)&macroHead_empty, MACRO_HEAD_STRU_LEN);
		}
		adderss += MACRO_HEAD_STRU_LEN;
	}
}



void Macro_Get_Authority(UINT8 *buffer)
{
	UINT8 idNum,freeBlock_num = 0,ii;
	UINT16 data_Lenth, adderss = 0,useBlock_num;
	MACRO_HEAD_PACKET_STRU macro_head;


	idNum = buffer[1];
	data_Lenth = (((UINT16)buffer[2] << 8) + (UINT16)buffer[3]); 
	
	loadingHead.id = idNum;
	loadingHead.len = (UINT32)data_Lenth;

	///calcute the will used block 
	useBlock_num = data_Lenth/MACRO_BLOCK_SIZE;

	if ((data_Lenth % MACRO_BLOCK_SIZE) != 0)
	{
		useBlock_num++;
	}
	
	///calcute the free block
	freeBlock_num = Macro_Check_FreeSpace();

	while (adderss < MACRO_HEAD_SPACE)
	{
		MACRO_READ_HEAD_BYTES(adderss, (UINT8 *)&macro_head, MACRO_HEAD_STRU_LEN);

		if (macro_head.id == idNum)
		{
			freeBlock_num++;
		}
		adderss += MACRO_HEAD_STRU_LEN;
	}

	if (freeBlock_num >= useBlock_num)
	{
		buffer[2] = TRUE;

	}else
	{
		buffer[2] = FALSE;
	}

	for (ii = 3;ii < TRANSFER_LENGTH;ii++)
	{
		buffer[ii] = 0;
	}

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH);	

}

void MACRO_Write_BlockHead(UINT8 blockNum,MACRO_HEAD_PACKET_STRU macro_head)
{
	UINT16 adderss;

	adderss = blockNum*MACRO_HEAD_STRU_LEN;

	MACRO_WRITE_HEAD_BYTES(adderss, (UINT8 *)&macro_head, MACRO_HEAD_STRU_LEN);
}



void Macro_DownLoad(UINT8 *buffer)
{
	UINT8 id,valueData_num,writeByteNum;
	UINT16 curPacketNum,allPacketNum;
	UINT8 *pdata;
	UINT32 address;


	id = buffer[1];
	curPacketNum = (((UINT16)buffer[2] << 8) + (UINT16)buffer[3]); 
	allPacketNum = (((UINT16)buffer[4] << 8) + (UINT16)buffer[5]); 
	valueData_num = buffer[6];
	pdata = &buffer[7];
	
	///Deal the first download packet
	if (curPacketNum == MACRO_PACKET_START)
	{
		Macro_Clear_Id(id);

		download_stru.byte_writeIndex = 0;
		download_stru.currenBlock = Macro_Search_Block(0,0);

		if (download_stru.currenBlock == 0XFF)
		{
			return;
		}

		MACRO_Write_BlockHead(download_stru.currenBlock,loadingHead);
		MACRO_ERASE_BLOCK(download_stru.currenBlock);
	}

	address = (download_stru.currenBlock * MACRO_BLOCK_SIZE) + download_stru.byte_writeIndex;

	if ((download_stru.byte_writeIndex + valueData_num) > MACRO_BLOCK_SIZE)
	{
		writeByteNum = MACRO_BLOCK_SIZE - download_stru.byte_writeIndex;
		MACRO_WRITE_BLOCK_BYTES(address,pdata,writeByteNum);

		download_stru.byte_writeIndex = 0;
		download_stru.currenBlock = Macro_Search_Block(0,0);

		if (download_stru.currenBlock == 0XFF)
		{
			return;
		}

		MACRO_Write_BlockHead(download_stru.currenBlock,loadingHead);
		MACRO_ERASE_BLOCK(download_stru.currenBlock);

		address = (download_stru.currenBlock * MACRO_BLOCK_SIZE);
		pdata = pdata + writeByteNum;
		writeByteNum = valueData_num - writeByteNum;
		MACRO_WRITE_BLOCK_BYTES(address,pdata,writeByteNum);

		download_stru.byte_writeIndex += writeByteNum;

	}else if ((download_stru.byte_writeIndex + valueData_num) == MACRO_BLOCK_SIZE)
	{
		MACRO_WRITE_BLOCK_BYTES(address,pdata,valueData_num);
		
		download_stru.byte_writeIndex += valueData_num;

		if (curPacketNum != allPacketNum)
		{
			download_stru.byte_writeIndex = 0;
			download_stru.currenBlock = Macro_Search_Block(0,0);

			if (download_stru.currenBlock == 0XFF)
			{
				return;
			}

			MACRO_Write_BlockHead(download_stru.currenBlock,loadingHead);
			MACRO_ERASE_BLOCK(download_stru.currenBlock);
		}
	}else
	{
		MACRO_WRITE_BLOCK_BYTES(address,pdata,valueData_num);

		download_stru.byte_writeIndex += valueData_num;
	}

	Tool_Clear_ArrayData(buffer,2,63);

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH);	

}




void Macro_Get_Information(UINT8 *buffer)
{
	UINT8 id = 0,type;
	UINT16 allPacketNum,adderss = 0;
	MACRO_HEAD_PACKET_STRU macro_head;
	UINT8 blockNum;

	type = buffer[1];
	id = buffer[2];

	if(report_continue_tag != REPORT_NONE)
	{
		report_continue_tag = REPORT_NONE;
		return;
	}

	if (type == MACRO_INFOR_MACRO_VALUE)
	{
		blockNum = Macro_Search_Block(0,id);

		if (blockNum == 0xff)
		{
			return;
		}

		adderss = blockNum * MACRO_HEAD_STRU_LEN;
		MACRO_READ_HEAD_BYTES(adderss, (UINT8 *)&macro_head, MACRO_HEAD_STRU_LEN);

		allPacketNum = (macro_head.len - MACRO_NAME_LENGTH)/MACRO_INFOR_REPORT_MAX_LEN;

		if (((macro_head.len - MACRO_NAME_LENGTH) % MACRO_INFOR_REPORT_MAX_LEN) != 0)
		{
			allPacketNum++;
		}

		reportInfor.id = macro_head.id;
		reportInfor.reportIndex = MACRO_NAME_LENGTH;
		reportInfor.reportNum = macro_head.len;
		reportInfor.packetIndex = 0;
		reportInfor.packetNum = allPacketNum;

		report_continue_tag = REPORT_MACRO_VALUE;

	}else if (type == MACRO_INFOR_MACRO_NAME)
	{
		report_continue_tag = REPORT_MACRO_NAME;
		reportInfor.id = 1;
	}
}


void Macro_Report_Value(UINT8 *buffer)
{	
	static UINT16 readCnt_byte;
	UINT32 flashAddress;
	UINT8 read_length,sendCnt,nextSendCnt;
	//MACRO_HEAD_PACKET_STRU macro_head;
	
	if((report_continue_tag != REPORT_MACRO_VALUE))
	{
		return;
	}

	reportInfor.packetIndex++;

	if (reportInfor.packetIndex == MACRO_PACKET_START)
	{
		readCnt_byte = MACRO_NAME_LENGTH;

		reportInfor.blockIndex  = Macro_Search_Block(0,reportInfor.id);
	}

	///Check the data will send num
	if (reportInfor.reportIndex + MACRO_INFOR_REPORT_MAX_LEN > reportInfor.reportNum)
	{
		read_length = reportInfor.reportNum - reportInfor.reportIndex;

	}else
	{
		read_length = MACRO_INFOR_REPORT_MAX_LEN;
	}

	///check send num if overflow block
	if ( readCnt_byte + read_length > MACRO_BLOCK_SIZE)
	{
		sendCnt = MACRO_BLOCK_SIZE - readCnt_byte;
		flashAddress = reportInfor.blockIndex * MACRO_BLOCK_SIZE + readCnt_byte;
		MACRO_READ_BLOCK_BYTES(flashAddress, (UINT8 *)&buffer[8], sendCnt);

		nextSendCnt = read_length - sendCnt;
		reportInfor.blockIndex = Macro_Search_Block((reportInfor.blockIndex+1),reportInfor.id);
		flashAddress = reportInfor.blockIndex * MACRO_BLOCK_SIZE;
		MACRO_READ_BLOCK_BYTES(flashAddress, (UINT8*)((UINT8 *)&buffer[8]+sendCnt), nextSendCnt);

		readCnt_byte = nextSendCnt;

	}else if (readCnt_byte + read_length == MACRO_BLOCK_SIZE)
	{
		flashAddress = reportInfor.blockIndex * MACRO_BLOCK_SIZE + readCnt_byte;

		MACRO_READ_BLOCK_BYTES(flashAddress, (UINT8 *)&buffer[8], read_length);

		reportInfor.blockIndex = Macro_Search_Block((reportInfor.blockIndex+1),reportInfor.id);

		readCnt_byte = 0;
	}else
	{
		flashAddress = reportInfor.blockIndex * MACRO_BLOCK_SIZE + readCnt_byte;
		MACRO_READ_BLOCK_BYTES(flashAddress, (UINT8 *)&buffer[8], read_length);

		readCnt_byte += read_length;
	}
	
	reportInfor.reportIndex += read_length;

	if (reportInfor.reportIndex >=  reportInfor.reportNum)
	{
		report_continue_tag = REPORT_NONE;
	}
	
	buffer[0] = MACRO_BLOCK_GET_INFOR;
	buffer[1] = REPORT_MACRO_VALUE;
	buffer[2] = reportInfor.id;
	buffer[3] = (UINT8)(reportInfor.packetIndex >> 8);
	buffer[4] = (UINT8)(reportInfor.packetIndex);
	buffer[5] = (UINT8)(reportInfor.packetNum >> 8);
	buffer[6] = (UINT8)(reportInfor.packetNum);
	buffer[7] = read_length;

	Tool_Clear_ArrayData(buffer,(read_length+8),63);

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH);	

}

void Macro_Report_Name(UINT8 *buffer)
{
	UINT16 blockNum = 0;
	UINT32 flashAddress;

	MACRO_HEAD_PACKET_STRU macro_head;

	if((report_continue_tag != REPORT_MACRO_NAME))
	{
		return;
	}

	while (reportInfor.id <= MACRO_NUM)
	{
		blockNum = Macro_Search_Block(0,reportInfor.id);

		if (blockNum == 0xff)
		{
			reportInfor.id++;

		}else
		{
			macro_head.id = reportInfor.id;
			break;
		}	
	}

	if (reportInfor.id > MACRO_NUM)
	{
		report_continue_tag = REPORT_NONE;
	}else
	{
		flashAddress = blockNum * MACRO_BLOCK_SIZE;
		MACRO_READ_BLOCK_BYTES(flashAddress, (UINT8 *)&buffer[3], MACRO_NAME_LENGTH);	

		reportInfor.id++;
	}

	if (report_continue_tag == REPORT_NONE)
	{
		buffer[2] = 0;	

		Tool_Clear_ArrayData(buffer,3,63);
	}else
	{
		buffer[2] = macro_head.id;

		Tool_Clear_ArrayData(buffer,(3+30),63);
	}

	buffer[0] = MACRO_BLOCK_GET_INFOR;
	buffer[1] = REPORT_MACRO_NAME;

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH);	

	
}

void Macro_Delete(UINT8 *buffer)
{
	UINT8 macroId = buffer[1];
	UINT8 blockNum = 0;
	BOOL hadDelete = FALSE;
	MACRO_HEAD_PACKET_STRU macroHead = {0,0,0};

	while (blockNum != 0xff)
	{
		blockNum = Macro_Search_Block(blockNum,macroId);

		if (blockNum != 0xff)
		{
			MACRO_Write_BlockHead(blockNum,macroHead);
			hadDelete = TRUE;
		}
	}

	if (hadDelete == TRUE)
	{
		buffer[2] = 1;
	}else
	{
		buffer[2] = 0;
	}

	Tool_Clear_ArrayData(buffer,3,63);

	USB_AutoDelay_LoadInEP_Data(EP_3_CUSTION_OUT, buffer, TRANSFER_LENGTH);

}



UINT8 Macro_Start(KEY_REGIST_T key_function,BOOL key_changed,BOOL key_status,UINT8 key_index)
{
	#define MACRO_POWER_TYPE	0xF0
	#define MACRO_CONTINUE_TYPE	0xF1
	#define MACRO_ONCE_TYPE		0xF2
	#define MACRO_SPEC_TYPE		0xF3

	UINT8 macro_id, i;
	UINT8 count,macro_type;
	UINT8 result = 0;
	UINT16 adderss;
	MACRO_HEAD_PACKET_STRU macro_head;

	macro_id = key_function.usb_registration.content.macro.id;
	count =  key_function.usb_registration.content.macro.count;
	macro_type = key_function.usb_registration.id;

	///if communicate with pc drive,no deal macro key
	if (report_continue_tag != REPORT_NONE)
	{
		return result;
	}

	if (key_status == KEY_STATUS_DOWN)
	{
		if (key_changed)
		{
			for( i = 0; i < 8; i++)
			{
				usb_keyboard_macro[i] = 0;
			}
			///macro sending

			//if (macroPack.send == TRUE)
			//{
			//	macroPack.send = FALSE;

			//}else
			{	
				macroPack.send = TRUE;
				macroPack.byteIndex = MACRO_NAME_LENGTH;


				macroPack.blockIndex = Macro_Search_Block(0,macro_id);

				macroPack.byteIndex_inBlock = MACRO_NAME_LENGTH;

				adderss = (macroPack.blockIndex * MACRO_HEAD_STRU_LEN);

				MACRO_READ_HEAD_BYTES(adderss, (UINT8 *)&macro_head, MACRO_HEAD_STRU_LEN);
				macroPack.macroHead = macro_head;

				switch(macro_type)
				{
				case MACRO_ONCE_TYPE:
					macroPack.macroPro = PR_ONCE;
					break;
				case MACRO_CONTINUE_TYPE:
					macroPack.macroPro = RP_CONTINUE;
					break;
				case MACRO_POWER_TYPE:
					macroPack.macroPro = RP_FIREPOWER;
					break;
				case MACRO_SPEC_TYPE:
					macroPack.macroPro = PR_SPEC_TIME;
					macroPack.sendProCnt = count;
					if (!macroPack.sendProCnt)
					{
						return result;
					}else if (macroPack.sendProCnt)
					{
						macroPack.sendProCnt--;
					}
					break;
				default:
					macroPack.macroPro = PR_ONCE;
					break;
				}
			}
		}	
	}
	else //key up
	{
		if (key_changed)
		{
			if ((macroPack.macroPro == RP_FIREPOWER) && (macroPack.send == TRUE))
			{
				macroPack.send = FALSE;
			}			
		}
	}

	return result;
}


void Macro_Deal_KeyboardData(KEYBOARD_KEY_TYPE keyboard_key_data,UINT8*keyboard_buffer)
{
	UINT8 key_code = 0;
	UINT8 ii = 0;
	UINT8 nn = 0;
	UINT8 buffer_temp[8];

	if ((keyboard_key_data.key_code & 0xF0) == 0xE0)//modify
	{
		key_code = (keyboard_key_data.key_code - 0xE0);
		if (key_code < 8) // To prevent the subscript overflow
		{
			if (keyboard_key_data.key_status == KB_KEY_DOWN)
			{
				keyboard_buffer[0] |= multifuction_table[key_code];
			}else  //up
			{
				keyboard_buffer[0] &= ~multifuction_table[key_code];
			}			
		}
	}else  //std
	{
		key_code = keyboard_key_data.key_code;

		if (keyboard_key_data.key_status == KB_KEY_DOWN)
		{
			for (ii = 2; ii < 8;ii++)
			{
				if (!keyboard_buffer[ii]) // this place is empty
				{
					keyboard_buffer[ii] = key_code;
					break;	
				}	
			}
		}else  //up
		{

			Tool_Assigned_BufferToBuffer(keyboard_buffer,buffer_temp,8);

			Tool_Clear_ArrayData(keyboard_buffer,0,7);

			///clear all up key
			for (ii = 2;ii < 8;ii++)
			{
				if (buffer_temp[ii] == key_code)
				{
					buffer_temp[ii] = 0;
				}
			}

			///add the no empty data to send buffer
			for (ii = 2; ii < 8;ii++)
			{
				for (nn = 2;nn < 8;nn++)
				{
					if (buffer_temp[nn] != 0)
					{
						keyboard_buffer[ii] = buffer_temp[nn];
						buffer_temp[nn] = 0;
						break;
					}
				}
			}
		}		
	}
}


void Macro_Get_Data(UINT8* mouse_buffer,UINT8* keyboard_buffer,UINT8 *key_event)
{
	KEYBOARD_KEY_TYPE keyboard_key_data;
	UINT32 flashAddress;
	UINT8 readByte_len,nextReadByte_len;
	UINT8 next_blockIndex,offSet = 0;

	if(macroEvent.type == TIMER)
	{
		if(macro_delayTimer != 0)
		{
			return;
		}		
	}

	///send over
	if(macroPack.byteIndex >= macroPack.macroHead.len)
	{
		if((macroPack.macroPro == RP_CONTINUE) || (macroPack.macroPro == RP_FIREPOWER) )
		{
			macroPack.byteIndex = MACRO_NAME_LENGTH;
			macroPack.byteIndex_inBlock = MACRO_NAME_LENGTH;
			macroPack.blockIndex = Macro_Search_Block(0,macroPack.macroHead.id);

			if (macroPack.blockIndex == 0xff)
			{
				macroPack.send = FALSE;
				return;
			}

			macroPack.send = TRUE;

			return;
		}else if (macroPack.macroPro == PR_SPEC_TIME)
		{
			if (macroPack.sendProCnt == 0)
			{
				macroPack.send = FALSE;
			}else
			{
				macroPack.sendProCnt--;
				
				macroPack.byteIndex = MACRO_NAME_LENGTH;
				macroPack.byteIndex_inBlock = MACRO_NAME_LENGTH;
				macroPack.blockIndex = Macro_Search_Block(0,macroPack.macroHead.id);

				if (macroPack.blockIndex == 0xff)
				{
					macroPack.send = FALSE;
					return;
				}

				macroPack.send = TRUE;
			}
		}else
		{
			macroPack.send = FALSE;
			
			return; 
		}
	}

	///Read Flash Block data
	if (macroPack.byteIndex_inBlock  >= MACRO_BLOCK_SIZE)
	{
		macroPack.byteIndex_inBlock = macroPack.byteIndex_inBlock - MACRO_BLOCK_SIZE;

		macroPack.blockIndex = Macro_Search_Block((macroPack.blockIndex + 1),macroPack.macroHead.id);

		if (macroPack.blockIndex == 0xff)
		{
			macroPack.send = FALSE;
			return;
		}

		flashAddress = (macroPack.blockIndex * MACRO_BLOCK_SIZE) + macroPack.byteIndex_inBlock;
		MACRO_READ_BLOCK_BYTES(flashAddress, (UINT8 *)&macroEvent, sizeof(MACRO_EVENT));

	}else if (macroPack.byteIndex_inBlock + sizeof(MACRO_EVENT) > MACRO_BLOCK_SIZE)
	{
		readByte_len = MACRO_BLOCK_SIZE - macroPack.byteIndex_inBlock;

		flashAddress = (macroPack.blockIndex * MACRO_BLOCK_SIZE) + macroPack.byteIndex_inBlock;
		MACRO_READ_BLOCK_BYTES(flashAddress, (UINT8 *)&macroEvent, readByte_len);

		nextReadByte_len = sizeof(MACRO_EVENT) - readByte_len;

		next_blockIndex = Macro_Search_Block((macroPack.blockIndex + 1),macroPack.macroHead.id);

		if (next_blockIndex == 0xff)
		{
			macroPack.send = FALSE;
			return;
		}

		flashAddress = (next_blockIndex * MACRO_BLOCK_SIZE);
		MACRO_READ_BLOCK_BYTES(flashAddress, (UINT8 *)(((UINT8 *)&macroEvent) + readByte_len), nextReadByte_len);

	}else
	{
		flashAddress = (macroPack.blockIndex * MACRO_BLOCK_SIZE) + macroPack.byteIndex_inBlock;
		MACRO_READ_BLOCK_BYTES(flashAddress, (UINT8 *)&macroEvent, sizeof(MACRO_EVENT));
	}

	

	switch(macroEvent.type)
	{
	case TIMER:
		offSet += 3;
		macro_delayTimer = (macroEvent.event_value.delay_time.timer_Hi << 8) + 
			macroEvent.event_value.delay_time.timer_Lo; 
		break;
	case MS_KEY_DOWN:
		if(macroEvent.event_value.key.key_index <= 5)
		{
			mouse_buffer[1] |= mouse_table[macroEvent.event_value.key.key_index - 1];
			*key_event |= MACRO_MS_KEY;
		}
		offSet += 2;

		break;
	case MS_KEY_UP:
		if(macroEvent.event_value.key.key_index <= 5)
		{
			mouse_buffer[1] &= ~mouse_table[macroEvent.event_value.key.key_index - 1];
			*key_event |= MACRO_MS_KEY;
		}
	
		offSet += 2;
		break;
	case MS_RELATIVE:
		mouse_buffer[2] = macroEvent.event_value.location.x_h;
		mouse_buffer[3] = macroEvent.event_value.location.x_l;
		mouse_buffer[4] = macroEvent.event_value.location.y_h;
		mouse_buffer[5] = macroEvent.event_value.location.y_l;
		*key_event |= MACRO_MS_RM;
		offSet += 5;
		break;
	case MS_WHEEL_INC:
		mouse_buffer[6] = 0x01;
		*key_event |= MACRO_MS_WHEEL_UD;
		offSet += 1;
		break;
	case MS_WHEEL_DEC:
		mouse_buffer[6] = 0xFF;
		*key_event |= MACRO_MS_WHEEL_UD;
		offSet += 1;
		break;
	case KB_KEY_DOWN:
		keyboard_key_data.key_code = macroEvent.event_value.key.key_index;	
		keyboard_key_data.key_status = KB_KEY_DOWN;

		Macro_Deal_KeyboardData(keyboard_key_data,keyboard_buffer);
	

		*key_event |= MACRO_KB_KEY;
		offSet += 2;
		break;
	case KB_KEY_UP:
		keyboard_key_data.key_code = macroEvent.event_value.key.key_index;	
		keyboard_key_data.key_status = KB_KEY_UP;

		Macro_Deal_KeyboardData(keyboard_key_data,keyboard_buffer);


		*key_event |= MACRO_KB_KEY;
		offSet += 2;
		break;
	default:

		macroPack.send = FALSE;
		
		break;
	}

	macroPack.byteIndex += offSet;
	macroPack.byteIndex_inBlock += offSet;
}


void Macro_Process(void)
{
	static BOOL last_macro_send = FALSE;
	static UINT8 macro_least_sendInterval = 0;

	if (macroPack.send == TRUE)
	{
		if (macro_least_sendInterval)macro_least_sendInterval--;

		if (!macro_least_sendInterval)
		{
			Macro_Get_Data(usb_mouse_macro,usb_keyboard_macro,&macro_event);
			macro_least_sendInterval = 8;
		}	
	}

	///Deal if macro  Sticky keys send empty packet
	if(macroPack.send != last_macro_send)
	{
		last_macro_send = macroPack.send;

		if (macroPack.send == FALSE)
		{
			if (Tool_Check_If_Had_NonZero(usb_keyboard_macro,0,7))
			{
				macro_event |= MACRO_KB_KEY;
			}

			if (Tool_Check_If_Had_NonZero(usb_mouse_macro,1,7))
			{
				macro_event |= MACRO_MS_KEY;
			}

			macro_lastPacket = TRUE;
		}
	}
}


UINT8 Macro_Get_ReportType(void)
{
	UINT8 result = 0;


	if (macro_event & MACRO_MS_KEY)
	{
		macro_event &= ~MACRO_MS_KEY;

		result |= MACRO_MS_KEY;
	}

	if (macro_event & MACRO_KB_KEY)
	{
		macro_event &= ~MACRO_KB_KEY;

		result |= MACRO_KB_KEY;
	}

	if(macro_event & MACRO_MS_WHEEL_UD)
	{
		result |= MACRO_MS_KEY;	
	}

    
    return result ;
}


void Macro_Combine_Data(UINT8 *mouseBuffer,UINT8 *keyboardBuffer)
{
	UINT8 nn,kk;

	if(macro_event & MACRO_MS_WHEEL_UD)
	{
		macro_event &= ~MACRO_MS_WHEEL_UD;
		mouseBuffer[6] = mouseBuffer[6] + usb_mouse_macro[6];
		usb_mouse_macro[6] = 0;
	}

	mouseBuffer[1] = mouseBuffer[1] | usb_mouse_macro[1];

	keyboardBuffer[0] = keyboardBuffer[0] | usb_keyboard_macro[0]; 

	for(kk = 2;kk < 8;kk++)
	{
		if (usb_keyboard_macro[kk]) // had code
		{
			for (nn = 2;nn < 8;nn++)
			{
				if (keyboardBuffer[nn] == 0)// this place no code
				{
					keyboardBuffer[nn] = usb_keyboard_macro[kk];
					break;
				}					
			}
		}
	}
}

BOOL Macro_Proceed(void)
{
	BOOL result = FALSE;
	
	if (macroPack.send  || macro_lastPacket)
	{
		if (macroPack.send )
		{
			result = TRUE;
		}

		if (macro_lastPacket)
		{
			macro_lastPacket = FALSE;
			result = TRUE;
		}
	}


	return result;
}


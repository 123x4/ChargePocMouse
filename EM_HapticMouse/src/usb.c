#include "usb.h"
#include "usbcore.h"
#include "test.h"
#include "gloab.h"
#include "usbhw.h"
#include "device.h"
//#include "Type.h"
//#include "stdint.h"

unsigned short check_usb_config_count = USB_CHECK_TIMER;
unsigned short check_usb_endpoit_count = USB_EP_CHECK_TIMER;

bool usb_first_invalid_tag = TRUE;	
bool endpoint_invalid_tag = TRUE;

bool last_usbConfig_event;

//unsigned char usb_firstPacket;

void Usb_Init(void)
{
//	USBFS_Start(0, USBFS_5V_OPERATION);	
	nsAPI_USB_Init();
	check_usb_and_endpoint(USB_ENUM_CHECK, 0);

	nsAPI_USB_EnableEP(EP_4_CUSTION_IN);
//	USBFS_EnableOutEP(EP_4_CUSTION_IN);	

//	USBFS_EnableSofInt();

	//while (!USBFS_GetConfiguration())
	//{
	//	WATCH_DOG_CLEAR; 
	//}
}

void Usb_Start(void)
{
//	USBFS_EnableOutEP(EP_4_CUSTION_IN);	
//  nsAPI_USB_EnableEP(EP_4_CUSTION_IN);
	USBFS_EnableSofInt();
}

void Usb_Stop(void)
{
	USBFS_DisableSofInt();
}


bool Usb_Read_Bytes(unsigned char endpoint_num,unsigned char *buf,unsigned char len)
{ 
	bool result=FALSE ;
#if 0                  //Add these will affect reportrate
	uint8_t i=0;
	for(i=0;i<200;i++)
	{
		if(nsAPI_USB_EP_Status(endpoint_num))	
		{
			break;
			}
		nsAPI_SSP_Delay1us(10);
	}

//	while(!nsAPI_USB_EP_Status(endpoint_num));
	if( nsAPI_USB_ReadEP(endpoint_num, buf))
	{
//		if (USBFS_bGetEPState(endpoint_num) == USBFS_OUT_BUFFER_FULL)
//		{
//			USBFS_EnableOutEP(endpoint_num);
//			USBFS_ReadOutEP(endpoint_num, buf, len);

		result = TRUE;
	}
	return result;
#endif
	
	if(nsAPI_USB_EP_Status(endpoint_num))	
	{
		nsAPI_USB_EnableEP(endpoint_num);
		if( nsAPI_USB_ReadEP(endpoint_num, buf))
		{
			result = TRUE;
		}
	}
	return result;
}


bool check_usb_and_endpoint(unsigned char check_type, unsigned char endpoint)
{

	bool result = FALSE;
	bool watchdog_event = FALSE;
	unsigned char temp;	
	unsigned char ii;

	//WATCH_DOG_CLEAR; 

	if(check_type & USB_ENUM_CHECK)
	{
		check_usb_config_count = USB_ENUM_TIMER;

//		if(CY_SYS_RES_CAUSE_REG & CY_SYS_RESET_WDT)
//		{
//			watchdog_event = TRUE;
//		}

//		while (!USBFS_GetConfiguration())
//		while (!nsAPI_USB_ReqGetConfiguration())
		while (1)
		{
//			WATCH_DOG_CLEAR; 

			if(watchdog_event)		
			{
				if(check_usb_config_count)	
				{
					TimerDelayMsec(1);
//					DrvDelay1mS(1);
					check_usb_config_count--;
				}

				if(check_usb_config_count == 0)
				{
					while(1);
				}
			}
		}
	}

	if(check_type & USB_CONFIG_CHECK)
	{
//		if(!nsAPI_USB_ReqGetConfiguration())
		if(1)	
		{
			last_usbConfig_event = TRUE;

			if(usb_first_invalid_tag == TRUE)
			{
				usb_first_invalid_tag = FALSE;
				check_usb_config_count = USB_CHECK_TIMER;
			}

			if (check_usb_config_count == 0)
			{					
				while (1);
			}
			else
			{
				result = FALSE;
			}
		}
		else
		{
			///because had some computer had configuration two count,so
			///if have the event usb unconfig->config ,enable OUT EP
			if (last_usbConfig_event)
			{
				last_usbConfig_event = FALSE;

//				USBFS_EnableOutEP(EP_4_CUSTION_IN);
				nsAPI_USB_EnableEP(EP_4_CUSTION_IN);
			}

			usb_first_invalid_tag = TRUE;		

			result = TRUE;
		}
	}

	if(check_type & USB_EP_CHECK)
	{
		for(ii = 0;ii < 60;ii++)
		{
			temp = (nsAPI_USB_EP_Status(endpoint));

			if (temp)
			{
				break;
			}
			Delayus(10);
//			TimerDelayUsec(10);	
		}

		if (temp != USBFS_IN_BUFFER_EMPTY)
		{
			if(endpoint_invalid_tag == TRUE)
			{
				endpoint_invalid_tag = FALSE;
				check_usb_endpoit_count = USB_EP_CHECK_TIMER;
			}

			if (check_usb_endpoit_count == 0)
			{					
				while (1);
			}
			else
			{
				result = FALSE;
			}
		}
		else if (temp == USBFS_IN_BUFFER_EMPTY)
		{
			endpoint_invalid_tag = TRUE;

			result = TRUE;
		}
	}

	if(check_type & USB_SUSBEND_CHECK)
	{
//		if(!nsAPI_USB_ReqGetConfiguration())
		if(1)
		{

			if(usb_first_invalid_tag == TRUE)
			{
				usb_first_invalid_tag = FALSE;
				check_usb_config_count = USB_SUSPEND_TIMER;
			}

			if(check_usb_config_count)
			{
				check_usb_config_count--;
			}

			if (check_usb_config_count == 0)
			{					
				while (1);
			}
		}
		else
		{

			usb_first_invalid_tag = TRUE;

			result = TRUE;
		}
	}

	return result;
}

bool USB_LoadInEP_Data(unsigned char ep_num, unsigned char * pData, unsigned short wLength)
{
	bool result = FALSE;

/*
	unsigned char temp_64_byte[64] = {0,1,2,3,4,5,6,7};
	unsigned char temp_8_byte[8];

	if (ep_num == EP_1_MOUSE)
	{
		Tool_Clear_ArrayData(temp_8_byte,0,7);

		if (usb_firstPacket & EP_1_MOUSE_MASK)
		{
			usb_firstPacket &= ~EP_1_MOUSE_MASK;
			USB_LoadInEP_Data(EP_1_MOUSE, temp_8_byte, 8);
			TimerDelayMsec(5); 
		}
	}

	if (ep_num == EP_2_KEYBOARD)
	{
		Tool_Clear_ArrayData(temp_8_byte,0,7);

		if (usb_firstPacket & EP_2_KEYBOARD_MASK)
		{
			usb_firstPacket &= ~EP_2_KEYBOARD_MASK;
			USB_LoadInEP_Data(EP_2_KEYBOARD, temp_8_byte, 8);

			TimerDelayMsec(5);
		}
	}

	if (ep_num == EP_3_CUSTION_OUT)
	{
		//Tool_Clear_ArrayData(temp_64_byte,0,63);

		if (usb_firstPacket & EP_3_DRIVE_IN_MASK)
		{
			usb_firstPacket &= ~EP_3_DRIVE_IN_MASK;
			USB_LoadInEP_Data(EP_3_CUSTION_OUT, temp_64_byte, TRANSFER_LENGTH);

			TimerDelayMsec(5);
		}
	}
*/
	//while(!(USBFS_GetEPState(ep_num) == USBFS_EVENT_PENDING)) {}
//	if(check_usb_and_endpoint(USB_CONFIG_CHECK | USB_EP_CHECK, ep_num))
	{
//		USBFS_LoadInEP(ep_num, pData, wLength);
		nsAPI_USB_WriteEP(ep_num, pData, wLength);
		result = TRUE;
	}

	return result;
}


bool USB_AutoDelay_LoadInEP_Data(unsigned char ep_num, unsigned char * pData, unsigned short wLength)
{
	bool result = FALSE;
	uint8_t i=0;

	for(i=0;i<60;i++)
	{
		result = nsAPI_USB_EP_Status(ep_num);
		if(result == TRUE)	
		{
			break;
		}
		nsAPI_SSP_Delay1us(10);
	}

	if(result ==TRUE)nsAPI_USB_WriteEP(ep_num, pData, wLength);
	result = TRUE;

	return result;
}

void Usb_Check_EFT(void)
{
	check_usb_and_endpoint(USB_CONFIG_CHECK, 0);
}

void Usb_Check_EnableOutEp(void)
{
//	if (USBFS_IsConfigurationChanged())
	{
		nsAPI_USB_EnableEP(EP_4_CUSTION_IN);
//		USBFS_EnableOutEP(EP_4_CUSTION_IN);
	}
}



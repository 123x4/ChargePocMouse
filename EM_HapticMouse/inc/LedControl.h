#ifndef LED_CONTROL_H		/* Guard to prevent multiple inclusions */
#define LED_CONTROL_H 

//#include "cytypes.h"

//#include "../inc/LPC11Uxx.h"
//#include "../inc/device.h"
//#include "../inc/I2C.h"
//#include "../inc/OSC.h"
//#include "../inc/led.h"
//#include "../inc/LedControl.h"



/*******************************************************************************
* 	Data Type Definitions
********************************************************************************/
	
typedef enum _PointPage_{
 POINT_TO_PAGE1 = 0x00,
 POINT_TO_PAGE2 = 0x01,
 POINT_TO_PAGE3 = 0x02,
 POINT_TO_PAGE4 = 0x03,
 POINT_TO_PAGE5 = 0x04,
 POINT_TO_PAGE6 = 0x05,
 POINT_TO_PAGE7 = 0x06,
 POINT_TO_PAGE8 = 0x07,
 POINT_TO_PAGE9 = 0x0b,     /* Page Night, 0b00001011 */
}Point_Page;




/*******************************************************************************
* 	Macro Definitions
********************************************************************************/
#define  EnableINTB
    
#define  LED_DRIVER_SLAVEADDR                       0xA0// Modify by Martin 2017.11.10   (0x74)   /* IS31FL3731 slave address, AD connect to the GND, 7-bit 1110100*/
#define  LED_DRIVER2_SLAVEADDR                      0x5c//(0x77)   /* IS31FL3731 slave address, AD connect to the GND, 7-bit 1110100*/
#define  LED_DRIVER_WRITE_BYTE_COUNT                  (2u)


#define  LED_DRIVER_CMD_REG             (0xFD)
#define	 LED_CONFIGURATION_REG			(0X00)
#define	 LED_SHUTDOWN_REG				(0X0A)
#define	 LED_PICTURE_DISPLAY_REG		(0X01)
#define	 LED_GCC_REG					(0X04)
#define	 LED_AudioSync_REG				(0X06)

#define LED_CONTROL_REG_ADDER	0x00
#define LED_BLINKCONTROL_REG_ADDER	0x12
#define LED_PWM_REG_ADDER	0x24





#define  I2C_COM_ERROR                  (0x00)    
#define  I2C_COM_SUCCESS                (0x01)    

#define	LED_PICTURE_MODE		(0X00)
#define LED_AUTOFRAMEPLAY_MODE	(0X01)

#define	LED_NORMAL_OPRATE_MODE		(0X01)
#define LED_SHUTDOWN_MODE			(0X00)

#define LED_PICTURE_DISPLAY_FRAME1 (0x00)
#define LED_PICTURE_DISPLAY_FRAME2 (0x01)
#define LED_PICTURE_DISPLAY_FRAME3 (0x02)
#define LED_PICTURE_DISPLAY_FRAME4 (0x03)
#define LED_PICTURE_DISPLAY_FRAME5 (0x04)
#define LED_PICTURE_DISPLAY_FRAME6 (0x05)
#define LED_PICTURE_DISPLAY_FRAME7 (0x06)
#define LED_PICTURE_DISPLAY_FRAME8 (0x07)



/*******************************************************************************
* 	Function Prototypes
*******************************************************************************/

uint8_t LedControl_Writebyte(uint8_t slaverAdr,uint8_t reg_addr,uint8_t data);
uint8_t LedControl_WriteArray(uint8_t slaverAdr,uint8_t* dataArray, uint32_t count);
void LedControl_Read(uint8_t slaverAdr,Point_Page page,uint8_t regAddr,uint8_t* dataArray ,uint32_t count);
void LedControl_EnableHardwareShutdown(void);
void LedControl_DisableHardwareShutdown(void);
void LedControl_Initial(void);




#endif /* #ifndef LED_CONTROL_H */


/* [] END OF FILE */

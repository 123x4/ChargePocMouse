/*----------------------------------------------------------------------------
*      Name : wnApi_Key_Controller.c
*      Note(s):
*----------------------------------------------------------------------------
* PLEASE READ THIS AS IS CLAUSE CAREFULLY. By accessing and using the Source Code, you accept this AS IS CLAUSE in 
* its entirety and agree to only use the Source Code in accordance with the *following terms and conditions. 
* If you do not wish to be bound by these terms and conditions, do not access or use the Source Code.

* The Source Code is provided to you "AS IS". YOU ASSUME TOTAL RESPONSIBILITY AND RISK FOR YOUR USE OF IT INCLUDING 
* THE RISK OF ANY DEFECTS OR INACCURACIES THEREIN. WPI DOES NOT *MAKE, AND EXPRESSLY DISCLAIMS, ANY EXPRESS OR IMPLIED
* WARRANTIES OF ANY KIND WHATSOEVER, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY OR 
* FITNESS FOR A PARTICULAR PURPOSE, WARRANTIES OF TITLE OR NON-INFRINGEMENT OF ANY IPR OR TRADEMARK RIGHTS, 
* ANY WARRANTIES ARISING BY USAGE OF TRADE, COURSE OF DEALING *OR COURSE OF PERFORMANCE, 
* OR ANY WARRANTY THAT THE SOURCE CODE IS "ERROR FREE" OR WILL MEET YOUR REQUIREMENTS.

* IN NO EVENT SHALL WPI BE LIABLE FOR (a) ANY INCIDENTAL, CONSEQUENTIAL, OR INDIRECT DAMAGES (INCLUDING, 
* WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS *INTERRUPTION, LOSS OF PROGRAMS OR INFORMATION,
* AND THE LIKE) ARISING OUT OF THE USE OF OR INABILITY TO USE THE SOURCE CODE , EVEN IF WPI OR ANY OF ITS AUTHORIZED
* REPRESENTATIVES HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, (b) ANY CLAIM ATTRIBUTABLE TO ERRORS,
* OMISSIONS, OR OTHER INACCURACIES IN THE SOURCE CODE, OR (c) ANY *CLAIM BY ANY THIRD PARTY. *
* Copyright (c) 2013 .  World Peace Industrial Co., LTD. All rights reserved.
*-----------------------------------------------------------------------------
*/


#ifndef __LED_DRIVER_C__
#define __LED_DRIVER_C__

#include "../inc/LPC11Uxx.h"
#include "../inc/GPIO.h"
#include "../inc/OSC.h"
#include "../inc/I2C.h"
#include "../inc/led_driver.h"
#include "type.h"

#define ZABMASK 0x01 //Nikita 20140224, 0x03
#define DEBOUNCE_TIME_COUNT 5
#define Addr_GND_GND 0xA0

#define No_Open 0
#define A_Open  1
#define B_Open  2
#define AB_Open 3


void Api_Led_Driver_IS31FL3732_Init(void)
{
//		DrvGPIOOutput(1,16,1);	// Reset pin hi
}



U8 RGBWCounter = 0;
//BOOLEAN TimeTickFlag;	//Nikita

//void wnApi_Led_Driver_AN32180_Handler(void)
//{		
//	if(TimeTickFlag == TRUE)
//	{
//		switch(RGBWCounter)
//		{	
//			case 0:
//				wnApi_Led_Driver_AN32180_LED_R();
//				RGBWCounter++;
//				break;	
//			case 1:
//				wnApi_Led_Driver_AN32180_LED_G();
//				RGBWCounter++;
//				break;
//			case 2:
//				wnApi_Led_Driver_AN32180_LED_B();
//				RGBWCounter++;
//			//RGBWCounter = 0; // Magicoe Testing
//				break;
//			case 3:
//				wnApi_Led_Driver_AN32180_LED_W();
//				RGBWCounter = 0;
//				break;				
//			case 4:
//				wnApi_Led_Driver_AN32180_LED_BLACK();
//				RGBWCounter = 0;
//			default:
//				break;
//		}
//		TimeTickFlag = FALSE;
//	}
//}
uint8_t Open_Num_Pro(uint8_t num)
{	
	if((num&0x0A)==0x0A)
	{
		return AB_Open;
	}
	if((num&0x02)==0x02)
	{
		return A_Open;
	}
	if((num&0x08)==0x08)
	{
		return B_Open;
	}

	return No_Open;
}

void Every_LED_Open_Test(void)
{
	uint8_t i = 0 ;

		I2CTransBuffer[0] = 0;
		wnDrv_wI2C_Write(Addr_GND_GND,0xFD,0x00,1);//Frame register   page1
		for(i=0x12;i<0x24;i++)
		{
			I2CTransBuffer[i]=0x00;
		}	
		wnDrv_wI2C_Write(Addr_GND_GND,0x12,0x00 , 0x24-0x12);//set Blink register
		for(i=0;i<0x12;i++)
		{
			I2CTransBuffer[i]=0;
		} 
		wnDrv_wI2C_Write(Addr_GND_GND,0x00,0x00,12);//turn off all LED LED contorl register
		for(i=0x24;i<0xB4;i++)
		{
			I2CTransBuffer[i]=0xFF;
		}	
		wnDrv_wI2C_Write(Addr_GND_GND,0x24,0x00 , 0xB4-0x24);//set PWM  PWM register

		
//		I2CTransBuffer[0]=0x0B;
//		wnDrv_wI2C_Write(Addr_GND_GND,0xFD,0x00,1);//write function register
//		
//		/***  config function register: 0x00~0x0C  ***/
//		for(i=0;i<=0x0C;i++){
//			I2CTransBuffer[0] = 0x00;
//			if(i!=0x04)
//				wnDrv_wI2C_Write(Addr_GND_GND,i,0x00,1);		
//		}
		
//		I2CTransBuffer[0] = 0x10;
//		wnDrv_wI2C_Write(Addr_GND_GND,0x0D,0x00,1);// !!!!!!!!!!!!!!!
		
		I2CTransBuffer[0] = 0x0B;
		I2CTransBuffer[1] = 0x00;
		I2CTransBuffer[2] = 0x01;	
		I2CTransBuffer[3] = 0xFF;
		wnDrv_wI2C_Write(Addr_GND_GND,0xFD,0x00,1);//write function register
		wnDrv_wI2C_Write(Addr_GND_GND,0x00,0x01,1);//picture mode
		wnDrv_wI2C_Write(Addr_GND_GND,0x0A,0x02,1);//normal operation		
		wnDrv_wI2C_Write(Addr_GND_GND,0x04,0x03,1);//Global current=2

		I2CTransBuffer[0] = 0x00;
		I2CTransBuffer[1] = 0x00;
		wnDrv_wI2C_Write(Addr_GND_GND,0xFD,0x00,1);//write function register
		for(i=0;i<18;i+=2){
			wnDrv_wI2C_Write(Addr_GND_GND,i,0x01,1);//turn on led  Matrix A 
			wnDrv_wI2C_Write(Addr_GND_GND,i+1,0x01,1);//turn on led  Matrix B 
			DrvDelay(20);//delay 2ms	
		}
		
//		wnDrv_wI2C_Write(Addr_GND_GND,0x0B,0x00,0);
	
//		wnDrv_wI2C_Read(Addr_GND_GND,0x0A,0x00,1);
//		if (I2CReceiverBuffer[0]==0x0a)
//		{
//			;
//		}
		
//		I2CTransBuffer[0] = 0xFF;
//		wnDrv_wI2C_Write(Addr_GND_GND,0x04,0x00,1);	  // set global current
//		
//		I2CTransBuffer[0] = 0x00;
//		wnDrv_wI2C_Write(Addr_GND_GND,0xFD,0x00,1);		
		
}

void LED_R(void)
{
	uint8_t i = 0;
	
//		I2CTransBuffer[0] = 0x0B;
//		I2CTransBuffer[1] = 0x08;  // auto frame play mode  frame1
//		I2CTransBuffer[2] = 0x00;  // frame select
//		I2CTransBuffer[3] = 0x01;	
//	  I2CTransBuffer[4] = 0x70;
//	  I2CTransBuffer[5] = 0x0a;
//		I2CTransBuffer[6] = 0xFF;
//	  I2CTransBuffer[7] = 0x42;
//	  I2CTransBuffer[8] = 0x14;

//		wnDrv_wI2C_Write(Addr_GND_GND,0xFD,0x00,1);//write function register
//		wnDrv_wI2C_Write(Addr_GND_GND,0x00,0x01,1);//picture mode
//  	wnDrv_wI2C_Write(Addr_GND_GND,0x01,0x02,1);//picture mode frame select
//		wnDrv_wI2C_Write(Addr_GND_GND,0x0A,0x03,1);//normal operation		
//		wnDrv_wI2C_Write(Addr_GND_GND,0x02,0x04,1);//4loops frame1
//	  wnDrv_wI2C_Write(Addr_GND_GND,0x03,0x05,1);//
//		wnDrv_wI2C_Write(Addr_GND_GND,0x04,0x06,1);//
//	  wnDrv_wI2C_Write(Addr_GND_GND,0x08,0x07,1);//
//	  wnDrv_wI2C_Write(Addr_GND_GND,0x09,0x08,1);

		I2CTransBuffer[0] = 0;
		wnDrv_wI2C_Write(Addr_GND_GND,0xFD,0x00 , 1);
		for(i=0x0;i<0x12;i+=2)
		{
			I2CTransBuffer[0]=0x55;
			wnDrv_wI2C_Write(Addr_GND_GND, i , 0x00 , 1); //set led control register
			wnDrv_wI2C_Write(Addr_GND_GND, i+1 ,0x00 , 1);//set led control register

		}	
		DrvDelay(3000);//delay 2ms	
		I2CTransBuffer[0] = 0;
		wnDrv_wI2C_Write(Addr_GND_GND,0xFD,0x00 , 1);
		for(i=0x0;i<0x12;i+=2)
		{
			I2CTransBuffer[0]=0xaa;
			wnDrv_wI2C_Write(Addr_GND_GND, i , 0x00 , 1); //set led control register
			wnDrv_wI2C_Write(Addr_GND_GND, i+1 ,0x00 , 1);//set led control register

		}	
	
//		I2CTransBuffer[0] = 0x00;
//		wnDrv_wI2C_Write(Addr_GND_GND,0xFD,0x00,1);//write function register
//		I2CTransBuffer[0] = 0xff;	
//		for(i=0;i<0x12;i+=1){
//			wnDrv_wI2C_Write(Addr_GND_GND,0x10,0x00,1);//turn on led  Matrix A 
////			wnDrv_wI2C_Write(Addr_GND_GND,i+1,0x00,1);//turn on led  Matrix B 
			DrvDelay(3000);//delay 2ms	
//		}
		
}

void LED_Blink(void)
{
	uint8_t i = 0;
	I2CTransBuffer[0] = 0x0B;
	wnDrv_wI2C_Write(Addr_GND_GND , 0xFD , 0x00 , 1);//write function register

	I2CTransBuffer[0] = 0x0C ;      								 //enable blink bit and set blink time 
	wnDrv_wI2C_Write(Addr_GND_GND , 0x05 , 0x00 , 1);//set blink register £ºEnable blink mode

	I2CTransBuffer[0] = 0x00 ;
	wnDrv_wI2C_Write(Addr_GND_GND , 0xFD , 0x00 , 1);//select page 0
	I2CTransBuffer[0] = 0x55;
	for(i=0x12;i<0x24;i++)
	{	
		wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); // 
	}
	
	
}

void LED_Ron(void)
{
 uint8_t i = 0,k=0;
	I2CTransBuffer[0] = 0x00;
	wnDrv_wI2C_Write(Addr_GND_GND , 0xFD , 0x00 , 1);//write function register
	I2CTransBuffer[0] = 0xff;
	for(i=0x24,k=0xA4;k<0xB4;i++,k++)
		{	
			if(i>=0x35) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1); 			
		}	
}

void LED_Gon(void)
{
 uint8_t i = 0,k=0;
	I2CTransBuffer[0] = 0x00;
	wnDrv_wI2C_Write(Addr_GND_GND , 0xFD , 0x00 , 1);//write function register
	I2CTransBuffer[0] = 0xff;
	
for(i=0x35,k=0x92 ; k<0xA4 ; i++,k++)
		{	
			if(i>=0x46) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1);
		}
		
}
void LED_Bon(void)
{
	 uint8_t i = 0,k=0;
	I2CTransBuffer[0] = 0x00;
	wnDrv_wI2C_Write(Addr_GND_GND , 0xFD , 0x00 , 1);//write function register
	I2CTransBuffer[0] = 0xff;
	
	for(i=0x46,k=0x79 ; k<0x92;i+=1,k+=1)
		{	
			if(i>=0x5E) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 	
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1); 
		}

}			 



  void LED_RGB(void)
{
	uint8_t i = 0,k=0;
	I2CTransBuffer[0] = 0x00;
	wnDrv_wI2C_Write(Addr_GND_GND , 0xFD , 0x00 , 1);//write function register
	I2CTransBuffer[0] = 0xff;
	for(i=0x24,k=0xA4;k<0xB4;i++,k++)
		{	
			if(i>=0x35) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1); 			
		}	
		DrvDelay(10);
//		I2CTransBuffer[0] = j;
		for(i=0x35,k=0x92 ; k<0xA4 ; i++,k++)
		{	
			if(i>=0x46) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1);
		}
		DrvDelay(10);//delay 
		for(i=0x46,k=0x79 ; k<0x92;i+=1,k+=1)
		{	
			if(i>=0x5E) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 	
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1); 
		}
		DrvDelay(10);//delay 	
		
}



void LED_off(void)
{
  uint8_t i = 0,k=0;
	I2CTransBuffer[0] = 0x00;
	wnDrv_wI2C_Write(Addr_GND_GND , 0xFD , 0x00 , 1);//write function register
	I2CTransBuffer[0] = 0x00;
	for(i=0x24,k=0xA4;k<0xB4;i++,k++)
		{	
			if(i>=0x35) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1); 			
		}
	for(i=0x35,k=0x92 ; k<0xA4 ; i++,k++)
		{	
			if(i>=0x46) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1);
		}
//		DrvDelay(10);//delay 
		for(i=0x46,k=0x79 ; k<0x92;i+=1,k+=1)
		{	
			if(i>=0x5E) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 	
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1); 
		}
//		DrvDelay(10);//delay 			
}


void LED_Breath(void)
{
	uint8_t i = 0,j=0,k=0;
	I2CTransBuffer[0] = 0x00;
	wnDrv_wI2C_Write(Addr_GND_GND , 0xFD , 0x00 , 1);//write function register
	
//	I2CTransBuffer[0] = 0x16 ;      								 //enable breath bit and set breath time 
//	wnDrv_wI2C_Write(Addr_GND_GND , 0x09 , 0x00 , 1);//set blink register £ºEnable blink mode

//	I2CTransBuffer[0] = 0x44 ;      								 //enable breath bit and set breath time 
//	wnDrv_wI2C_Write(Addr_GND_GND , 0x08 , 0x00 , 1);//set blink register £ºEnable blink mode
#if 1
	I2CTransBuffer[0] = 0x00 ;
	wnDrv_wI2C_Write(Addr_GND_GND , 0xFD , 0x00 , 1);//select page 0
	//  0x24->0x35 & 0xA4->0xB4 >>>>>LED_R   //   By Martin 2017.11.10
	#if 1
	for(j=0;j<0xff;j++){
		I2CTransBuffer[0] = j;
		for(i=0x24,k=0xA4;k<0xB4;i++,k++)
		{	
			if(i>=0x35) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1); 			
		}
//		DrvDelay(10);//delay 
	}
	for(j=0xfe;j>0;j--){
		I2CTransBuffer[0] = j;
		for(i=0x24,k=0xA4;k<0xB4;i++,k++)
		{	
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); // 
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1); 	
//			DrvDelay(1);//delay 
		}
//		DrvDelay(10);//delay 
	}
	#endif
	//  0x35->0x47 0x92->0xA4  >>>>>LED_G   //   By Martin 2017.11.10
	#if 0
	for(j=0;j<0xff;j++){
		I2CTransBuffer[0] = j;
		for(i=0x35,k=0x92 ; k<0xA4 ; i++,k++)
		{	
			if(i>=0x46) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1);
		}
		DrvDelay(1);//delay 
	}
	for(j=0xfe;j>0;j--){
		I2CTransBuffer[0] = j;
		for(i=0x35,k=0x92 ; k<0xA4; i++,k++)
		{	
			if(i>=0x46) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1);
		}
		DrvDelay(1);//delay 
	}
	#endif
	#if 0
	//  0x47->0x5E  0x79->0x92 >>>>>LED_B   //   By Martin 2017.11.10
	for(j=0;j<0xff;j++){
		I2CTransBuffer[0] = j;
		for(i=0x47,k=0x79 ; k<0x92;i+=1,k+=1)
		{	
			if(i>=0x5E) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 	
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1); 
		}
		DrvDelay(10);//delay 
	}
	for(j=0xfe;j>0;j--){
		I2CTransBuffer[0] = j;
		for(i=0x47,k=0x79 ; k<0x92;i+=1,k+=1)
		{	
			if(i>=0x5E) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1);
      wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1);			
//			DrvDelay(1);//delay 
		}
		DrvDelay(10);//delay 
	}
	#endif
	
	//  blue
//	for(j=0;j<0xff;j++){
//		I2CTransBuffer[0] = j;
//		for(i=0xA4;i<0xB4;i+=1)
//		{	
//			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); // 		
//		}
//		DrvDelay(10);//delay 
//	}
//	for(j=0xfe;j>0;j--){
//		I2CTransBuffer[0] = j;
//		for(i=0xA4;i<0xB4;i+=1)
//		{	
//			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); // 
////			DrvDelay(1);//delay 
//		}
//		DrvDelay(10);//delay 
//	}

//	DrvDelay(1000);//delay 
#endif
	
}



void LED_flash(void)
{
	uint8_t i = 0,j=0;
	I2CTransBuffer[0] = 0x00;
	wnDrv_wI2C_Write(Addr_GND_GND , 0xFD , 0x00 , 1);//write function register
	
	I2CTransBuffer[0] = 0x00 ;
	wnDrv_wI2C_Write(Addr_GND_GND , 0xFD , 0x00 , 1);//select page 0
	//  0x24->0x35 & 0xA4->0xB4 >>>>>LED_R   //   By Martin 2017.11.10
	#if 1
	for(j=0;j<0xff;j++){
		I2CTransBuffer[0] = j;
		for(i=0;i<17;i++)
		{		
			wnDrv_wI2C_Write(Addr_GND_GND , 0x24 , 0x00 , 1); //R      //LED18
			wnDrv_wI2C_Write(Addr_GND_GND , 0x92 , 0x00 , 1); //G
			wnDrv_wI2C_Write(Addr_GND_GND , 0x46+15 , 0x00 , 1); //B

			#if 1
			wnDrv_wI2C_Write(Addr_GND_GND , 0x25 , 0x00 , 1); //R      //LED18
			wnDrv_wI2C_Write(Addr_GND_GND , 0x35 , 0x00 , 1); //G
			wnDrv_wI2C_Write(Addr_GND_GND , 0x46+16 , 0x00 , 1); //B
		
			wnDrv_wI2C_Write(Addr_GND_GND , 0x26 , 0x00 , 1); //R       //LED19
			wnDrv_wI2C_Write(Addr_GND_GND , 0x36 , 0x00 , 1); //G
			wnDrv_wI2C_Write(Addr_GND_GND , 0x46 , 0x00 , 1); //B
			
			wnDrv_wI2C_Write(Addr_GND_GND , 0x27 , 0x00 , 1); //R         //LED20
			wnDrv_wI2C_Write(Addr_GND_GND , 0x37 , 0x00 , 1); //G
			wnDrv_wI2C_Write(Addr_GND_GND , 0x47 , 0x00 , 1); //B
			
			wnDrv_wI2C_Write(Addr_GND_GND , 0x28 , 0x00 , 1); //R         //LED21
			wnDrv_wI2C_Write(Addr_GND_GND , 0x38 , 0x00 , 1); //G
			wnDrv_wI2C_Write(Addr_GND_GND , 0x48 , 0x00 , 1); //B

			wnDrv_wI2C_Write(Addr_GND_GND , 0x29 , 0x00 , 1); //R         //LED22    
			wnDrv_wI2C_Write(Addr_GND_GND , 0x39 , 0x00 , 1); //G
			wnDrv_wI2C_Write(Addr_GND_GND , 0x49 , 0x00 , 1); //B
			
			wnDrv_wI2C_Write(Addr_GND_GND , 0x2A , 0x00 , 1); //R         //LED23   
			wnDrv_wI2C_Write(Addr_GND_GND , 0x3A , 0x00 , 1); //G
			wnDrv_wI2C_Write(Addr_GND_GND , 0x4A , 0x00 , 1); //B

			wnDrv_wI2C_Write(Addr_GND_GND , 0x2B , 0x00 , 1); //R         //LED24   
			wnDrv_wI2C_Write(Addr_GND_GND , 0x3B , 0x00 , 1); //G
			wnDrv_wI2C_Write(Addr_GND_GND , 0x4B , 0x00 , 1); //B
      #endif


//      wnDrv_wI2C_Write(Addr_GND_GND , 0x47 , 0x00 , 1); //B
			
			
//			wnDrv_wI2C_Write(Addr_GND_GND , 0x24 +1, 0x00 , 1); 
//			wnDrv_wI2C_Write(Addr_GND_GND , 0x35 +1, 0x00 , 1);
//      wnDrv_wI2C_Write(Addr_GND_GND , 0x47, 0x00 , 1);
		}
		DrvDelay(100);//delay 
	}
//	for(j=0xfe;j>0;j--){
//		I2CTransBuffer[0] = j;
//		for(i=0x24,k=0xA4;k<0xB4;i++,k++)
//		{	
//			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); // 
//			//wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1); 	
//		}
//		DrvDelay(10);//delay 
//	}
	#endif
	//  0x35->0x47 0x92->0xA4  >>>>>LED_G   //   By Martin 2017.11.10
	#if 0
	for(j=0;j<0xff;j++){
		I2CTransBuffer[0] = j;
		for(i=0x35,k=0x92 ; k<0xA4 ; i++,k++)
		{	
			if(i>=0x46) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1);
		}
		DrvDelay(10);//delay 
	}
	for(j=0xfe;j>0;j--){
		I2CTransBuffer[0] = j;
		for(i=0x35,k=0x92 ; k<0xA4; i++,k++)
		{	
			if(i>=0x46) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1);
		}
		DrvDelay(10);//delay 
	}
	#endif
	#if 0
	//  0x47->0x5E  0x79->0x92 >>>>>LED_B   //   By Martin 2017.11.10
	for(j=0;j<0xff;j++){
		I2CTransBuffer[0] = j;
		for(i=0x47,k=0x79 ; k<0x92;i+=1,k+=1)
		{	
			if(i>=0x5E) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1); 	
			wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1); 
		}
		DrvDelay(10);//delay 
	}
	for(j=0xfe;j>0;j--){
		I2CTransBuffer[0] = j;
		for(i=0x47,k=0x79 ; k<0x92;i+=1,k+=1)
		{	
			if(i>=0x5E) i=0;
			wnDrv_wI2C_Write(Addr_GND_GND , i , 0x00 , 1);
      wnDrv_wI2C_Write(Addr_GND_GND , k , 0x00 , 1);			
//			DrvDelay(1);//delay 
		}
		DrvDelay(10);//delay 
	}
	#endif

	DrvDelay(1000);//delay 
	
}




#undef INTERFACE
#endif/*End __WNAPI_KEY_CONTROLLER_C__*/


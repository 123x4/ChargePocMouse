/*******************************************************************************
 * @file    PAW3950.c
 * @brief
 * @author  chengjie
 * @date    created on 2026-05-08
 * @version V0.1
 * @note
*******************************************************************************/
#include "my_board.h"

#if (defined(IRPT_V4) || defined(V4_HALL_HPTIC_V4) || defined(GT_LINA_T1A_H5))

#include "PAW3950.h"




#define PAW3950_DelayMs(x)  MyTick_DelayMs(x)
#define PAW3950_DelayUs(x)  CT16B1_DelayUs(x)
#define PAW3950_Print(format, ...) InfoLog("[PAW3950]" format, ##__VA_ARGS__)

#define PAW3950_IO_NoPull(x)  do{ LPC_IOCON->x &= (~((0x01 << 3) | (0x01 << 4)));}while(0)
#define PAW3950_IO_CfgAsOutput(port, pin) do{LPC_GPIO->DIR[port] |= (1 << pin);}while(0)
#define PAW3950_IO_CfgAsInput(port, pin)  do{LPC_GPIO->DIR[port] &= ~(1 << pin);}while(0)
#define PAW3950_IO_HighLevel(port, pin)   do{LPC_GPIO->SET[port] = (1 << pin);}while(0)
#define PAW3950_IO_LowLevel(port, pin)    do{LPC_GPIO->CLR[port] = (1 << pin);}while(0)

#define PAW3950_USED_HW_RESET       0
//#define PAW3950_IO_NRESET_PORT    1
//#define PAW3950_IO_NRESET_PIN     19
//#define PAW3950_IO_NRESET         PIO1_19

//#define PAW3950_IO_NCS_PORT    0
//#define PAW3950_IO_NCS_PIN     20
//#define PAW3950_IO_NCS         PIO0_20

//#define PAW3950_IO_MOTION_PORT    0
//#define PAW3950_IO_MOTION_PIN     2
//#define PAW3950_IO_MOTION         PIO0_2

#if PAW3950_USED_HW_RESET
#define PAW3950_IO_NRESET_PORT    1
#define PAW3950_IO_NRESET_PIN     19
#define PAW3950_IO_NRESET         PIO1_19
#endif

#define PAW3950_IO_NCS_PORT    NCS_PORT
#define PAW3950_IO_NCS_PIN     NCS_PIN
#define PAW3950_IO_NCS         NCS

#define PAW3950_IO_MOTION_PORT    MOTION_PORT
#define PAW3950_IO_MOTION_PIN     MOTION_PIN
#define PAW3950_IO_MOTION         MOTION

#define PAW3950_spi_put( addr, data )  SPI_Write((addr ), (data))//SPI_Write( (addr | 0x80), (data) )
#define PAW3950_spi_get( addr )        SPI_Read (addr)

static uint8_t initialized = 0;
const uint8_t regSettingOfInit0[][2] =
{
  {0x7F, 0x07},
  {0x40, 0x41},
  {0x41, 0x0F},
  {0x42, 0x28},
  {0x43, 0x00},
  {0x4A, 0x08},
  {0x51, 0x16},
  {0x53, 0x04},
  {0x7F, 0x00},
};

const uint8_t regSettingOfInit1[][2] =
{
  {0x7F, 0x0E},
  {0x55, 0xFC},
  {0xFC, 0xFF},
  {0x7F, 0x01},
  {0x43, 0xF8},
  {0x55, 0x28},
  {0x57, 0x1A},
  {0x5A, 0x20},
  {0x5B, 0x0C},
  {0x7F, 0x05},
  {0x46, 0x1B},
  {0x5F, 0x90},
  {0x61, 0x31},
  {0x6D, 0xB8},
  {0x6E, 0xCF},
  {0x7F, 0x06},
  {0x6B, 0x20},
  {0x6D, 0x8F},
  {0x44, 0x23},
  {0x46, 0xB9},
  {0x53, 0x03},
  {0x61, 0x00},
  {0x63, 0x00},
  {0x7F, 0x08},
  {0x63, 0x08},
  {0x6D, 0x68},
  {0x75, 0x2C},
  {0x79, 0xC8},
  {0x7A, 0x10},
  {0x7C, 0x34},
  {0x7F, 0x09},
  {0x73, 0x0C},
  {0x7F, 0x0A},
  {0x48, 0x80},
  {0x50, 0x25},
  {0x52, 0x40},
  {0x53, 0x1F},
  {0x56, 0x90},
  {0x57, 0x01},
  {0x4F, 0xD0},
  {0x59, 0x3F},
  {0x7F, 0x0C},
  {0x4E, 0x0A},
  {0x48, 0x0A},
  {0x42, 0x00},
  {0x4C, 0x4F},
  {0x54, 0x1A},
  {0x61, 0x18},
  {0x62, 0x18},
  {0x7F, 0x0D},
  {0x50, 0x66},
  {0x5A, 0x47},
  {0x5B, 0x66},
  {0x5C, 0xCB},
  {0x5D, 0x6C},
  {0x71, 0xDC},
  {0x70, 0x07},
  {0x73, 0x16},
  {0x72, 0x08},
  {0x75, 0xDC},
  {0x74, 0x07},
  {0x77, 0x16},
  {0x76, 0x08},
  {0x7F, 0x10},
  {0x42, 0x1C},
  {0x43, 0xD0},
  {0x45, 0x11},
  {0x46, 0x20},
  {0x48, 0x0A},
  {0x49, 0x0F},
  {0x4B, 0x09},
  {0x4C, 0xC4},
  {0x7F, 0x14},
  {0x43, 0x73},
  {0x52, 0x19},
  {0x62, 0x14},
  {0x63, 0x04},
  {0x65, 0x81},
  {0x53, 0x50},
  {0x54, 0xD8},
  {0x7F, 0x15},
  {0x41, 0xA2},
  {0x4C, 0x04},
  {0x56, 0x28},
  {0x6F, 0x28},
  {0x70, 0x04},
  {0x71, 0x04},
  {0x77, 0x02},
  {0x7D, 0x10},
  {0x45, 0xF4},
  {0x47, 0x44},
  {0x4B, 0x12},
  {0x7F, 0x16},
  {0x57, 0x43},
  {0x58, 0x43},
  {0x59, 0x43},
  {0x5A, 0x43},
  {0x5D, 0x32},
  {0x5E, 0x32},
  {0x69, 0x1E},
  {0x6A, 0x22},
  {0x6B, 0x22},
  {0x6C, 0x22},
  {0x6D, 0x22},
  {0x6E, 0x18},
  {0x6F, 0x18},
  {0x70, 0x18},
  {0x71, 0x18},
  {0x73, 0x00},
  {0x74, 0xDA},
  {0x75, 0xBE},
  {0x78, 0x0E},
  {0x7F, 0x18},
  {0x41, 0x02},
  {0x43, 0x80},
  {0x44, 0x1F},
  {0x45, 0x01},
  {0x46, 0x82},
  {0x48, 0x65},
  {0x4B, 0x00},
  {0x4C, 0x2C},
  {0x4D, 0x38},
  {0x4E, 0x32},
  {0x4F, 0x7F},
  {0x50, 0x34},
  {0x51, 0x54},
  {0x52, 0x38},
  {0x54, 0x02},
  {0x5B, 0x1E},
  {0x5C, 0x60},
  {0x5D, 0x06},
  {0x5E, 0x02},
  {0x5F, 0x48},
  {0x60, 0x44},
  {0x61, 0x1C},
  {0x62, 0x60},
  {0x63, 0xFF},
  {0x64, 0x00},
  {0x65, 0x75},
  {0x67, 0x4C},
  {0x69, 0x85},
  {0x6B, 0x60},
  {0x6D, 0xF5},
  {0x6E, 0x06},
  {0x6F, 0x06},
  {0x70, 0x22},
  {0x71, 0xF2},
  {0x72, 0xF6},
  {0x76, 0x00},
  {0x77, 0x32},
  {0x78, 0x88},
  {0x79, 0xCC},
  {0x7A, 0x00},
  {0x7E, 0xB0},
  {0x7F, 0x19},
  {0x40, 0x43},
  {0x41, 0x32},
  {0x44, 0x32},
  {0x46, 0xE6},
  {0x4B, 0x01},
  {0x4C, 0x01},
  {0x52, 0x15},
  {0x53, 0x12},
  {0x54, 0x23},
  {0x56, 0x78},
  {0x5F, 0x7F},
  {0x6E, 0x8F},
  {0x7F, 0x00},
  {0x5B, 0x40},
  {0x47, 0x01},
  {0x57, 0x20},
  {0x64, 0x08},
  {0x65, 0x07},
  {0x7F, 0x07},
  {0x40, 0x40},
  {0x7F, 0x00},
  {0x40, 0x80},
  {0x55, 0x02},
  {0x23, 0x70},
  {0x22, 0x01}
};

const uint8_t regSettingOfInit2[][2] =
{
  {0x22, 0x00},
  {0x55, 0x00},
  {0x40, 0x00},
  {0x68, 0x01},
};



static int32_t PA3950_ModuleInit(void)
{
  // 1. GPIO
  #if PAW3950_USED_HW_RESET
  // NRESET
  PAW3950_IO_NoPull(PAW3950_IO_NRESET);
  PAW3950_IO_CfgAsOutput(PAW3950_IO_NRESET_PORT, PAW3950_IO_NRESET_PIN);
  PAW3950_IO_HighLevel(PAW3950_IO_NRESET_PORT, PAW3950_IO_NRESET_PIN);
  #endif
  //NSC
  PAW3950_IO_NoPull(PAW3950_IO_NCS);
  PAW3950_IO_CfgAsOutput(PAW3950_IO_NCS_PORT, PAW3950_IO_NCS_PIN);
  PAW3950_IO_HighLevel(PAW3950_IO_NCS_PORT, PAW3950_IO_NCS_PIN);
  //MOTION
  PAW3950_IO_NoPull(PAW3950_IO_MOTION);
  PAW3950_IO_CfgAsInput(PAW3950_IO_MOTION_PORT, PAW3950_IO_MOTION_PIN);
  
  //2. SPI
  #if (defined(GT_LINA_T1A_H5))
  //spi0
    //-----SPI0-----
    LPC_SYSCON->PRESETCTRL |= (0x1<<0) ;                            //SPI0 Reset De-Asserted
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<11) ;                          //Setup AHB Clock To The SPI0 Domain 
    LPC_SYSCON->SSP0CLKDIV = 0x1 ;                                  //SPI0 Clock Divider 1(48MHz/1=48MHz)
	//-----GPIO TO SPI0-----
    LPC_IOCON->PIO0_8           &= ~0x07 ;                          //Select GPIO0_8 To MISO0
    LPC_IOCON->PIO0_8           |= 0x01 ;
    
    LPC_IOCON->PIO0_9           &= ~0x07 ;                          //Select GPIO0_9 To MOSI0
    LPC_IOCON->PIO0_9           |= 0x01 ;


	LPC_IOCON->SWCLK_PIO0_10	&= ~0x07 ;							//Select GPIO0_10 To CLK0
	LPC_IOCON->SWCLK_PIO0_10	|= 0x02 ;

		
	LPC_SSP0->CR0 = 0x01C7  ;                                        //CPOL = 1 CPHA = 1 8Bits
    LPC_SSP0->CPSR = 0x18 ;
    //SPI SPEED = ((MCU_OSC/SSP1CLKDIV)/(CPSR*[CR0 High Byte+1]))
    //((48MHz/1)/(24*[1+1])) = 1Mhz
    LPC_SSP0->CR1 = SSPCR1_SSE ;                                    //Enable SSP0
  #else
  //-----SPI1-----
  LPC_SYSCON->PRESETCTRL |= (0x1<<2) ;                            //SPI1 Reset De-Asserted
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<18) ;                          //Setup AHB Clock To The SPI1 Domain
  LPC_SYSCON->SSP1CLKDIV = 0x01 ;                                 //SPI1 Clock Divider 1(48MHz/1=48MHz)
	//-----GPIO TO SPI1-----
   LPC_IOCON->PIO0_21			&= ~0x07 ;							//Select GPIO0_21 To MOSI1
   LPC_IOCON->PIO0_21			|= 0x02 ;

   LPC_IOCON->PIO0_22			&= ~0x07 ;							//Select GPIO0_22 To MISO1
   LPC_IOCON->PIO0_22			|= 0x03 ;

   LPC_IOCON->PIO1_15			&= ~0x07 ;							//Select GPIO1_15 To SCK1
   LPC_IOCON->PIO1_15			|= 0x03 ;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  LPC_SSP1->CR0 = 0x01C7  ;                                        //CPOL = 1 CPHA = 1 8Bits
  LPC_SSP1->CPSR = 0x18 ;//0x06
  //SPI SPEED = ((MCU_OSC/SSP1CLKDIV)/(CPSR*[CR0 High Byte+1]))
  //((48MHz/1)/(24*[1+1])) = 1Mhz

//  for ( uint8_t i = 0; i < 8; i++ )
//  {
//      uint32_t dummy = LPC_SSP1->DR ;
//  }
  LPC_SSP1->CR1 |= SSPCR1_SSE ;                               //Enable SSP1
  #endif
  
  return 0;

}
static void PAW3950_ModuleExit(void)
{
  ;
}



int32_t PAW3950_Init(void)
{
  uint8_t tmp = 0;
  uint8_t regNum = 0;
  
  initialized = 0;
  if(PA3950_ModuleInit())
  {
    PAW3950_Print("Failed to init.");
    return -1;
  }
  //0.
  PAW3950_DelayMs(50); //Wait for at least 50ms
  //1. ncs reset the spi prot
  PAW3950_IO_HighLevel(PAW3950_IO_NCS_PORT, PAW3950_IO_NCS_PIN);
  PAW3950_DelayMs(1);
  PAW3950_IO_LowLevel(PAW3950_IO_NCS_PORT, PAW3950_IO_NCS_PIN);
  //2. chip reset
  #if PAW3950_USED_HW_RESET
  PAW3950_IO_LowLevel(PAW3950_IO_NRESET_PORT, PAW3950_IO_NRESET_PIN);
  PAW3950_DelayMs(1); //Reset, wait for at least 100ns
  PAW3950_IO_HighLevel(PAW3950_IO_NRESET_PORT, PAW3950_IO_NRESET_PIN);
  PAW3950_DelayMs(5); //Wait for at least 5ms
  #else //software reset
  PAW3950_Write(0x3A, 0x5A); //Write 0x5A to Register 0x3A(Power_Up_Reset register)
  PAW3950_DelayMs(5); //Wait for at least 5ms
  #endif
  
  //3. read Product_ID register
  for(uint8_t i = 0; i < 3; i++)
  {
    PAW3950_Read(0x00, &tmp, 1);
  }
  if(tmp != 0x53)
  {
    PAW3950_Print("Error. Init fail! reg0x00: 0x%02X", tmp);
    return -1;
  }
  PAW3950_Print("Init success reg0x00: 0x%02X", tmp);
  
  //4. Initialzation register setting
  regNum = sizeof(regSettingOfInit0) / sizeof(regSettingOfInit0[0][0]) / 2;
  for(uint8_t i = 0; i < regNum; i++)
  {
    PAW3950_Write(regSettingOfInit0[i][0], regSettingOfInit0[i][1]);
  }
  PAW3950_DelayMs(1); // wait for at lease 1ms
  
  regNum = sizeof(regSettingOfInit1) / sizeof(regSettingOfInit1[0][0]) / 2;
  for(uint8_t i = 0; i < regNum; i++)
  {
    PAW3950_Write(regSettingOfInit1[i][0], regSettingOfInit1[i][1]);
  }
  PAW3950_DelayMs(1); // wait for at lease 1ms
  
  for(uint8_t i = 0; i < 45; i++)
  {
    PAW3950_Read(0x6C, &tmp, 1);
    if(tmp == 0x80)
    {
      PAW3950_Print("count: %u", i);
      break;
    }
    PAW3950_DelayUs(990);
  }
  PAW3950_DelayMs(5); // wait for at lease 5ms
  
  regNum = sizeof(regSettingOfInit2) / sizeof(regSettingOfInit2[0][0]) / 2;
  for(uint8_t i = 0; i < regNum; i++)
  {
    PAW3950_Write(regSettingOfInit2[i][0], regSettingOfInit2[i][1]);
  }
  
  //5. Read to clear output registers
  PAW3950_Read(0x02, &tmp, 1);
  PAW3950_Read(0x03, &tmp, 1);
  PAW3950_Read(0x04, &tmp, 1);
  PAW3950_Read(0x05, &tmp, 1);
  PAW3950_Read(0x06, &tmp, 1);
  
  //6. set cpi
  if(PAW3950_SetCPI(1600))
  {
    return -2;
  }

  initialized = 1;
  return 0;
}

void PAW3950_Exit(void)
{
  PAW3950_ModuleExit();
}



int32_t PAW3950_Write(uint8_t regAddr, uint8_t data)
{
  uint8_t aData[2];
  
  aData[0] = regAddr | (1 << 7);
  aData[1] = data;

  PAW3950_IO_LowLevel(PAW3950_IO_NCS_PORT, PAW3950_IO_NCS_PIN);
  PAW3950_DelayUs(3);
  #if (defined(GT_LINA_T1A_H5))
    SSP_Send(0, &aData[0], 1);
  SSP_Send(0, &aData[1], 1);
  #else
  SSP_Send(1, &aData[0], 1);
  SSP_Send(1, &aData[1], 1);
  #endif
  PAW3950_DelayUs(3);
  PAW3950_IO_HighLevel(PAW3950_IO_NCS_PORT, PAW3950_IO_NCS_PIN);
  PAW3950_DelayUs(3);
  
  return 0;
}

int32_t PAW3950_Read(uint8_t regAddr, uint8_t *pData, uint16_t length)
{
  uint8_t addr;
  
  addr = regAddr & (~(1 << 7));

  PAW3950_IO_LowLevel(PAW3950_IO_NCS_PORT, PAW3950_IO_NCS_PIN);
  PAW3950_DelayUs(3);
  #if (defined(GT_LINA_T1A_H5))
    SSP_Send(0, &addr, 1);
  #else
  SSP_Send(1, &addr, 1);
  #endif
  PAW3950_DelayUs(3);
  
  for(uint16_t i = 0; i < length; i++)
  {
    #if (defined(GT_LINA_T1A_H5))
    SSP_Receive(0, pData, 1);
    #else
    SSP_Receive(1, pData, 1);
    #endif
  }
  
  PAW3950_IO_HighLevel(PAW3950_IO_NCS_PORT, PAW3950_IO_NCS_PIN);
  PAW3950_DelayUs(3);
  
  return 0;
}

int32_t PAW3950_SetCPI(uint16_t cpi)
{
  if(cpi > 30000 || cpi < 50)
  {
    return -1;
  }
  
  uint8_t highByte, lowByte;
  uint16_t tmp;
  
  tmp = cpi - 50;
  tmp = tmp / 50;
  highByte = tmp >> 8;
  lowByte = (uint8_t)tmp;
  
  uint8_t readTmp[4] = {0};
  for(uint8_t i = 0; i < 3; i++)
  {
    PAW3950_Write(0x48, lowByte);
    PAW3950_Write(0x49, highByte);
    PAW3950_Write(0x4A, lowByte);
    PAW3950_Write(0x4B, highByte);
    PAW3950_Write(0x47, 0x01);
    
    PAW3950_Read(0x48, &readTmp[0], 1);
    PAW3950_Read(0x49, &readTmp[1], 1);
    PAW3950_Read(0x4A, &readTmp[2], 1);
    PAW3950_Read(0x4B, &readTmp[3], 1);
    
    if(readTmp[0] == lowByte && readTmp[1] == highByte \
        && readTmp[2] == lowByte && readTmp[3] == highByte)
    {
      PAW3950_Print("CPI set successfully! cpi: %u", cpi);
      return 0;
    }
  }
  
  PAW3950_Print("Failed to Set cpi!");
  return -2;
}

static int32_t PAW3950_GetMotionDataSingleRead(int16_t *pX, int16_t *pY)
{
  uint8_t xLow = 0;
  uint8_t xHigh = 0;
  uint8_t yLow = 0;
  uint8_t yHigh = 0;
  uint16_t tmp = 0;
  uint8_t motionFlag = 0;
  PAW3950_Read(0x02, &motionFlag, 1);
  
  if(motionFlag & (0x01 << 7))
  {
    PAW3950_Read(0x03, &xLow, 1);
    PAW3950_Read(0x04, &xHigh, 1);
    PAW3950_Read(0x05, &yLow, 1);
    PAW3950_Read(0x06, &yHigh, 1);
    
    tmp = xHigh;
    tmp <<= 8;
    tmp |= xLow;
    *pX = tmp;
    
    tmp = yHigh;
    tmp <<= 8;
    tmp |= yLow;
    *pY = tmp;
    
    //PAW3950_Print("x: %d, y: %d", *pX, *pY);
    return 0;
  }
  
  return -1;
}

//static int32_t PAW3950_GetMotionDataBurstRead(int16_t *pX, int16_t *pY)
//{
//  ;
//}


int32_t PAW3950_GetMotionData(int16_t *pX, int16_t *pY)
{
  if(initialized)
  {
    return PAW3950_GetMotionDataSingleRead(pX, pY);
  }
  else
  {
    return -1;
  }
}

#endif







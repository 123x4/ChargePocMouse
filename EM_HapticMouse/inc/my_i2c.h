/*******************************************************************************
 * @file    my_i2c.h
 * @brief
 * @author  chengjie
 * @date    created on 2026-01-05
 * @version V1.0
 * @note
*******************************************************************************/
#ifndef __my_i2c_h__
#define __my_i2c_h__

#include "../inc/LPC11Uxx.h"
#include "device.h"
#include "GPIO.h"

//LPC_GPIO->DIR[Port] |= (1<<Set) ;       //Set to Output
//LPC_GPIO->DIR[Port] &= ~(1<<Set) ;      //Set to Input

#define ACTIVE_I2C2

#define I2C_SCL_PORT    0
#define I2C_SCL_PIN     4
#define I2C_SDA_PORT    0
#define I2C_SDA_PIN     5


#ifdef ACTIVE_I2C2 
#define I2C2_SCL_PORT    0
#define I2C2_SCL_PIN     17
#define I2C2_SDA_PORT    0
#define I2C2_SDA_PIN     18
#endif
/******IO config**********/
#define IOCON_REGISTER(xPort, xPin) \
          (*((uint32_t *)(LPC_IOCON_BASE + ((xPort) * 0x60) + ((xPin) * 0x04))))
#define SET_IO_NO_PULL(xPort, xPin) \
          IOCON_REGISTER(xPort, xPin) &= (~((0x01 << 3) | (0x01 << 4)))//no pull-up and no pull-down resistor
#define SET_IO_OD_MODE(xPort, xPin) IOCON_REGISTER(xPort, xPin) |= (0x01 << 10)//open drain

/******SCL config**********/
#define SET_SCL_NO_PULL(x)      SET_IO_NO_PULL(x->sclPort, x->sclPin)
#define SET_SCL_OD_MODE(x)      SET_IO_OD_MODE(x->sclPort, x->sclPin)
#define SET_SCL_INPUTMODE(x)    LPC_GPIO->DIR[x->sclPort] &= ~(1 << (x->sclPin))
#define SET_SCL_OUTPUTMODE(x)   LPC_GPIO->DIR[x->sclPort] |= (1 << (x->sclPin))
#define SET_I2C_SCL_HIGH(x)     LPC_GPIO->SET[x->sclPort] = 1 << (x->sclPin)
#define CLR_I2C_SCL_LOW(x)      LPC_GPIO->CLR[x->sclPort] = 1 << (x->sclPin)
/******SDA config**********/
#define SET_SDA_NO_PULL(x)      SET_IO_NO_PULL(x->sdaPort, x->sdaPin)
#define SET_SDA_OD_MODE(x)      SET_IO_OD_MODE(x->sdaPort, x->sdaPin)
#define SET_SDA_INPUTMODE(x)    LPC_GPIO->DIR[x->sdaPort] &= ~(1 << (x->sdaPin))
#define SET_SDA_OUTPUTMODE(x)   LPC_GPIO->DIR[x->sdaPort] |= (1 << (x->sdaPin))
#define SET_I2C_SDA_HIGH(x)     LPC_GPIO->SET[x->sdaPort] = 1 << (x->sdaPin)
#define CLR_I2C_SDA_LOW(x)      LPC_GPIO->CLR[x->sdaPort] = 1 << (x->sdaPin)

#define I2C_SDA_READ(x)         DrvGPIORead(x->sdaPort, (x->sdaPin))
#define soft_i2c_delay()        nsAPI_SSP_Delay1us(5)

typedef struct 
{
  uint8_t sclPort;
  uint8_t sclPin;
  uint8_t sdaPort;
  uint8_t sdaPin;
}I2C_GPIOType_t;

extern I2C_GPIOType_t handleI2c1;
#ifdef ACTIVE_I2C2 
extern I2C_GPIOType_t handleI2c2;
#endif

void soft_i2c_init(I2C_GPIOType_t *hi2c);
uint8_t soft_i2c_write_fast(I2C_GPIOType_t *hi2c, uint8_t dev_addr_7bit,
                      uint8_t reg_addr, uint8_t const * writebuff, int length);
uint8_t soft_i2c_read_fast(I2C_GPIOType_t *hi2c, uint8_t dev_addr_7bit,
                        uint8_t reg_addr, uint8_t * const readbuff, int length);



#endif /* __my_i2c_h__ */

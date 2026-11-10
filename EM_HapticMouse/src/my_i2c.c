/*******************************************************************************
 * @file    my_i2c.c
 * @brief
 * @author  chengjie
 * @date    created on 2026-01-05
 * @version V1.0
 * @note
*******************************************************************************/



#include "my_i2c.h"

I2C_GPIOType_t handleI2c1 = {
.sclPort = I2C_SCL_PORT, 
.sclPin = I2C_SCL_PIN, 
.sdaPort = I2C_SDA_PORT, 
.sdaPin = I2C_SDA_PIN
};

#ifdef ACTIVE_I2C2 
I2C_GPIOType_t handleI2c2 = {
.sclPort = I2C2_SCL_PORT, 
.sclPin = I2C2_SCL_PIN, 
.sdaPort = I2C2_SDA_PORT, 
.sdaPin = I2C2_SDA_PIN
};
#endif



void soft_i2c_start(I2C_GPIOType_t *hi2c)
{
  SET_I2C_SDA_HIGH(hi2c);
  SET_I2C_SCL_HIGH(hi2c);
  soft_i2c_delay();
  CLR_I2C_SDA_LOW(hi2c);
  soft_i2c_delay();
  
  CLR_I2C_SCL_LOW(hi2c);
  SET_I2C_SDA_HIGH(hi2c);
  //soft_i2c_delay();
}


void soft_i2c_stop(I2C_GPIOType_t *hi2c)
{
  CLR_I2C_SCL_LOW(hi2c);
  CLR_I2C_SDA_LOW(hi2c);
  soft_i2c_delay();
  SET_I2C_SCL_HIGH(hi2c);
  soft_i2c_delay();
  SET_I2C_SDA_HIGH(hi2c);
  soft_i2c_delay();
}


uint8_t soft_i2c_write_byte(I2C_GPIOType_t *hi2c, uint8_t byte)
{
  uint8_t i;
  uint8_t ack;
  
  for (i = 0; i < 8; i++)
  {
    CLR_I2C_SCL_LOW(hi2c);
    
    if (byte & 0x80)
    {
      SET_I2C_SDA_HIGH(hi2c);
    }
    else
    {
      CLR_I2C_SDA_LOW(hi2c);
    }
    
    soft_i2c_delay();
    SET_I2C_SCL_HIGH(hi2c);
    soft_i2c_delay();
    byte <<= 1;
  }
  
  CLR_I2C_SCL_LOW(hi2c);
  SET_I2C_SDA_HIGH(hi2c);
  SET_SDA_INPUTMODE(hi2c);
  soft_i2c_delay();
  
  SET_I2C_SCL_HIGH(hi2c);
  soft_i2c_delay();

  //SET_SDA_INPUTMODE(hi2c);

  ack = I2C_SDA_READ(hi2c);

  //SET_SDA_OUTPUTMODE(hi2c);

  CLR_I2C_SCL_LOW(hi2c);
  SET_SDA_OUTPUTMODE(hi2c);
  SET_I2C_SDA_HIGH(hi2c);
  soft_i2c_delay();
  
  return (ack == 0) ? 0 : 1;
}


uint8_t soft_i2c_read_byte(I2C_GPIOType_t *hi2c, uint8_t ack)
{
  uint8_t i;
  uint8_t byte = 0;

  //SET_I2C_SDA_HIGH(hi2c); 
  //SET_SDA_INPUTMODE(hi2c);
  CLR_I2C_SCL_LOW(hi2c);
  SET_I2C_SDA_HIGH(hi2c); 
  SET_SDA_INPUTMODE(hi2c);

  for (i = 0; i < 8; i++)
  {
    byte <<= 1;
    CLR_I2C_SCL_LOW(hi2c);
    soft_i2c_delay();
    SET_I2C_SCL_HIGH(hi2c);
    soft_i2c_delay();
    
    if (I2C_SDA_READ(hi2c))
    {
      byte |= 0x01;
    }
  }

  CLR_I2C_SCL_LOW(hi2c);
  SET_SDA_OUTPUTMODE(hi2c);
  if (ack) 
  {
    CLR_I2C_SDA_LOW(hi2c);//ACK
  } 
  else 
  {
    SET_I2C_SDA_HIGH(hi2c);//NACK
  }
  soft_i2c_delay();
  
  SET_I2C_SCL_HIGH(hi2c);
  soft_i2c_delay();
  
  CLR_I2C_SCL_LOW(hi2c);
  SET_I2C_SDA_HIGH(hi2c);
  soft_i2c_delay();//
  
  //SET_I2C_SDA_HIGH(hi2c);

  return byte;
}



void soft_i2c_init(I2C_GPIOType_t *hi2c)
{
  SET_SDA_NO_PULL(hi2c);
  SET_SDA_OD_MODE(hi2c);
  SET_SDA_OUTPUTMODE(hi2c);
  SET_I2C_SDA_HIGH(hi2c);
  
  SET_SCL_NO_PULL(hi2c);
  SET_SCL_OD_MODE(hi2c);
  SET_SCL_OUTPUTMODE(hi2c);
  SET_I2C_SCL_HIGH(hi2c);
  
  soft_i2c_delay();
}


uint8_t soft_i2c_write_fast(I2C_GPIOType_t *hi2c, uint8_t dev_addr_7bit,
                        uint8_t reg_addr, uint8_t const * writebuff, int length) 
{
  soft_i2c_start(hi2c);
  if (soft_i2c_write_byte(hi2c, dev_addr_7bit << 1))
  {
    soft_i2c_stop(hi2c);
    return 1;
  }

  
  if (soft_i2c_write_byte(hi2c, reg_addr)) 
  {
    soft_i2c_stop(hi2c);
    return 1;
  }

  for (int i = 0; i < length; i++) 
  {
    if (soft_i2c_write_byte(hi2c, writebuff[i]))
    {
      soft_i2c_stop(hi2c);
      return 1;
    }
  }
  soft_i2c_stop(hi2c);
  return 0;
}

uint8_t soft_i2c_read_fast(I2C_GPIOType_t *hi2c, uint8_t dev_addr_7bit,
                          uint8_t reg_addr, uint8_t* const readbuff, int length) 
{
  soft_i2c_start(hi2c);
  if (soft_i2c_write_byte(hi2c, dev_addr_7bit << 1))
  {
    soft_i2c_stop(hi2c);
    return 1;
  }

  if (soft_i2c_write_byte(hi2c, reg_addr))
  {
    soft_i2c_stop(hi2c);
    return 1;
  }

  soft_i2c_start(hi2c);
  if (soft_i2c_write_byte(hi2c, (dev_addr_7bit << 1) | 0x01))
  {
    soft_i2c_stop(hi2c);
    return 1;
  }

  for (int i = 0; i < length; i++)
  {
    readbuff[i] = soft_i2c_read_byte(hi2c, i == length - 1 ? 0 : 1);
  }

  soft_i2c_stop(hi2c);
  return 0;
}




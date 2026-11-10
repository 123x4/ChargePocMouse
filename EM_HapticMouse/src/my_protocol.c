/*******************************************************************************
 * @file    my_protocol.c
 * @brief
 * @author  chengjie
 * @date    created on 2026-04-07
 * @version V1.0
 * @note
*******************************************************************************/
#include "my_protocol.h"

uint16_t Protocol_ConvertWord(uint16_t value)
{
  uint16_t tmp = 0;
  uint32_t i = 1;
  
  if(*(uint8_t*)&i) // This machine is Little-endian.
  {
    tmp = value;
  }
  else // This machine is Big-endian.
  {
    tmp = value;
    tmp = ((tmp << 8) & 0xFF00) | ((tmp >> 8) & 0x00FF);
  }
  
  return tmp;
}

static uint8_t CalculateSum(uint8_t *p, uint16_t num)
{
  uint8_t sum = 0;
  
  for(uint16_t i = 0; i < num; i++)
  {
    sum += *(p + i);
  }
  
  return sum;
}

uint8_t Protocol_CalculateChecksum(Packet_ts *p)
{
  PacketHeader_ts *pxPackHeader = &(p->xPacketHeader);
  uint8_t tmp = 0;
  
  tmp += pxPackHeader->bDestination;
  tmp += pxPackHeader->bSource;
  tmp += (uint8_t)(pxPackHeader->wPayLoadLength);
  tmp += (uint8_t)((pxPackHeader->wPayLoadLength) >> 8);
  
  if(pxPackHeader->wPayLoadLength)
  {
    tmp += CalculateSum((uint8_t*)(pxPackHeader + 1), Protocol_ConvertWord(pxPackHeader->wPayLoadLength));
  }
  
  return tmp;
}

/*********************Example Code Begin****************************************
int8_t Protocol_ProcessReceivePayload(Payload_ts *pxPayload, uint16_t payloadLen)
{
  // Process Data
  ;
  return 0;
}

int8_t Protocol_ProcessReceivePacket(Packet_ts *pxPacket)
{
  uint16_t payloadLen = 0;
  
  if(!pxPacket)
  {
      return -1;
  }
  
  payloadLen = Protocol_ConvertWord(pxPacket->xPacketHeader.wPayLoadLength);
  
  if(payloadLen)
  { // Not Empty Payload.
    Protocol_ProcessReceivePayload(&(pxPacket->xPayload), payloadLen);
  }
  
  return 0;
}


int8_t Protocol_ProcessReceiveData(uint8_t *pData, uint16_t length)
{
    uint8_t *p = pData;
    uint16_t remainLen = length;
    uint16_t payloadLen = 0;
    
    if(!pData || length < sizeof(PacketHeader_ts))
    {
        return -1;
    }
    while(remainLen >= sizeof(PacketHeader_ts))
    {
        if(*p == PACKET_PREAMBLER) // Please modify it to your Preamble.
        {
            if(*(p + 1) == ID_YOUR) // Please modify it to your ID.
            {
                payloadLen = *(p + 5);
                payloadLen <<= 8;
                payloadLen |= *(p + 4);
                
                if((remainLen - sizeof(PacketHeader_ts)) >= payloadLen)
                {
                    if(*(p + 3) == Protocol_CalculateChecksum((Packet_ts *)p))
                    {
                        Protocol_ProcessReceivePacket((Packet_ts *)p);
                        
                        remainLen -= (payloadLen + sizeof(PacketHeader_ts));
                        p += (payloadLen + sizeof(PacketHeader_ts));
                        continue;
                    }
                }
            }
        }
        remainLen -= 1;
        p += 1;
    }
    
    return 0;
}
*********************Example Code End******************************************/


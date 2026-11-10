/*******************************************************************************
 * @file    my_protocol.h
 * @brief
 * @author  chengjie
 * @date    created on 2026-04-07
 * @version V1.0
 * @note
*******************************************************************************/
#ifndef __my_protocol_h__
#define __my_protocol_h__

#include "stdint.h"

/* 8bit       byte
 * 16bit      word
 * 32bit      double word
 * 64bit      quad word
 *            bit map
*/
/*********************Payload Define Code Begin********************************/
typedef struct __attribute__((packed)) { /* 4bytes */
  uint8_t bPayloadType;
  uint8_t bCommand;
  uint8_t bSubCommand;
  uint8_t bValue;
}PayloadHeader_ts;
typedef struct __attribute__((packed)) {
  PayloadHeader_ts xPayloadHeader;
//  uint8_t aData[0];
}Payload_ts;
/*********************Payload Define Code End********************************/

/*********************Packet Define Code Begin*********************************/
typedef struct __attribute__((packed)) { /* 6bytes */
  uint8_t bPreamble;        // Fixed value, such as 0x55.
  uint8_t bDestination;     // Destination(receiver) Identifier.
  uint8_t bSource;          // Source(Sender) Identifier.
  uint8_t bChecksum;        // Calculate the sum from bDestination to the last byte, exclude itself(byte3).
  uint16_t wPayLoadLength;  // Little-endian transmission.
}PacketHeader_ts;
typedef struct __attribute__((packed)) {
  PacketHeader_ts xPacketHeader;
  Payload_ts xPayload;
}Packet_ts;

uint16_t Protocol_ConvertWord(uint16_t value);
uint8_t Protocol_CalculateChecksum(Packet_ts *p);
/*********************Packet Define Code End***********************************/



#endif /* __my_protocol_h__ */




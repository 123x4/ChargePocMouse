/** 
*****************************************************************************************
 *
 * @file gtry_type.h
 *
 * @brief Header file defining software data types.
 *
 * @author      floudy
 * @date        2019-12-17
 * @version     V1.0.0
 * @copyright   Copyright (c) 2019 All rights reserved.
 * 
 * @par Modified log£º
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2019/12/17  <td>1.0.0    <td>liulong   <td>Create
 * </table>
 * 
*****************************************************************************************
 */

#ifndef __GTRY_TYPE_H__
#define __GTRY_TYPE_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef unsigned char   					uint8;
typedef unsigned short  					uint16;
typedef unsigned int    					uint32;
typedef unsigned long long    				uint64;
typedef signed   char   					int8;
typedef signed   short  					int16;
typedef signed   int    					int32;
typedef signed long long    				int64;


typedef volatile unsigned char   			vuint8;
typedef volatile unsigned short 			vuint16;
typedef volatile unsigned int    			vuint32;
typedef volatile unsigned long long    		vuint64;
typedef volatile signed   char   			vint8;
typedef volatile signed   short  			vint16;
typedef volatile signed   int    			vint32;
typedef volatile signed long long    		vint64;

typedef signed int   						BOOLEAN;

#ifndef true
#define		true			1
#endif

#ifndef false
#define		false			0
#endif

#ifndef NULL
#define		NULL			((void *)0)
#endif

#ifndef TRUE
typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}RY_BOOL;
#endif

typedef enum { RESET = 0, SET = ~RESET }FlagStatus, ITStatus;

typedef enum { DISABLE = 0, ENABLE = ~DISABLE}FunctionalState;

typedef enum { ERROR = 0, SUCCESS = ~ERROR}ErrorStatus;

#ifndef BIT
#define BIT(n)                      ((unsigned int) 0x1ul << (n))
#define BITS2(m,n)                  (BIT(m) | BIT(n) )

/* bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */
#define BITS(m,n)                   (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /* BIT */


#define BIT0             ((uint32)0x01)
#define BIT1             ((uint32)0x02)
#define BIT2             ((uint32)0x04)
#define BIT3             ((uint32)0x08)
#define BIT4             ((uint32)0x10)
#define BIT5             ((uint32)0x20)
#define BIT6             ((uint32)0x40)
#define BIT7             ((uint32)0x80)
#define BIT8           ((uint32)0x0100)
#define BIT9           ((uint32)0x0200)
#define BIT10          ((uint32)0x0400)
#define BIT11          ((uint32)0x0800)
#define BIT12          ((uint32)0x1000)
#define BIT13          ((uint32)0x2000)
#define BIT14          ((uint32)0x4000)
#define BIT15          ((uint32)0x8000)
#define BIT16         ((uint32)0x10000)
#define BIT17         ((uint32)0x20000)
#define BIT18         ((uint32)0x40000)
#define BIT19         ((uint32)0x80000)
#define BIT20        ((uint32)0x100000)
#define BIT21        ((uint32)0x200000)
#define BIT22        ((uint32)0x400000)
#define BIT23        ((uint32)0x800000)
#define BIT24       ((uint32)0x1000000)
#define BIT25       ((uint32)0x2000000)
#define BIT26       ((uint32)0x4000000)
#define BIT27       ((uint32)0x8000000)
#define BIT28      ((uint32)0x10000000)
#define BIT29      ((uint32)0x20000000)
#define BIT30      ((uint32)0x40000000)
#define BIT31      ((uint32)0x80000000)


/** Swaps the upper byte with the lower byte in a 16 bit variable */
#define SWAP(x) 					((((x)&0xFF)<<8)|(((x)>>8)&0x00FF))

/** The upper 8 bits of a 16 bit value */
#define MSB(a) 						(((a) & 0xFF00) >> 8)
/** The lower 8 bits (of a 16 bit value) */
#define LSB(a) 						((a) & 0xFF)

#ifndef MIN
#define MIN(a, b) 					((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) 					((a) > (b) ? (a) : (b))
#endif

#define _NOP_()			__asm("nop")
#define __NOP()			__asm("nop")

#define MEMCPY(des, src, n)     __builtin_memcpy ((des), (src), (n))
#define MEMSET(s, c, n)         __builtin_memset ((s), (c), (n))
#define MEMCMP(str1, str2, n)   __builtin_memcmp ((str1), (str2), (n))

#define RY_ARRAY_SIZE(arr)	 		(sizeof(arr) / sizeof(arr[0]))

#define GET_REG32(addr)				(*(volatile uint32 *)(addr))
#define SET_REG32(addr, value)		(*(volatile uint32 *)(addr) = value)

#define GET_REGBIT(addr, bit)		((GET_REG32(addr) >> bit) & 0x1)

#define SET_REGBIT(addr, bit, value)		\
{\
	uint32 regVal;\
	regVal = GET_REG32(addr);\
	regVal &= (~((uint32)0x1<<bit));\
	regVal |= ((uint32)value<<bit);\
	SET_REG32(addr, regVal);\
}

//*****************************************************************************
//
// Macros for hardware access, both direct and via the bit-band region.
//
//*****************************************************************************
#define HWREG(x)                                                              \
        (*((volatile uint32 *)(x)))
#define HWREGH(x)                                                             \
        (*((volatile uint16 *)(x)))
#define HWREGB(x)                                                             \
        (*((volatile uint8 *)(x)))
#define HWREGBITW(x, b)                                                       \
        HWREG(((uint32)(x) & 0xF0000000) | 0x02000000 |                     \
              (((uint32)(x) & 0x000FFFFF) << 5) | ((b) << 2))
#define HWREGBITH(x, b)                                                       \
        HWREGH(((uint32)(x) & 0xF0000000) | 0x02000000 |                    \
               (((uint32)(x) & 0x000FFFFF) << 5) | ((b) << 2))
#define HWREGBITB(x, b)                                                       \
        HWREGB(((uint32)(x) & 0xF0000000) | 0x02000000 |                    \
               (((uint32)(x) & 0x000FFFFF) << 5) | ((b) << 2))

#endif /* end of __GTRY_TYPE_H__ */

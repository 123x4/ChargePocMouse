/*******************************************************************************
 * @file    my_debug.h
 * @brief
 * @author  chengjie
 * @date    created on 2026-01-07
 * @version V1.0
 * @note
*******************************************************************************/
#ifndef __my_debug_h__
#define __my_debug_h__

#include "LPC11Uxx.h"
#include "my_board.h"

#define MY_MAIN_VERSION 0x0011//product version
#define DEBUG_VER  1139

/********************************/
#define MYDEBUG_ENABLE          0
#if MYDEBUG_ENABLE

#define MYDEBUG_UART_PRINTF_MASK      (0x01 << 0)
#define MYDEBUG_USB_PRINTF_MASK       (0x01 << 1)
#define MYDEBUG_PRINTF_ENABLE         (MYDEBUG_USB_PRINTF_MASK)
#include "stdio.h"
/*******1. uart print code begin*********************************/
#if (MYDEBUG_PRINTF_ENABLE & MYDEBUG_UART_PRINTF_MASK)
/*********Hardware parameter code begin*******************************/
#define MYDEBUG_UART_BAUDRATE     115200//bps. max value is 500kbps.
#define MYDEBUG_UART_TX_PORT      0
#define MYDEBUG_UART_TX_PIN       15
#define MYDEBUG_UART_TX_GPIO_CONFIG()   do{ LPC_IOCON->SWDIO_PIO0_15 &= ~0x07; \
                                            LPC_IOCON->SWDIO_PIO0_15 |= 0x01 ; \
                                          }while(0)
#define MYDEBUG_UART_SET_TX_PIN_AS_OUTPUTMODE() \
    do{LPC_GPIO->DIR[MYDEBUG_UART_TX_PORT] |= (1<<MYDEBUG_UART_TX_PIN);}while(0)
#define MYDEBUG_UART_SET_TX_PIN_HIGH()  \
    do{LPC_GPIO->SET[MYDEBUG_UART_TX_PORT] = 1<<MYDEBUG_UART_TX_PIN;}while(0)
#define MYDEBUG_UART_CLR_TX_PIN_LOW()   \
    do{LPC_GPIO->CLR[MYDEBUG_UART_TX_PORT] = 1<<MYDEBUG_UART_TX_PIN;}while(0)
/*********Hardware parameter code end******************************/
#define MYDEBUG_UART_NON_BLOCKING_ENABLE    1
#if MYDEBUG_UART_NON_BLOCKING_ENABLE
#include "my_queue.h"
#define MYDEBUG_UART_QUEUE_LENGTH           256
#endif
#endif
/*******uart print code end************************************/


/*******2. usb print code begin*********************************/
#if (MYDEBUG_PRINTF_ENABLE & MYDEBUG_USB_PRINTF_MASK)
#include "usbhw.h"
#include "my_queue.h"
#define MYDEBUG_USB_QUEUE_LENGTH            400
#define MYDEBUG_USB_EDP_NUM                 EP_3_CUSTION_OUT
#define MYDEBUG_USB_EDP_TRANSFER_LEN        TRANSFER_LENGTH
#define MYDEBUG_USB_EDP_IS_IDLE()   nsAPI_USB_EP_Status(MYDEBUG_USB_EDP_NUM)
#define MYDEBUG_USB_SEND_DATA(pdata, len) \
                              nsAPI_USB_WriteEP(MYDEBUG_USB_EDP_NUM, pdata, len)
#endif
/*******usb print code end************************************/

#endif /*MYDEBUG_ENABLE*/


#if MYDEBUG_PRINTF_ENABLE
#define InfoLog(format, ...)  \
    printf("[I]%s: " format "\r\n", __func__, ##__VA_ARGS__)
void MyDebug_Init(void);
void MyDebug_Task(void);
#else
#define InfoLog(format, ...)      do{}while(0)
#define MyDebug_Init()            do{}while(0)
#define MyDebug_Task()            do{}while(0)
#endif



#endif /* __my_debug_h__ */


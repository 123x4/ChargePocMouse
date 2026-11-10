/*******************************************************************************
 * @file    my_debug.c
 * @brief
 * @author  chengjie
 * @date    created on 2026-01-07
 * @version V1.0
 * @note
*******************************************************************************/
#include "my_debug.h"
#if MYDEBUG_ENABLE

#if (MYDEBUG_PRINTF_ENABLE & MYDEBUG_UART_PRINTF_MASK)
#if MYDEBUG_UART_NON_BLOCKING_ENABLE
MYQUEUE_Type_t uartQueueHandle = {0};
#endif
static void MyDebug_TimerInitCT16B1(unsigned int PR ,unsigned int Time)
{
  LPC_CT16B1->TCR = 0;//stop timer1
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8) ;//Set AHB Clock To The CT16B1 Domain
  LPC_CT16B1->TC = 0;//clear
  LPC_CT16B1->PC = 0;//clear
  
  LPC_CT16B1->PR = PR ;
  LPC_CT16B1->MR0 = Time ;
  /* bit 1 of MCR (sybol is MR0R): Reset on MR0: the TC will be reset if MR0 matches it. */
  LPC_CT16B1->MCR = ( 0x01 << 1 ) ; //enable MR0 match.
  
  //LPC_CT16B1->TCR = 1 ;//start timer
}
//static void MyDebug_TimerDeinitCT16B1(void)
//{
//  LPC_CT16B1->TCR = 0 ;//stop timer1
//}
static void MyDebug_UartHardInit(void)
{
  MYDEBUG_UART_TX_GPIO_CONFIG();
  MYDEBUG_UART_SET_TX_PIN_AS_OUTPUTMODE();
  MYDEBUG_UART_SET_TX_PIN_HIGH();
}
static void MyDebug_UartSendOneByte(unsigned char dat)
{
  #define timerPlusOneSpentTime   125 //nano seconds.
  #define holdTimeValue   ((1000000000)/timerPlusOneSpentTime/MYDEBUG_UART_BAUDRATE)
  unsigned short wait = 0;
  unsigned char i = 0;
  unsigned char val = dat;
  
  MyDebug_UartHardInit();
  MyDebug_TimerInitCT16B1(5, 0xFFFF);//TC will increase by 1 every 0.125us(8MHz = 48MHz/6).
  
  LPC_CT16B1->TCR = 1 ;//start timer
  MYDEBUG_UART_CLR_TX_PIN_LOW();
  wait = holdTimeValue;
  
  for(i = 0; i < 8; i++)
  {
    if(val & 0x01)
    {
      while((LPC_CT16B1->TC) < wait);
      MYDEBUG_UART_SET_TX_PIN_HIGH();
    }
    else
    {
      while((LPC_CT16B1->TC) < wait);
      MYDEBUG_UART_CLR_TX_PIN_LOW();
    }
    wait += holdTimeValue;
    val >>= 1;
  }
  while((LPC_CT16B1->TC) < wait);
  MYDEBUG_UART_SET_TX_PIN_HIGH();
  wait += holdTimeValue;
  while((LPC_CT16B1->TC) < wait);
  LPC_CT16B1->TCR = 0 ;//stop timer1
}
//void MyDebug_UartSend(unsigned char *pdata, unsigned short len)
//{
//  unsigned short i = 0;
//  
//  for(i = 0; i < len; i++)
//  {
//    MyDebug_UartSendOneByte(*(pdata + i));
//  }
//}
static void MyDebug_UartInit(void)
{
#if MYDEBUG_UART_NON_BLOCKING_ENABLE
  static uint8_t uartQueueBuff[MYDEBUG_UART_QUEUE_LENGTH];
  MyQueue_CreateStatic(&uartQueueHandle, uartQueueBuff, MYDEBUG_UART_QUEUE_LENGTH);
#endif
  MyDebug_UartHardInit();
}
static void MyDebug_UartTask(void)
{
#if MYDEBUG_UART_NON_BLOCKING_ENABLE
  uint8_t val;
  if(!MyQueue_Read(&uartQueueHandle, &val, 1))
  {
    MyDebug_UartSendOneByte(val);
  }
#endif
}
#endif


#if (MYDEBUG_PRINTF_ENABLE & MYDEBUG_USB_PRINTF_MASK)
MYQUEUE_Type_t usbQueueHandle = {0};
static void MyDebug_USBInit(void)
{
  static uint8_t usbQueueBuff[MYDEBUG_USB_QUEUE_LENGTH];
  MyQueue_CreateStatic(&usbQueueHandle, usbQueueBuff, MYDEBUG_USB_QUEUE_LENGTH);
}
static void MyDebug_USBTask(void)
{
#if (MYDEBUG_USB_EDP_TRANSFER_LEN > 64)
  #define MYDEBUG_USB_EDP_MAX_TRANSFER_LEN   64
#else
  #define MYDEBUG_USB_EDP_MAX_TRANSFER_LEN   MYDEBUG_USB_EDP_TRANSFER_LEN
#endif
  uint8_t buf[MYDEBUG_USB_EDP_MAX_TRANSFER_LEN];
  Queue_BaseType_t count = 0;
  
  MyQueue_GetItemCount(&usbQueueHandle, &count);
  
  if(count)
  {
    if(MYDEBUG_USB_EDP_IS_IDLE())
    {
      if(count > MYDEBUG_USB_EDP_MAX_TRANSFER_LEN)
      {
        count = MYDEBUG_USB_EDP_MAX_TRANSFER_LEN;
      }
      if(!MyQueue_Read(&usbQueueHandle, buf, count))
      {
        /***fill****/
        for( ; count < MYDEBUG_USB_EDP_MAX_TRANSFER_LEN; count++)
        {
          buf[count] = 0;
        }
        MYDEBUG_USB_SEND_DATA(buf, count);
      }
    }
  }
}
#endif


#if MYDEBUG_PRINTF_ENABLE
/*****redefine fputc of printf function********/
int fputc(int ch, FILE *f)
{
  unsigned char val = (unsigned char)ch;
#if (MYDEBUG_PRINTF_ENABLE & MYDEBUG_UART_PRINTF_MASK)
  #if (MYDEBUG_UART_NON_BLOCKING_ENABLE)
  MyQueue_Write(&uartQueueHandle, &val, 1);
  #else
  MyDebug_UartSendOneByte(val);
  #endif
#endif
#if (MYDEBUG_PRINTF_ENABLE & MYDEBUG_USB_PRINTF_MASK)
  MyQueue_Write(&usbQueueHandle, &val, 1);
#endif
  return ch;
}

void MyDebug_Init(void)
{
#if (MYDEBUG_PRINTF_ENABLE & MYDEBUG_UART_PRINTF_MASK)
  MyDebug_UartInit();
#endif
  
#if (MYDEBUG_PRINTF_ENABLE & MYDEBUG_USB_PRINTF_MASK)
  MyDebug_USBInit();
#endif
}

void MyDebug_Task(void)
{
#if (MYDEBUG_PRINTF_ENABLE & MYDEBUG_UART_PRINTF_MASK)
  MyDebug_UartTask();
#endif
  
#if (MYDEBUG_PRINTF_ENABLE & MYDEBUG_USB_PRINTF_MASK)
  MyDebug_USBTask();
#endif
}
#endif


#endif /* MYDEBUG_ENABLE */


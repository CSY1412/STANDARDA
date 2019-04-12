#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "sys.h"


#define  BSP_USART3_PRINTF_BUF_LEN              256u

extern uint8_t printf_buff[BSP_USART3_PRINTF_BUF_LEN];
extern uint16_t printf_buff_len;


void send_str( USART_TypeDef* USARTx, u8 *pWord, u16 Len );


#endif



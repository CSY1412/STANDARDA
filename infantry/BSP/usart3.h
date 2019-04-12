#ifndef __USART3_H__
#define __USART3_H__
#include "main.h"

/*-----USART3_TX-----PB10----DMA1----Stream3---Channel_4----*/
/*-----USART3_RX-----PB11----DMA1----Stream1---Channel_4----*/


/*
*********************************************************************************************************
*                                               MACROS
*********************************************************************************************************
*/
#define printf_enable          1
#define usart3_remap_enable    0               //1使用PD8   PD9
//0使用PB10  PB11

#define  BSP_USART3_DMA_RX_BUF_LEN              128u
#define  BSP_USART3_DMA_TX_BUF_LEN              256u
/*
*********************************************************************************************************
*                                             FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void USART3_IRQHandler(void);

void USART3_Configuration(uint32_t baud_rate);
void USART3_MYDMA_Enable(u16 ndtr);
extern uint8_t _USART3_DMA_TX_BUF[BSP_USART3_DMA_TX_BUF_LEN];

#endif

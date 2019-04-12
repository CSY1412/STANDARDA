#ifndef __USART2_H
#define __USART2_H

#include "main.h"

/*
*********************************************************************************************************
*                                               MACROS
*********************************************************************************************************
*/

#define  BSP_USART2_DMA_RX_BUF_LEN               128u        
/*
*********************************************************************************************************
*                                             FUNCTION PROTOTYPES
*********************************************************************************************************
*/



void USART2_IRQHandler(void);
void USART2_Configuration(uint32_t baud_rate);
void GetGestureData(u8* DMA_BUFF);
void Hi219DataPrcess(uint8_t *pData);


#endif
#ifndef __USART6_H__
#define __USART6_H__
#include "main.h"



/*
*********************************************************************************************************
*                                               MACROS
*********************************************************************************************************
*/

#define  BSP_USART6_DMA_RX_BUF_LEN               128u


typedef struct
{
		int pitch;
		int yaw;
		int gx;
		int gz;
}packed_6050_t;
extern packed_6050_t packed_6050;



/*
*********************************************************************************************************
*                                             FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void USART6_IRQHandler(void);

void USART6_Configuration(uint32_t baud_rate);
void Mpu6000DataProcess(uint8_t *pData);

#endif

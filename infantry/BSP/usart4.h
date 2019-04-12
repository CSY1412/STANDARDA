#ifndef __USART4_H__
#define __USART4_H__
#include "main.h"

/*
*********************************************************************************************************
*                                               MACROS
*********************************************************************************************************
*/

#define  BSP_USART4_DMA_RX_BUF_LEN              50u                   


typedef struct
{
	_Bool manifold_lock;
	_Bool aim_form;
	float x;
	float y;
	float distance;
	float aim_long;
	float aim_width;
	float x_offset;
	float y_offset;
	float x_assist;
	float y_assist;
	float x_temp;
	float y_temp;
}manifold_t;

extern manifold_t manifold;





/*
*********************************************************************************************************
*                                             FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void USART4_IRQHandler(void);

void USART4_Configuration(uint32_t baud_rate);
void ManifoldProcess(u8 * buff);

#endif

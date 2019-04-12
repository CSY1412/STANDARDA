#ifndef __USART1_H__
#define __USART1_H__
#include "main.h"

/*
*********************************************************************************************************
*                                               MACROS
*********************************************************************************************************
*/

#define  BSP_USART1_DMA_RX_BUF_LEN               30u                   
 
/*
*********************************************************************************************************
*                                             FUNCTION PROTOTYPES
*********************************************************************************************************
*/
extern uint8_t _USART1_DMA_RX_BUF[3][BSP_USART1_DMA_RX_BUF_LEN];//定义双缓存buff


typedef struct{
	struct{
		uint16_t ch0;
		uint16_t ch1;
		uint16_t ch2;
		uint16_t ch3;
		uint8_t  s1;
		uint8_t  s2;
				}rc;	
	struct{
		int16_t x;
		int16_t y;
		int16_t z;
		uint8_t press_l;
		uint8_t press_r;
				}mouse;
	struct{
		uint16_t v;
				}key;
}RC_Ctl_t;

extern RC_Ctl_t RC_Ctl;




typedef	struct
{
	float ch0;
	float ch1;
	float ch2;
	float ch3;

}cal_ch_t;
extern cal_ch_t cal_ch;



#define KEY_Q 0x0040
#define KEY_W 0x0001
#define KEY_E 0x0080
#define KEY_R 0x0100
#define KEY_A 0x0004
#define KEY_S 0x0002
#define KEY_D 0x0008
#define KEY_F 0x0200
#define KEY_G 0x0400
#define KEY_Z 0x0800
#define KEY_X 0x1000
#define KEY_C 0x2000
#define KEY_V 0x4000
#define KEY_B 0x8000
#define KEY_SHIFT 0x0010
#define KEY_CTRL 0x0020


typedef struct{

u8 s1;
u8 s2;
_Bool key_Q;
_Bool key_W;
_Bool key_E;
_Bool key_R;
_Bool key_A;
_Bool key_S;
_Bool key_D;
_Bool key_F;
_Bool key_G;
_Bool key_Z;
_Bool key_X;
_Bool key_C;
_Bool key_V;
_Bool key_B;
_Bool key_SHIFT;
_Bool key_CTRL;
_Bool key_l;
_Bool key_r;
_Bool board_s1;
_Bool board_s2;
_Bool board_s3;
_Bool board_s4;
}key_valu_t;
extern key_valu_t last_key_valu;
extern key_valu_t this_key_valu;

typedef struct{
		_Bool key_Q;
		_Bool key_W;
		_Bool key_E;
		_Bool key_R;
		_Bool key_A;
		_Bool key_S;
		_Bool key_D;
		_Bool key_F;
		_Bool key_G;
		_Bool key_Z;
		_Bool key_X;
		_Bool key_C;
		_Bool key_V;
		_Bool key_B;
		_Bool key_SHIFT;
		_Bool key_CTRL;
}key_boad_t;
extern key_boad_t  key_boad;

extern xQueueHandle RC_DataQueue;  //队列


void USART1_IRQHandler(void);

void USART1_Configuration(uint32_t baud_rate);
void ProcessRC(u8 *Data);
void RefreshKeyValu(void);
float DeathAreaProcess(uint16_t *ch_raw_val,int death_val);
void Rc_Init(void);
void KeyBoardProcess(key_boad_t* key,uint16_t key_v_in);
#endif

#include "usart.h"	
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOSʹ��	  
#endif


#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 


uint8_t printf_buff[BSP_USART3_PRINTF_BUF_LEN];
uint16_t printf_buff_len = 0;
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
//	while((USART6->SR&0X40)==0);//ѭ������,ֱ���������   
//	USART6->DR = (u8) ch;      
//	return ch;
	
	 if(printf_buff_len <= (BSP_USART3_PRINTF_BUF_LEN-50))
        printf_buff[printf_buff_len++] = (u8) ch;
    return ch;
}

void send_str( USART_TypeDef* USARTx, u8 *pWord, u16 Len )
{
	u8 i = 0;
	for(i=0; i<Len; i++) {
		USART_SendData(USARTx, (uint8_t)*pWord);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
		pWord++;
	}
}
#endif
 
 




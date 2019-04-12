#include "usart.h"	
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOS使用	  
#endif


#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 


uint8_t printf_buff[BSP_USART3_PRINTF_BUF_LEN];
uint16_t printf_buff_len = 0;
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
//	while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
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
 
 




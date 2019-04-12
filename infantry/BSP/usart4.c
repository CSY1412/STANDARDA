#include "FIFO.h"
#include "usart4.h"


    
/*-----USART4_RX-----PC11----*/
/*-DMA1_Stream2,DMA_Channel_4-*/

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/
static uint8_t _USART4_DMA_RX_BUF[2][BSP_USART4_DMA_RX_BUF_LEN];//定义双缓存buff
manifold_t manifold;

void USART4_Configuration(uint32_t baud_rate)
{
    GPIO_InitTypeDef gpio;
	  USART_InitTypeDef usart;
	  NVIC_InitTypeDef nvic;
    DMA_InitTypeDef dma;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); 
    
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
    
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_11;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &gpio);
    
    USART_DeInit(UART4);
    USART_StructInit(&usart);
    usart.USART_BaudRate = baud_rate;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_Mode = USART_Mode_Rx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(UART4, &usart);
        
    USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
    
    DMA_DeInit(DMA1_Stream2);
    DMA_StructInit(&dma);
    dma.DMA_Channel = DMA_Channel_4;
    dma.DMA_PeripheralBaseAddr = (uint32_t)(&UART4->DR);
    dma.DMA_Memory0BaseAddr = (uint32_t)&_USART4_DMA_RX_BUF[0][0];
    dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dma.DMA_BufferSize = sizeof(_USART4_DMA_RX_BUF)/2;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		
    dma.DMA_Mode = DMA_Mode_Circular;
    dma.DMA_Priority = DMA_Priority_Medium;
    dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
    dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream2, &dma);
    
    //配置Memory1,Memory0是第一个使用的Memory   （配置双缓存）
    DMA_DoubleBufferModeConfig(DMA1_Stream2, (uint32_t)&_USART4_DMA_RX_BUF[1][0], DMA_Memory_0);   //first used memory configuration
    DMA_DoubleBufferModeCmd(DMA1_Stream2, ENABLE);
    
    DMA_Cmd(DMA1_Stream2, ENABLE);
    
    nvic.NVIC_IRQChannel = UART4_IRQn;                          
		nvic.NVIC_IRQChannelPreemptionPriority = 2;   //pre-emption priority 
		nvic.NVIC_IRQChannelSubPriority = 0;		    //subpriority 
		nvic.NVIC_IRQChannelCmd = ENABLE;			
		NVIC_Init(&nvic);	
    
		USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);        //usart rx idle interrupt  enabled
    USART_Cmd(UART4, ENABLE);

}

//串口接收中断服务函数
void UART4_IRQHandler(void)
{
	static uint8_t this_time_rx_len = 0;
	if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)
	{
		//clear the idle pending flag 
		(void)UART4->SR;
		(void)UART4->DR;

		//Target is Memory0
		if(DMA_GetCurrentMemoryTarget(DMA1_Stream2) == 0)
		{
			DMA_Cmd(DMA1_Stream2, DISABLE);
			this_time_rx_len = BSP_USART4_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream2);
			DMA1_Stream2->NDTR = (uint16_t)BSP_USART4_DMA_RX_BUF_LEN;     //relocate the dma memory pointer to the beginning position
			DMA1_Stream2->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1
			DMA_Cmd(DMA1_Stream2, ENABLE);
			ManifoldProcess(_USART4_DMA_RX_BUF[0]);
            
			
			
		}
		else 
		{
			DMA_Cmd(DMA1_Stream2, DISABLE);
			this_time_rx_len = BSP_USART4_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream2);
			DMA1_Stream2->NDTR = (uint16_t)BSP_USART4_DMA_RX_BUF_LEN;      //relocate the dma memory pointer to the beginning position
			DMA1_Stream2->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0
			DMA_Cmd(DMA1_Stream2, ENABLE);
			ManifoldProcess(_USART4_DMA_RX_BUF[1]);
		}
	}       
}


void ManifoldProcess(u8 * buff)
{
				if ( buff[0] != 0xFF) 
				{	
					manifold.manifold_lock=0;
					return;//接收错误，直接返回
				}
				else if(buff[5]==0) 
				{
							
					manifold.manifold_lock=0;				
					manifold.x =0;							
				  manifold.y =0;
					manifold.x_assist=0;
					manifold.y_assist=0;
					return; 
				}		
				manifold.manifold_lock=1;  //锁定目标
				manifold.x =-(float)((short)(buff[1]|buff[2]<<8 ))/10;							
				manifold.y =(float)((short)(buff[3]|buff[4]<<8))/10;
    		manifold.aim_width=(short)(buff[5]|buff[6]<<8);
				manifold.aim_long=(short)(buff[7]|buff[8]<<8);
				BeepON();	
}

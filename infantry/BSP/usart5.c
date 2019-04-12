#include "fifo.h"
#include "usart5.h"

/*-----USART5_TX-----PC12-----*/
/*-----USART5_RX-----PD2----*/
/*-DMA1_Stream0,DMA_Channel_4-*/

static uint8_t _USART5_DMA_RX_BUF[2][BSP_USART5_DMA_RX_BUF_LEN];//定义双缓存buff



void USART5_Configuration(uint32_t baud_rate)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;
    NVIC_InitTypeDef nvic;
    DMA_InitTypeDef dma;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);

    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_12;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &gpio);
	
	  gpio.GPIO_Pin = GPIO_Pin_2;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &gpio);

    USART_DeInit(UART5);
    USART_StructInit(&usart);
    usart.USART_BaudRate = baud_rate;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(UART5, &usart);

    USART_DMACmd(UART5, USART_DMAReq_Rx, ENABLE);

    DMA_DeInit(DMA1_Stream0);
    DMA_StructInit(&dma);
    dma.DMA_Channel = DMA_Channel_4;
    dma.DMA_PeripheralBaseAddr = (uint32_t)(&UART5->DR);
    dma.DMA_Memory0BaseAddr = (uint32_t)&_USART5_DMA_RX_BUF[0][0];
    dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dma.DMA_BufferSize = sizeof(_USART5_DMA_RX_BUF)/2;
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
    DMA_Init(DMA1_Stream0, &dma);

    //配置Memory1,Memory0是第一个使用的Memory   （配置双缓存）
    DMA_DoubleBufferModeConfig(DMA1_Stream0, (uint32_t)&_USART5_DMA_RX_BUF[1][0], DMA_Memory_0);   //first used memory configuration
    DMA_DoubleBufferModeCmd(DMA1_Stream0, ENABLE);
    DMA_Cmd(DMA1_Stream0, ENABLE);
		DMA_ITConfig(DMA1_Stream0,DMA_IT_TC,ENABLE);
		DMA_ClearFlag(DMA1_Stream0,DMA_FLAG_TCIF0); //清除DMA接收完成标志
	

    nvic.NVIC_IRQChannel = UART5_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;   //pre-emption priority
    nvic.NVIC_IRQChannelSubPriority = 0;		    //subpriority
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);        //usart rx idle interrupt  enabled
    USART_Cmd(UART5, ENABLE);

}

//串口接收中断服务函数



void UART5_IRQHandler(void)
{
    static uint8_t this_time_rx_len = 0;
		u8 	Res; 
		Res =USART_ReceiveData(UART5);//	//读取接收到的数据
    if(USART_GetITStatus(UART5, USART_IT_IDLE) != RESET)
    {
        //clear the idle pending flag
        (void)UART5->SR;
        (void)UART5->DR;

        //Target is Memory0
        if(DMA_GetCurrentMemoryTarget(DMA1_Stream0) == 0)
        {
            DMA_Cmd(DMA1_Stream0, DISABLE);
            this_time_rx_len = BSP_USART5_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream0);
            DMA1_Stream0->NDTR = (uint16_t)BSP_USART5_DMA_RX_BUF_LEN;     //relocate the dma memory pointer to the beginning position
            DMA1_Stream0->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1
            DMA_Cmd(DMA1_Stream0, ENABLE);
						FIFO_S_Puts(&REFERENCE_FIFO,_USART5_DMA_RX_BUF[0],this_time_rx_len);

        }
        //Target is Memory1
        else
        {
            DMA_Cmd(DMA1_Stream0, DISABLE);
            this_time_rx_len = BSP_USART5_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream0);
            DMA1_Stream0->NDTR = (uint16_t)BSP_USART5_DMA_RX_BUF_LEN;      //relocate the dma memory pointer to the beginning position
            DMA1_Stream0->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0
            DMA_Cmd(DMA1_Stream0, ENABLE);
						FIFO_S_Puts(&REFERENCE_FIFO,_USART5_DMA_RX_BUF[1],this_time_rx_len);
				
        }
    }
}


void DMA1_Stream0_IRQHandler(void)
{
		 if(DMA_GetITStatus(DMA1_Stream0, DMA_IT_TCIF0) != RESET)  //如果是满终端
    {
				DMA_ClearFlag(DMA1_Stream0,DMA_FLAG_TCIF0); //清除DMA接收完成标志
	
		}	


}





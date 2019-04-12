#include "FIFO.h"
#include "usart2.h"


/*-----USART2_RX-----PA3----*/
/*-DMA1_Stream5,DMA_Channel_4-*/

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/


static uint8_t _USART2_DMA_RX_BUF[2][BSP_USART2_DMA_RX_BUF_LEN];//定义双缓存buff

void USART2_Configuration(uint32_t baud_rate)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;
    NVIC_InitTypeDef nvic;
    DMA_InitTypeDef dma;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_3;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpio);

    USART_DeInit(USART2);
    USART_StructInit(&usart);
    usart.USART_BaudRate = baud_rate;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_Mode = USART_Mode_Rx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &usart);

    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

    DMA_DeInit(DMA1_Stream5);
    DMA_StructInit(&dma);
    dma.DMA_Channel = DMA_Channel_4;
    dma.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);
    dma.DMA_Memory0BaseAddr = (uint32_t)&_USART2_DMA_RX_BUF[0][0];
    dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dma.DMA_BufferSize = sizeof(_USART2_DMA_RX_BUF)/2;
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
    DMA_Init(DMA1_Stream5, &dma);

    //配置Memory1,Memory0是第一个使用的Memory   （配置双缓存）
    DMA_DoubleBufferModeConfig(DMA1_Stream5, (uint32_t)&_USART2_DMA_RX_BUF[1][0], DMA_Memory_0);   //first used memory configuration
    DMA_DoubleBufferModeCmd(DMA1_Stream5, ENABLE);

    DMA_Cmd(DMA1_Stream5, ENABLE);

    nvic.NVIC_IRQChannel = USART2_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;   //pre-emption priority
    nvic.NVIC_IRQChannelSubPriority = 0;		    //subpriority
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);        //usart rx idle interrupt  enabled
    USART_Cmd(USART2, ENABLE);

}

//串口接收中断服务函数
void USART2_IRQHandler(void)
{
    static uint8_t this_time_rx_len = 0;
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        //clear the idle pending flag
        (void)USART2->SR;
        (void)USART2->DR;

        //Target is Memory0
        if(DMA_GetCurrentMemoryTarget(DMA1_Stream5) == 0)
        {
            DMA_Cmd(DMA1_Stream5, DISABLE);
            this_time_rx_len = BSP_USART2_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);
            DMA1_Stream5->NDTR = (uint16_t)BSP_USART2_DMA_RX_BUF_LEN;     //relocate the dma memory pointer to the beginning position
            DMA1_Stream5->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1
            DMA_Cmd(DMA1_Stream5, ENABLE);
					 Mpu6000DataProcess(_USART2_DMA_RX_BUF[0]);
//            if(this_time_rx_len == Hi219_FRAME_LENGTH)
//            {
//                //数据处理
//               
//            }
        }
        else
        {
            DMA_Cmd(DMA1_Stream5, DISABLE);
            this_time_rx_len = BSP_USART2_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);
            DMA1_Stream5->NDTR = (uint16_t)BSP_USART2_DMA_RX_BUF_LEN;      //relocate the dma memory pointer to the beginning position
            DMA1_Stream5->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0
            DMA_Cmd(DMA1_Stream5, ENABLE);
					  Mpu6000DataProcess(_USART2_DMA_RX_BUF[0]);
//            if(this_time_rx_len == Hi219_FRAME_LENGTH)
//            {
//                //数据处理
//                Mpu6000DataProcess(_USART2_DMA_RX_BUF[1]);
//            }
        }
    }
}

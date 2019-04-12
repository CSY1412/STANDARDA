#include "usart3.h"

/*-----USART3_TX-----PB10----DMA1----Stream3---Channel_4----*/
/*-----USART3_RX-----PB11----DMA1----Stream1---Channel_4----*/

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/
uint8_t _USART3_DMA_TX_BUF[BSP_USART3_DMA_TX_BUF_LEN];
static uint8_t _USART3_DMA_RX_BUF[2][BSP_USART3_DMA_RX_BUF_LEN];//定义双缓存buff


void USART3_Configuration(uint32_t baud_rate)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;
    NVIC_InitTypeDef nvic;
    DMA_InitTypeDef dma;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &gpio);

    USART_DeInit(USART3);
    USART_StructInit(&usart);
    usart.USART_BaudRate = baud_rate;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &usart);

    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
    USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);

    DMA_DeInit(DMA1_Stream1);
    DMA_StructInit(&dma);
    dma.DMA_Channel = DMA_Channel_4;
    dma.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR);
    dma.DMA_Memory0BaseAddr = (uint32_t)&_USART3_DMA_RX_BUF[0][0];
    dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dma.DMA_BufferSize = sizeof(_USART3_DMA_RX_BUF)/2;
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
    DMA_Init(DMA1_Stream1, &dma);

    //配置Memory1,Memory0是第一个使用的Memory   （配置双缓存）
    DMA_DoubleBufferModeConfig(DMA1_Stream1, (uint32_t)&_USART3_DMA_RX_BUF[1][0], DMA_Memory_0);   //first used memory configuration
    DMA_DoubleBufferModeCmd(DMA1_Stream1, ENABLE);

    DMA_Cmd(DMA1_Stream1, ENABLE);

    nvic.NVIC_IRQChannel = USART3_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;   //pre-emption priority
    nvic.NVIC_IRQChannelSubPriority = 0;		    //subpriority
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);        //usart rx idle interrupt  enabled
    USART_Cmd(USART3, ENABLE);

    DMA_InitTypeDef  DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1时钟使能

    DMA_DeInit(DMA1_Stream3);

    while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE) {} //等待DMA可配置

    /* 配置 DMA Stream */
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART3->DR;//DMA外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)_USART3_DMA_TX_BUF;//DMA 存储器0地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
    DMA_InitStructure.DMA_BufferSize = BSP_USART3_DMA_TX_BUF_LEN;//数据传输量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
    DMA_Init(DMA1_Stream3, &DMA_InitStructure);//初始化DMA Stream

}
//开启一次DMA传输
void USART3_MYDMA_Enable(u16 ndtr)
{
    static _Bool fist_fag=1;
    if(fist_fag == 0) {
        if(DMA_GetFlagStatus(DMA1_Stream3,DMA_FLAG_TCIF3)!=RESET) { //等待DMA1_Steam3传输完成
        } else return;
    }
    DMA_ClearFlag(DMA1_Stream3,DMA_FLAG_TCIF3);//清除DMA1_Steam3传传输完成标志
    USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);  //使能串口3的DMA发送

    DMA_Cmd(DMA1_Stream3, DISABLE);                      //关闭DMA传输

    while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE) {}	//确保DMA可以被设置  a

    DMA_SetCurrDataCounter(DMA1_Stream3,ndtr);          //数据传输量

    DMA_Cmd(DMA1_Stream3, ENABLE);                      //开启DMA传输
    fist_fag = 0;
}

//串口接收中断服务函数
void USART3_IRQHandler(void)
{ 	
    static uint8_t this_time_rx_len = 0;
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
    {
        //clear the idle pending flag
        (void)USART3->SR;
        (void)USART3->DR;

        //Target is Memory0
        if(DMA_GetCurrentMemoryTarget(DMA1_Stream1) == 0)
        {
            DMA_Cmd(DMA1_Stream1, DISABLE);
            this_time_rx_len = BSP_USART3_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream1);
            DMA1_Stream1->NDTR = (uint16_t)BSP_USART3_DMA_RX_BUF_LEN;     //relocate the dma memory pointer to the beginning position
            DMA1_Stream1->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1
            DMA_Cmd(DMA1_Stream1, ENABLE);
            if(this_time_rx_len != sizeof(TopToBottomData_t)) return;
            TopToBottomDataPPrcess(_USART3_DMA_RX_BUF[0]);

        }
        else
        {
            DMA_Cmd(DMA1_Stream1, DISABLE);
            this_time_rx_len = BSP_USART3_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream1);
            DMA1_Stream1->NDTR = (uint16_t)BSP_USART3_DMA_RX_BUF_LEN;      //relocate the dma memory pointer to the beginning position
            DMA1_Stream1->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0
            DMA_Cmd(DMA1_Stream1, ENABLE);
            if(this_time_rx_len != sizeof(TopToBottomData_t)) return;
            TopToBottomDataPPrcess(_USART3_DMA_RX_BUF[1]);

        }
    }
}




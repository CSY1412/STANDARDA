#include "FIFO.h"
#include "usart6.h"

/*-----USART6_TX-----PC6-----*/
/*-----USART6_RX-----PC7----*/
/*-DMA2_Stream1,DMA_Channel_5-*/

static uint8_t _USART6_DMA_RX_BUF[2][BSP_USART6_DMA_RX_BUF_LEN];//定义双缓存buff

packed_6050_t packed_6050;

void USART6_Configuration(uint32_t baud_rate)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;
    NVIC_InitTypeDef nvic;
    DMA_InitTypeDef dma;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);

    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &gpio);
	
	  gpio.GPIO_Pin = GPIO_Pin_6;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &gpio);

    USART_DeInit(USART6);
    USART_StructInit(&usart);
    usart.USART_BaudRate = baud_rate;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART6, &usart);

    USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);

    DMA_DeInit(DMA2_Stream1);
    DMA_StructInit(&dma);
    dma.DMA_Channel = DMA_Channel_5;
    dma.DMA_PeripheralBaseAddr = (uint32_t)(&USART6->DR);
    dma.DMA_Memory0BaseAddr = (uint32_t)&_USART6_DMA_RX_BUF[0][0];
    dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dma.DMA_BufferSize = sizeof(_USART6_DMA_RX_BUF)/2;
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
    DMA_Init(DMA2_Stream1, &dma);

    //配置Memory1,Memory0是第一个使用的Memory   （配置双缓存）
    DMA_DoubleBufferModeConfig(DMA2_Stream1, (uint32_t)&_USART6_DMA_RX_BUF[1][0], DMA_Memory_0);   //first used memory configuration
    DMA_DoubleBufferModeCmd(DMA2_Stream1, ENABLE);

    DMA_Cmd(DMA2_Stream1, ENABLE);

    nvic.NVIC_IRQChannel = USART6_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;   //pre-emption priority
    nvic.NVIC_IRQChannelSubPriority = 0;		    //subpriority
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);        //usart rx idle interrupt  enabled
    USART_Cmd(USART6, ENABLE);

}

//串口接收中断服务函数
void USART6_IRQHandler(void)
{
    static uint8_t this_time_rx_len = 0;
		u8 	Res; 
		Res =USART_ReceiveData(USART6);//	//读取接收到的数据
    if(USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)
    {
        //clear the idle pending flag
        (void)USART6->SR;
        (void)USART6->DR;
        //Target is Memory0
        if(DMA_GetCurrentMemoryTarget(DMA2_Stream1) == 0)
        {
            DMA_Cmd(DMA2_Stream1, DISABLE);
            this_time_rx_len = BSP_USART6_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA2_Stream1);
            DMA2_Stream1->NDTR = (uint16_t)BSP_USART6_DMA_RX_BUF_LEN;     //relocate the dma memory pointer to the beginning position
            DMA2_Stream1->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1
            DMA_Cmd(DMA2_Stream1, ENABLE);
           	Mpu6000DataProcess(_USART6_DMA_RX_BUF[0]);

        }
        //Target is Memory1
        else
        {
            DMA_Cmd(DMA2_Stream1, DISABLE);
            this_time_rx_len = BSP_USART6_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA2_Stream1);
            DMA2_Stream1->NDTR = (uint16_t)BSP_USART6_DMA_RX_BUF_LEN;      //relocate the dma memory pointer to the beginning position
            DMA2_Stream1->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0
            DMA_Cmd(DMA2_Stream1, ENABLE);
					
						Mpu6000DataProcess(_USART6_DMA_RX_BUF[1]);
        }
    }
}




void Mpu6000DataProcess(uint8_t *pData)
{
		    memcpy(&packed_6050,pData,sizeof(packed_6050));
	
				imu_data_access.Angle.yaw=-(packed_6050.yaw*0.01f);
        imu_data_access.Angle.pitch=(packed_6050.pitch*0.01f);
				imu_data_access.Gyro.x=-((packed_6050.gz)*0.01f);
        imu_data_access.Gyro.z=((packed_6050.gx)*0.01f);		
				ExtendAngle(&extend_angle,imu_data_access.Angle.yaw);
				imu_data_access.Angle.expand_yaw=extend_angle.ecd_value;
}



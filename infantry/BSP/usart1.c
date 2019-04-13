#include "FIFO.h"
#include "usart1.h"


/*-----USART1_RX-----PB7----*/
/*-DMA2_Stream5,DMA_Channel_4-*/


uint8_t _USART1_DMA_RX_BUF[3][BSP_USART1_DMA_RX_BUF_LEN];//定义双缓存buff
cal_ch_t     cal_ch;
key_valu_t   last_key_valu;
key_valu_t   this_key_valu;
RC_Ctl_t     RC_Ctl;
key_boad_t   key_boad;
xQueueHandle RC_DataQueue;


void USART1_Configuration(uint32_t baud_rate)
{
    GPIO_InitTypeDef gpio;
	  USART_InitTypeDef usart;
	  NVIC_InitTypeDef nvic;
    DMA_InitTypeDef dma;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
    
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
    
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &gpio);
    
    USART_DeInit(USART1);
    USART_StructInit(&usart);
    usart.USART_BaudRate = baud_rate;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_Mode = USART_Mode_Rx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &usart);
        
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    
    DMA_DeInit(DMA2_Stream5);
    DMA_StructInit(&dma);
    dma.DMA_Channel = DMA_Channel_4;
    dma.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);
    dma.DMA_Memory0BaseAddr = (uint32_t)&_USART1_DMA_RX_BUF[0][0];
    dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dma.DMA_BufferSize = sizeof(_USART1_DMA_RX_BUF)/2;
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
    DMA_Init(DMA2_Stream5, &dma);
    
    //配置Memory1,Memory0是第一个使用的Memory   （配置双缓存）
    DMA_DoubleBufferModeConfig(DMA2_Stream5, (uint32_t)&_USART1_DMA_RX_BUF[1][0], DMA_Memory_0);   //first used memory configuration
    DMA_DoubleBufferModeCmd(DMA2_Stream5, ENABLE);
    
    DMA_Cmd(DMA2_Stream5, ENABLE);
    
    nvic.NVIC_IRQChannel = USART1_IRQn;                          
		nvic.NVIC_IRQChannelPreemptionPriority = 2;   //pre-emption priority 
		nvic.NVIC_IRQChannelSubPriority = 0;		    //subpriority 
		nvic.NVIC_IRQChannelCmd = ENABLE;			
		NVIC_Init(&nvic);	
    
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);        //usart rx idle interrupt  enabled
    USART_Cmd(USART1, ENABLE);
		
		
		RC_DataQueue = xQueueCreate(1, BSP_USART1_DMA_RX_BUF_LEN);

}

//串口接收中断服务函数
void USART1_IRQHandler(void)
{
	static uint8_t this_time_rx_len = 0;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		//clear the idle pending flag 
		(void)USART1->SR;
		(void)USART1->DR;

		//Target is Memory0
		if(DMA_GetCurrentMemoryTarget(DMA2_Stream5) == 0)
		{
			DMA_Cmd(DMA2_Stream5, DISABLE);
			this_time_rx_len = BSP_USART1_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA2_Stream5);
			DMA2_Stream5->NDTR = (uint16_t)BSP_USART1_DMA_RX_BUF_LEN;     //relocate the dma memory pointer to the beginning position
			DMA2_Stream5->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1
			DMA_Cmd(DMA2_Stream5, ENABLE);
//			xQueueOverwriteFromISR(RC_DataQueue,_USART1_DMA_RX_BUF[0],NULL);  //存进队列
////			
		  	ProcessRC(_USART1_DMA_RX_BUF[0]);     
		}
		else 
		{
			DMA_Cmd(DMA2_Stream5, DISABLE);
			this_time_rx_len = BSP_USART1_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA2_Stream5);
			DMA2_Stream5->NDTR = (uint16_t)BSP_USART1_DMA_RX_BUF_LEN;      //relocate the dma memory pointer to the beginning position
			DMA2_Stream5->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0
			DMA_Cmd(DMA2_Stream5, ENABLE);
//			
//			xQueueOverwriteFromISR(RC_DataQueue,_USART1_DMA_RX_BUF[1],NULL);
//			
			
			ProcessRC(_USART1_DMA_RX_BUF[1]);     
		}
	}       
}


/**
  * @brief        	Buff处理
  * @author         蜗牛蜗牛跑
  * @param[in]        
  * @retval       
  */
void ProcessRC(u8 *Data) 
{
		 RC_Ctl.rc.ch0 = (Data[0]| (Data[1] << 8)) & 0x07ff; //!< Channel 0
		 RC_Ctl.rc.ch1 = ((Data[1] >> 3) | (Data[2] << 5)) & 0x07ff; //!< Channel 1		
		 RC_Ctl.rc.ch2 = ((Data[2] >> 6) | (Data[3] << 2) | (Data[4] << 10)) & 0x07ff;//!< Channel 2
		 RC_Ctl.rc.ch3 = ((Data[4] >> 1) | (Data[5] << 7)) & 0x07ff; //!< Channel 3
		 RC_Ctl.rc.s1 = ((Data[5] >> 4)& 0x000C) >> 2; //!< Switch left
		 RC_Ctl.rc.s2 = ((Data[5] >> 4)& 0x0003);
		 RC_Ctl.mouse.x = Data[6] | (Data[7] << 8); //!< Mouse X axis
		 RC_Ctl.mouse.y = Data[8] | (Data[9] << 8); //!< Mouse Y axis
		 RC_Ctl.mouse.z = Data[10] | (Data[11] << 8); //!< Mouse Z axis
		 RC_Ctl.mouse.press_l = Data[12]; //!< Mouse Left Is Press ?
		 RC_Ctl.mouse.press_r = Data[13]; //!< Mouse Right Is Press ?
		 RC_Ctl.key.v = Data[14] | (Data[15] << 8); //!< KeyBoard value
		 cal_ch.ch0=DeathAreaProcess(&RC_Ctl.rc.ch0,RC_DEAD_BAND);  //死区处理
		 cal_ch.ch1=DeathAreaProcess(&RC_Ctl.rc.ch1,RC_DEAD_BAND);
		 cal_ch.ch2=DeathAreaProcess(&RC_Ctl.rc.ch2,RC_DEAD_BAND);
		 cal_ch.ch3=DeathAreaProcess(&RC_Ctl.rc.ch3,RC_DEAD_BAND);
		 KeyBoardProcess(&key_boad,RC_Ctl.key.v);	
}

/**
  * @brief        	按键值更新
  * @author         蜗牛蜗牛跑
  * @param[in]        
  * @retval       
  */
void RefreshKeyValu(void)
{
		memcpy(&last_key_valu,&this_key_valu,sizeof(key_valu_t));//保存上次的按键值
	
		this_key_valu.s1=RC_Ctl.rc.s1;
		this_key_valu.s2=RC_Ctl.rc.s2;
		this_key_valu.key_CTRL=key_boad.key_CTRL;
		this_key_valu.key_r=RC_Ctl.mouse.press_r;
		this_key_valu.key_l=RC_Ctl.mouse.press_l;
	/*------------板载按键----------------------------*/
		this_key_valu.board_s1=!KEY1;
		this_key_valu.board_s2=!KEY2;
		this_key_valu.board_s3=!KEY3;
		this_key_valu.board_s4=!KEY4;
}
/**
  * @brief        	死区处理
  * @author         蜗牛蜗牛跑
  * @param[in]        
  * @retval       
  */
float DeathAreaProcess(uint16_t *ch_raw_val,int death_val)
{	
	if((*ch_raw_val-1024)>0&&(*ch_raw_val-1024)<death_val)
	{
		 return 1024;
	}
	if((*ch_raw_val-1024)<0&&(*ch_raw_val-1024)>-death_val)
	{
		 return 1024;
	}
	return *ch_raw_val;
}

/**
  * @brief        	遥控器键值初始化
  * @author         蜗牛蜗牛跑
  * @param[in]        
  * @retval       
  */
void Rc_Init(void)
{
	 RC_Ctl.rc.ch0 =1024;
	 RC_Ctl.rc.ch1 =1024;
	 RC_Ctl.rc.ch2 =1024;
	 RC_Ctl.rc.ch3 =1024;
}


/**
  * @brief        	按键处理
  * @author         蜗牛蜗牛跑
  * @param[in]        
  * @retval       
  */
void KeyBoardProcess(key_boad_t* key,uint16_t key_v_in)
{
	if((key_v_in&KEY_Q)!=0)key->key_Q=1;else key->key_Q=0;
	if((key_v_in&KEY_W)!=0)key->key_W=1;else key->key_W=0;
	if((key_v_in&KEY_E)!=0)key->key_E=1;else key->key_E=0;
	if((key_v_in&KEY_R)!=0)key->key_R=1;else key->key_R=0;
	if((key_v_in&KEY_A)!=0)key->key_A=1;else key->key_A=0;
	if((key_v_in&KEY_S)!=0)key->key_S=1;else key->key_S=0;
	if((key_v_in&KEY_D)!=0)key->key_D=1;else key->key_D=0;
	if((key_v_in&KEY_F)!=0)key->key_F=1;else key->key_F=0;
	if((key_v_in&KEY_G)!=0)key->key_G=1;else key->key_G=0;
	if((key_v_in&KEY_Z)!=0)key->key_Z=1;else key->key_Z=0;
	if((key_v_in&KEY_X)!=0)key->key_X=1;else key->key_X=0;
	if((key_v_in&KEY_C)!=0)key->key_C=1;else key->key_C=0;
	if((key_v_in&KEY_V)!=0)key->key_V=1;else key->key_V=0;
	if((key_v_in&KEY_B)!=0)key->key_B=1;else key->key_B=0;
	if((key_v_in&KEY_SHIFT)!=0)key->key_SHIFT=1;else key->key_SHIFT=0;
	if((key_v_in&KEY_CTRL)!=0)key->key_CTRL=1;else key->key_CTRL=0;
}





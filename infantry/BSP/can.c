#include "can.h"
/*----CAN1_TX-----PA12----*/
/*----CAN1_RX-----PA11----*/

chasis_motor_t chasis_motor[4];  //���̵�����ز����ṹ��
gimbal_motor_t gimbal_motor[4];  //��̨������ز����ṹ��

void CAN1_Configuration(void)
{
    CAN_InitTypeDef        can;
    CAN_FilterInitTypeDef  can_filter;
    GPIO_InitTypeDef       gpio;
    NVIC_InitTypeDef       nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);

    gpio.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOA, &gpio);
    
    nvic.NVIC_IRQChannel = CAN1_RX0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    
    nvic.NVIC_IRQChannel = CAN1_TX_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 2;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);    
    
    CAN_DeInit(CAN1);
    CAN_StructInit(&can);
    
    can.CAN_TTCM = DISABLE;
    can.CAN_ABOM = DISABLE;
    can.CAN_AWUM = DISABLE;
    can.CAN_NART = DISABLE;
    can.CAN_RFLM = DISABLE;
    can.CAN_TXFP = ENABLE;
    can.CAN_Mode = CAN_Mode_Normal;
    can.CAN_SJW  = CAN_SJW_1tq;
    can.CAN_BS1 = CAN_BS1_9tq;
    can.CAN_BS2 = CAN_BS2_4tq;
    can.CAN_Prescaler = 3;   //CAN BaudRate 42/(1+9+4)/3=1Mbps
    CAN_Init(CAN1, &can);

	  can_filter.CAN_FilterNumber=0;
	  can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;
	  can_filter.CAN_FilterScale=CAN_FilterScale_32bit;
	  can_filter.CAN_FilterIdHigh=0x0000;
	  can_filter.CAN_FilterIdLow=0x0000;
	  can_filter.CAN_FilterMaskIdHigh=0x0000;
	  can_filter.CAN_FilterMaskIdLow=0x0000;
	  can_filter.CAN_FilterFIFOAssignment=0;//the message which pass the filter save in fifo0
	  can_filter.CAN_FilterActivation=ENABLE;
	  CAN_FilterInit(&can_filter);
    
    CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
    CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE); 
}



void CAN2_Configuration(void)
{
    CAN_InitTypeDef        can;
    CAN_FilterInitTypeDef  can_filter;
    GPIO_InitTypeDef       gpio;
    NVIC_InitTypeDef       nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_CAN2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_CAN2); 

    gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 ;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOB, &gpio);

    nvic.NVIC_IRQChannel = CAN2_RX0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    
    nvic.NVIC_IRQChannel = CAN2_TX_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 2;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    CAN_DeInit(CAN2);
    CAN_StructInit(&can);

    can.CAN_TTCM = DISABLE;
    can.CAN_ABOM = DISABLE;    
    can.CAN_AWUM = DISABLE;    
    can.CAN_NART = DISABLE;    
    can.CAN_RFLM = DISABLE;    
    can.CAN_TXFP = ENABLE;     
    can.CAN_Mode = CAN_Mode_Normal; 
    can.CAN_SJW  = CAN_SJW_1tq;
    can.CAN_BS1 = CAN_BS1_9tq;
    can.CAN_BS2 = CAN_BS2_4tq;
    can.CAN_Prescaler = 3;   //CAN BaudRate 42/(1+9+4)/3=1Mbps
    CAN_Init(CAN2, &can);
    
    can_filter.CAN_FilterNumber=14;
    can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;
    can_filter.CAN_FilterScale=CAN_FilterScale_32bit;
    can_filter.CAN_FilterIdHigh=0x0000;
    can_filter.CAN_FilterIdLow=0x0000;
    can_filter.CAN_FilterMaskIdHigh=0x0000;
    can_filter.CAN_FilterMaskIdLow=0x0000;
    can_filter.CAN_FilterFIFOAssignment=0;//the message which pass the filter save in fifo0
    can_filter.CAN_FilterActivation=ENABLE;
    CAN_FilterInit(&can_filter);
    
    CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);
    CAN_ITConfig(CAN2,CAN_IT_TME,ENABLE);
}




/********************************************************************************
   ����CAN���ߵ������
   ת��ת�ٷ�Χ  820R�������3510    -32768~32767
                 C620�������3508    -16384~0~16384 
*********************************************************************************/

void CAN1_RX0_IRQHandler(void)
{   
	  CanRxMsg can1_rx_message;	
    if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
        CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);
    		CAN_Receive(CAN1, CAN_FIFO0, &can1_rx_message);
		switch(can1_rx_message.StdId)
         {
             case CAN_3508_M1_ID:
				 Process3508Data(&chasis_motor[0],can1_rx_message.Data);
				 slef_check_count.chasis_motor1++;
				 break;
             case CAN_3508_M2_ID:
				 Process3508Data(&chasis_motor[1],can1_rx_message.Data);
				 slef_check_count.chasis_motor2++;						 
				 break;
             case CAN_3508_M3_ID:
				 Process3508Data(&chasis_motor[2],can1_rx_message.Data);
				 slef_check_count.chasis_motor3++;	
				 break;
             case CAN_3508_M4_ID:
				 Process3508Data(&chasis_motor[3],can1_rx_message.Data);
				 slef_check_count.chasis_motor4++;	
				 break;
             case CAN_YAW_MOTOR_ID://Y����
         Process6623Data(&gimbal_motor[0],can1_rx_message.Data);
				 slef_check_count.gimbal_yaw++;	
				 break;
						case CAN_PIT_MOTOR_ID://P����
			   Process6623Data(&gimbal_motor[1],can1_rx_message.Data);
				 slef_check_count.gimbal_pitch++;
				 break;
             default:break;
		    }
    }
}




void CAN1_TX_IRQHandler(void) //CAN1�����ж�
{
    if (CAN_GetITStatus(CAN1,CAN_IT_TME)!= RESET) 
		{
		
		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
    }
}

void CAN2_TX_IRQHandler(void) //CAN TX
{
  if (CAN_GetITStatus(CAN2,CAN_IT_TME)!= RESET)    //if transmit mailbox is empty 
  {
	   CAN_ClearITPendingBit(CAN2,CAN_IT_TME);   
  }
}


/********************************************************************************
   ����CAN���ߵ������
   ת��ת�ٷ�Χ  820R�������3510    -32768~32767
                 C620�������3508    -16384~0~16384 
*********************************************************************************/
void CAN2_RX0_IRQHandler(void)
{
    CanRxMsg can2_rx_message;
    if (CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET) 
    {
        CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
        CAN_Receive(CAN2, CAN_FIFO0, &can2_rx_message);  
       //������������ݴ���
		
//	switch(can2_rx_message.StdId)
//         {
//       
//		    }
	}
}


  /****************************
���ܣ�      ��е�ǽ���
���룺 ��ǰ�Ƕ�angle����ʼ���Ƕ�first_angle
�������4096Ϊ���ĵ�����4096����λ�þ���ԭ��first_angle��λ��
�Ķ�������ǶȽ�һ����Ϊ����Ƕȱ仯��̧ͷ�ӣ���ͷ����
*****************************/ 
s16 transe_angle(s16 angle,u16 first_angle)
{
		s16 i;
		i=4095-first_angle;
		angle=angle+i;
		if(angle<0)
		angle=angle+8191;
		if(angle>8191)
		angle=angle-8191;
		return angle;	
}

/********************************************************************************
   ������巢��ָ�ID��Ϊ0x200 
   ��Χ  820R�������3510    -32768~32767
         C620�������3508    -16384~0~16384 
   
*********************************************************************************/

void Set_CM_Speed(int16_t cm1_iq, int16_t cm2_iq, int16_t cm3_iq, int16_t cm4_iq)
{  
	  CanTxMsg tx_message;
    tx_message.StdId = 0x200;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
    tx_message.Data[0] = (uint8_t)(cm1_iq >> 8);
    tx_message.Data[1] = (uint8_t)cm1_iq;
    tx_message.Data[2] = (uint8_t)(cm2_iq >> 8);
    tx_message.Data[3] = (uint8_t)cm2_iq;
    tx_message.Data[4] = (uint8_t)(cm3_iq >> 8);
    tx_message.Data[5] = (uint8_t)cm3_iq;
    tx_message.Data[6] = (uint8_t)(cm4_iq >> 8);
    tx_message.Data[7] = (uint8_t)cm4_iq;
  
    CAN_Transmit(CAN1,&tx_message);
}


/********************************************************************************
   ������巢��ָ�ID��Ϊ0x1FF��ֻ����������壬���ݻش�IDΪ0x205��0x206
   ��Χ   -5000~5000
	 cyq:����Ϊ�������������ָ�
*********************************************************************************/
void Set_Gimbal_Current(int16_t gimbal_yaw_iq, int16_t gimbal_pitch_iq)
{
    CanTxMsg tx_message;    
    tx_message.StdId = 0x1FF;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
    tx_message.Data[0] = (unsigned char)(gimbal_yaw_iq >> 8);
    tx_message.Data[1] = (unsigned char)gimbal_yaw_iq;
    tx_message.Data[2] = (unsigned char)(gimbal_pitch_iq >> 8);
    tx_message.Data[3] = (unsigned char)gimbal_pitch_iq;
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
	  
    CAN_Transmit(CAN1,&tx_message);

}


/**
  * @brief    У׼6623
  * @author   ��ţ��ţ��
  * @param[in]      
  * @retval       
 */
void Set_Gimbal_Calibration(void)
{
    CanTxMsg tx_message;    
    tx_message.StdId = 0x3F0;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    
    tx_message.Data[0] = 'c';
    tx_message.Data[1] = 0x00;
    tx_message.Data[2] = 0x00;
    tx_message.Data[3] = 0x00;
    tx_message.Data[4] = 0x00 ;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
  	CAN_Transmit(CAN1,&tx_message);

}


/**
  * @brief    3508��������ID
  * @author   ��ţ��ţ��
  * @param[in]      
  * @retval       
 */
void CAN_CMD_CHASSIS_RESET_ID(void)
{

    CanTxMsg TxMessage;
    TxMessage.StdId = 0x700;
    TxMessage.IDE = CAN_ID_STD;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = 0x08;
    TxMessage.Data[0] = 0;
    TxMessage.Data[1] = 0;
    TxMessage.Data[2] = 0;
    TxMessage.Data[3] = 0;
    TxMessage.Data[4] = 0;
    TxMessage.Data[5] = 0;
    TxMessage.Data[6] = 0;
    TxMessage.Data[7] = 0;

    CAN_Transmit(CAN1, &TxMessage);
}


/**
  * @brief    3508������ݴ���
  * @author   ��ţ��ţ��
  * @param[in]      
  * @retval       
 */

void Process3508Data(chasis_motor_t *motor, u8 *message)
{
		 motor->last_angle=motor->angle;
		 motor->angle = (message[0]<<8)|message[1];
		 motor->speed = (message[2]<<8)|message[3];
		 motor->current=(message[4]<<8)|message[5];
		 motor->temperature=message[6];	
}

/**
  * @brief    6623������ݴ���
  * @author   ��ţ��ţ��
  * @param[in]      
  * @retval       
*/
void Process6623Data(gimbal_motor_t* motor,u8 *message)
{
		motor->real_angle=message[0]<<8|message[1];
		motor->cal_angle=transe_angle(motor->real_angle,flash_save_data_temp.gimbal_yaw_zero);  //��������Ļ�е�Ƕ�
		motor->last_angle=gimbal_motor->this_angle;
		motor->this_angle=gimbal_motor->cal_angle;
		motor->angle_speed=gimbal_motor->this_angle-gimbal_motor->last_angle;
}



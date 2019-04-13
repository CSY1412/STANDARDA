#include "IO_task.h"

/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file
  * @brief
  * @note
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     4-2-2019       施明朝         1. 完成
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 CDTU****************************
 */

 
u8 Upload_Enable_count = 0;
uint8_t app_param_calied_flag = 0;
 extern const unsigned char CRC8_INIT;  // CRC8 ?????
  const uint16_t CRC16_INIT= 0xffff;   // CRC16 ?????
 extern const uint8_t CRC8_TAB[256];
 extern const uint16_t wCRC_Table[256];


unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int dwLength,unsigned char ucCRC8)
{
    unsigned char ucIndex;
    while(dwLength--)
    {
        ucIndex = ucCRC8^(*pchMessage++);
        ucCRC8 = CRC8_TAB[ucIndex];
    }
    return(ucCRC8);
}
/*
** Descriptions: CRC8 Verify function
** Input: Data to Verify,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)
{
    unsigned char ucExpected = 0;

    if ((pchMessage == 0) || (dwLength <= 2)) return 0;
    ucExpected = Get_CRC8_Check_Sum(pchMessage, dwLength-1, CRC8_INIT);
    return ( ucExpected == pchMessage[dwLength-1] );
}

/*
** Descriptions: append CRC8 to the end of data
** Input: Data to CRC and append,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)
{
    unsigned char ucCRC = 0;

    if ((pchMessage == 0) || (dwLength <= 2)) return;
    ucCRC = Get_CRC8_Check_Sum((unsigned char *)pchMessage, dwLength-1, CRC8_INIT);
    pchMessage[dwLength-1] = ucCRC;
}

/*
** Descriptions: CRC16 checksum function
** Input: Data to check,Stream length, initialized checksum
** Output: CRC checksum
*/
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC)
{
    uint8_t chData;

    if (pchMessage == 0)
    {
        return 0xFFFF;
    }
    while(dwLength--)
    {
        chData = *pchMessage++;
        (wCRC) = ((uint16_t)(wCRC) >> 8) ^ wCRC_Table[((uint16_t)(wCRC) ^ (uint16_t)(chData)) & 0x00ff];
    }
    return wCRC;
}

/*
** Descriptions: CRC16 Verify function
** Input: Data to Verify,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength)
{
    uint16_t wExpected = 0;

    if ((pchMessage == 0) || (dwLength <= 2))
    {
        return 0;
    }
    wExpected = Get_CRC16_Check_Sum(pchMessage, dwLength - 2, CRC16_INIT);
    return ((wExpected & 0xff) == pchMessage[dwLength - 2] && ((wExpected >> 8) & 0xff) == pchMessage[dwLength - 1]);
}

/*
** Descriptions: append CRC16 to the end of data
** Input: Data to CRC and append,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength)
{
    uint16_t wCRC = 0;
    if ((pchMessage == 0) || (dwLength <= 2))
    {
        return;
    }
    wCRC = Get_CRC16_Check_Sum ((uint8_t *)pchMessage, dwLength-2, CRC16_INIT);
    pchMessage[dwLength-2] = (uint8_t)(wCRC & 0x00ff);
    pchMessage[dwLength-1] = (uint8_t)((wCRC >> 8)& 0x00ff);
}

/**
   * @brief     pack data to bottom device
   * @param[in] cmd_id:  command id of data
   * @param[in] *p_data: pointer to the data to be sent
   * @param[in] len:     the data length
   */
void data_pack_handle(CarClass_e Car_type,CmdID_e Cmd_id, uint8_t *p_data, uint16_t len)
{
    memset(_USART3_DMA_TX_BUF, 0, HEADER_LEN);
    tFrameHeader *p_header = (tFrameHeader*)_USART3_DMA_TX_BUF;
    p_header->SOF = UP_REG_ID;//insert frame
    p_header->CarID = (unsigned int)Car_type;//insert Car type
    p_header->CmdID = (unsigned int)Cmd_id;
    p_header->DataLength  = len;//insert data length

    Append_CRC8_Check_Sum(_USART3_DMA_TX_BUF, HEADER_LEN);////insert CRC8
//    uint8_t buff[1050];
//   int i ;
//   for(i=0;i<len;i++)
//   {
//     buff[i] = i;
//   }
    memcpy(&_USART3_DMA_TX_BUF[HEADER_LEN], p_data, len);//insert data

    Append_CRC16_Check_Sum(_USART3_DMA_TX_BUF, HEADER_LEN + len + CRC16_LEN);//insert CRC16
}

/**
   * @brief     Verify data from upper device
   * @param[in] *p: pointer to the received data
   */
_Bool DataVerify(uint8_t *p)
{
    TopToBottomData_t *frame= (TopToBottomData_t *)p;

    if(Verify_CRC8_Check_Sum((uint8_t *)frame,sizeof(tFrameHeader)) &&
            Verify_CRC16_Check_Sum((uint8_t *)frame,frame->FrameHeader.DataLength + sizeof(tFrameHeader) + sizeof(frame->CRC16)))
        return 1;
    else return 0;
}

TopToBottomData_t TopToBottomData;
uint8_t data_save_flag = 0;
void TopToBottomDataPPrcess(uint8_t *pData)
{
    if( (pData == NULL) || (DataVerify(pData) == 0) )
    {
        return;
    }
    memcpy(&TopToBottomData,pData,sizeof(TopToBottomData_t));
    flash_save_data_temp.chasis_motor1.kp = (float)TopToBottomData.PID_Data[0].P/100;
    flash_save_data_temp.chasis_motor1.ki = (float)TopToBottomData.PID_Data[0].I/100;
    flash_save_data_temp.chasis_motor1.kd = (float)TopToBottomData.PID_Data[0].D/100;
		
		flash_save_data_temp.chasis_motor2.kp = (float)TopToBottomData.PID_Data[1].P/100;
    flash_save_data_temp.chasis_motor2.ki = (float)TopToBottomData.PID_Data[1].I/100;
    flash_save_data_temp.chasis_motor2.kd = (float)TopToBottomData.PID_Data[1].D/100;

		
		flash_save_data_temp.chasis_motor3.kp = (float)TopToBottomData.PID_Data[2].P/100;
    flash_save_data_temp.chasis_motor3.ki = (float)TopToBottomData.PID_Data[2].I/100;
    flash_save_data_temp.chasis_motor3.kd = (float)TopToBottomData.PID_Data[2].D/100;

				
		flash_save_data_temp.chasis_motor4.kp = (float)TopToBottomData.PID_Data[3].P/100;
    flash_save_data_temp.chasis_motor4.ki = (float)TopToBottomData.PID_Data[3].I/100;
    flash_save_data_temp.chasis_motor4.kd = (float)TopToBottomData.PID_Data[3].D/100;
		
		
		flash_save_data_temp.gimbal_pitch_v.kp = (float)TopToBottomData.PID_Data[4].P/100;
    flash_save_data_temp.gimbal_pitch_v.ki = (float)TopToBottomData.PID_Data[4].I/100;
    flash_save_data_temp.gimbal_pitch_v.kd = (float)TopToBottomData.PID_Data[4].D/100;
		
		
		flash_save_data_temp.gimbal_pitch_p.kp = (float)TopToBottomData.PID_Data[5].P/100;
    flash_save_data_temp.gimbal_pitch_p.ki = (float)TopToBottomData.PID_Data[5].I/100;
    flash_save_data_temp.gimbal_pitch_p.kd = (float)TopToBottomData.PID_Data[5].D/100;
		
		flash_save_data_temp.gimbal_yaw_v.kp = (float)TopToBottomData.PID_Data[6].P/100;
    flash_save_data_temp.gimbal_yaw_v.ki = (float)TopToBottomData.PID_Data[6].I/100;
    flash_save_data_temp.gimbal_yaw_v.kd = (float)TopToBottomData.PID_Data[6].D/100;
		
		flash_save_data_temp.gimbal_yaw_p.kp = (float)TopToBottomData.PID_Data[7].P/100;
    flash_save_data_temp.gimbal_yaw_p.ki = (float)TopToBottomData.PID_Data[7].I/100;
    flash_save_data_temp.gimbal_yaw_p.kd = (float)TopToBottomData.PID_Data[7].D/100;
		
		flash_save_data_temp.chasis_follow_p.kp = (float)TopToBottomData.PID_Data[8].P/100;
    flash_save_data_temp.chasis_follow_p.ki = (float)TopToBottomData.PID_Data[8].I/100;
    flash_save_data_temp.chasis_follow_p.kd = (float)TopToBottomData.PID_Data[8].D/100;
		
		flash_save_data_temp.manifold_follow_y.kp = (float)TopToBottomData.PID_Data[9].P/100;
    flash_save_data_temp.manifold_follow_y.ki = (float)TopToBottomData.PID_Data[9].I/100;
    flash_save_data_temp.manifold_follow_y.kd = (float)TopToBottomData.PID_Data[9].D/100;
		
		flash_save_data_temp.manifold_follow_x.kp = (float)TopToBottomData.PID_Data[10].P/100;
    flash_save_data_temp.manifold_follow_x.ki = (float)TopToBottomData.PID_Data[10].I/100;
    flash_save_data_temp.manifold_follow_x.kd = (float)TopToBottomData.PID_Data[10].D/100;
	
		flash_save_data_temp.power_control.kp = (float)TopToBottomData.PID_Data[11].P/100;
    flash_save_data_temp.power_control.ki = (float)TopToBottomData.PID_Data[11].I/100;
    flash_save_data_temp.power_control.kd = (float)TopToBottomData.PID_Data[11].D/100;
	
		
//    CM2SpeedPID.kp = (float)TopToBottomData.PID_Data[1].P/100;
//    CM2SpeedPID.ki = (float)TopToBottomData.PID_Data[1].I/100;
//    CM2SpeedPID.kd = (float)TopToBottomData.PID_Data[1].D/100;

//    CM3SpeedPID.kp = (float)TopToBottomData.PID_Data[2].P/100;
//    CM3SpeedPID.ki = (float)TopToBottomData.PID_Data[2].I/100;
//    CM3SpeedPID.kd = (float)TopToBottomData.PID_Data[2].D/100;

//    CM4SpeedPID.kp = (float)TopToBottomData.PID_Data[3].P/100;
//    CM4SpeedPID.ki = (float)TopToBottomData.PID_Data[3].I/100;
//    CM4SpeedPID.kd = (float)TopToBottomData.PID_Data[3].D/100;

//    CF_PositionPID.kp = (float)TopToBottomData.PID_Data[4].P/100;
//    CF_PositionPID.ki = (float)TopToBottomData.PID_Data[4].I/100;
//    CF_PositionPID.kd = (float)TopToBottomData.PID_Data[4].D/100;

//    GM_Pitch_PositionPID.kp = (float)TopToBottomData.PID_Data[5].P/100;
//    GM_Pitch_PositionPID.ki = (float)TopToBottomData.PID_Data[5].I/100;
//    GM_Pitch_PositionPID.kd = (float)TopToBottomData.PID_Data[5].D/100;

//    GM_Pitch_SpeedPID.kp = (float)TopToBottomData.PID_Data[6].P/100;
//    GM_Pitch_SpeedPID.ki = (float)TopToBottomData.PID_Data[6].I/100;
//    GM_Pitch_SpeedPID.kd = (float)TopToBottomData.PID_Data[6].D/100;

//    GM_Yaw_PositionPID.kp = (float)TopToBottomData.PID_Data[7].P/100;
//    GM_Yaw_PositionPID.ki = (float)TopToBottomData.PID_Data[7].I/100;
//    GM_Yaw_PositionPID.kd = (float)TopToBottomData.PID_Data[7].D/100;

//    GM_Yaw_SpeedPID.kp = (float)TopToBottomData.PID_Data[8].P/100;
//    GM_Yaw_SpeedPID.ki = (float)TopToBottomData.PID_Data[8].I/100;
//    GM_Yaw_SpeedPID.kd = (float)TopToBottomData.PID_Data[8].D/100;

			 Flash_Data_Save();

}

BottomToTopData_t UploadData;
//上传任务

void Upload_task(void *pvParameters)
{
    static uint32_t uploadCount = 0;
   	u32 lastWakeTime = GetSysTickCnt();  //获取系统时间
	while(1)        //调试内容
	{
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_50_HZ));
        int i;///////////////////填充波形图数据
        UploadData.WaveData[0] = judge_rece_mesg.power_heatdata.chassisPower;
        UploadData.WaveData[1] = cm_motor.current_offset;
        UploadData.WaveData[2] = judge_rece_mesg.power_heatdata.chassisBuffer;
        UploadData.WaveData[3] = 0;
        UploadData.WaveData[4] = 0;                     //电机期望
        UploadData.WaveData[5] = chasis_motor[0].speed; //电机速度
        UploadData.WaveData[6] = imu_data_access.Angle.yaw;
        UploadData.WaveData[7] = imu_data_access.Gyro.x;
        UploadData.WaveData[8] = imu_data_access.Gyro.z;
        UploadData.WaveData[9] = 0;
		

        for(i=0; i<SHOW_DATA_NUM; i++) //填充显示数据
            UploadData.ShowData[i]=i;
			
				
		      UploadData.ShowData[11-10]=judge_rece_mesg.blood_changed_data.hurt_type;
					UploadData.ShowData[12-10]=judge_rece_mesg.blood_changed_data.armor_type;
					UploadData.ShowData[13-10]=judge_rece_mesg.game_information.remain_hp;
					UploadData.ShowData[14-10]=judge_rece_mesg.power_heatdata.chassisVolt;
					UploadData.ShowData[15-10]=judge_rece_mesg.power_heatdata.shooterHeat0;
					UploadData.ShowData[16-10]=judge_rece_mesg.power_heatdata.shooterHeat1;
//        UploadData.ShowData[31-10] = ManifoldData.pitch;//妙算
//        UploadData.ShowData[36-10] = ManifoldData.yaw;
//		
//        UploadData.ShowData[30-10] = Hi219Data.Pitch;//欧拉角
//        UploadData.ShowData[35-10] = Hi219Data.Yaw;
//		
//		
//		
//        UploadData.ShowData[37-10] = Motor5_yaw.extend_angle;
//        UploadData.ShowData[38-10] = Motor6_pitch.extend_angle;
//        UploadData.ShowData[39-10] = (float)OSStatTaskCPUUsage/100;


        for(i=0; i<PID_DATA_LEN; i++) //填充PID数据
        {
            UploadData.PID[i].P=i;
            UploadData.PID[i].I=i+1;
            UploadData.PID[i].D=i+2;
        }
        UploadData.PID[0].P= flash_save_data_temp.chasis_motor1.kp*100;
        UploadData.PID[0].I= flash_save_data_temp.chasis_motor1.ki*100;
        UploadData.PID[0].D= flash_save_data_temp.chasis_motor1.kd*100;

        UploadData.PID[1].P= flash_save_data_temp.chasis_motor2.kp*100;
        UploadData.PID[1].I= flash_save_data_temp.chasis_motor2.ki*100;
        UploadData.PID[1].D= flash_save_data_temp.chasis_motor2.kd*100;

        UploadData.PID[2].P= flash_save_data_temp.chasis_motor3.kp*100;
        UploadData.PID[2].I= flash_save_data_temp.chasis_motor3.ki*100;
        UploadData.PID[2].D= flash_save_data_temp.chasis_motor3.kd*100;
				
				UploadData.PID[3].P= flash_save_data_temp.chasis_motor4.kp*100;
        UploadData.PID[3].I= flash_save_data_temp.chasis_motor4.ki*100;
        UploadData.PID[3].D= flash_save_data_temp.chasis_motor4.kd*100;
				
				
				UploadData.PID[4].P= flash_save_data_temp.gimbal_pitch_v.kp*100;
        UploadData.PID[4].I= flash_save_data_temp.gimbal_pitch_v.ki*100;
        UploadData.PID[4].D= flash_save_data_temp.gimbal_pitch_v.kd*100;

				UploadData.PID[5].P= flash_save_data_temp.gimbal_pitch_p.kp*100;
        UploadData.PID[5].I= flash_save_data_temp.gimbal_pitch_p.ki*100;
        UploadData.PID[5].D= flash_save_data_temp.gimbal_pitch_p.kd*100;

				UploadData.PID[6].P= flash_save_data_temp.gimbal_yaw_v.kp*100;
        UploadData.PID[6].I= flash_save_data_temp.gimbal_yaw_v.ki*100;
        UploadData.PID[6].D= flash_save_data_temp.gimbal_yaw_v.kd*100;
				
				UploadData.PID[7].P= flash_save_data_temp.gimbal_yaw_p.kp*100;
        UploadData.PID[7].I= flash_save_data_temp.gimbal_yaw_p.ki*100;
        UploadData.PID[7].D= flash_save_data_temp.gimbal_yaw_p.kd*100;
				
				UploadData.PID[8].P= flash_save_data_temp.chasis_follow_p.kp*100;
        UploadData.PID[8].I= flash_save_data_temp.chasis_follow_p.ki*100;
        UploadData.PID[8].D= flash_save_data_temp.chasis_follow_p.kd*100;
				
				UploadData.PID[9].P= flash_save_data_temp.manifold_follow_y.kp*100;
        UploadData.PID[9].I= flash_save_data_temp.manifold_follow_y.ki*100;
        UploadData.PID[9].D= flash_save_data_temp.manifold_follow_y.kd*100;
				
				UploadData.PID[10].P= flash_save_data_temp.manifold_follow_x.kp*100;
        UploadData.PID[10].I= flash_save_data_temp.manifold_follow_x.ki*100;
        UploadData.PID[10].D= flash_save_data_temp.manifold_follow_x.kd*100;
				
				UploadData.PID[11].P= flash_save_data_temp.power_control.kp*100;
        UploadData.PID[11].I= flash_save_data_temp.power_control.ki*100;
        UploadData.PID[11].D= flash_save_data_temp.power_control.kd*100;
				
				
//        UploadData.PID[3].P=100*  CM4SpeedPID.kp;
//        UploadData.PID[3].I=100*  CM4SpeedPID.ki;
//        UploadData.PID[3].D=100*  CM4SpeedPID.kd;

//        UploadData.PID[4].P=100*  CF_PositionPID.kp;
//        UploadData.PID[4].I=100*  CF_PositionPID.ki;
//        UploadData.PID[4].D=100*  CF_PositionPID.kd;

//        UploadData.PID[5].P=100*  GM_Pitch_PositionPID.kp;
//        UploadData.PID[5].I=100*  GM_Pitch_PositionPID.ki;
//        UploadData.PID[5].D=100*  GM_Pitch_PositionPID.kd;

//        UploadData.PID[6].P=100*  GM_Pitch_SpeedPID.kp;
//        UploadData.PID[6].I=100*  GM_Pitch_SpeedPID.ki;
//        UploadData.PID[6].D=100*  GM_Pitch_SpeedPID.kd;

//        UploadData.PID[7].P=100*  GM_Yaw_PositionPID.kp;
//        UploadData.PID[7].I=100*  GM_Yaw_PositionPID.ki;
//        UploadData.PID[7].D=100*  GM_Yaw_PositionPID.kd;

//        UploadData.PID[8].P=100*  GM_Yaw_SpeedPID.kp;
//        UploadData.PID[8].I=100*  GM_Yaw_SpeedPID.ki;
//        UploadData.PID[8].D=100*  GM_Yaw_SpeedPID.kd;


        if(uploadCount >= 200) {
            if((uploadCount%50) == 0) {//PID上传周期  20*50=1000ms
                data_pack_handle(THIS_CAR_TYPE,PID_DATA, (uint8_t*)&UploadData.PID,  sizeof(UploadData.PID));
                USART3_MYDMA_Enable(sizeof(UploadData.PID)+HEADER_LEN+CRC16_LEN);
            } else if(((uploadCount%10) == 0 )&&(printf_buff_len > 0)) {//printf打印周期 20*10 = 200ms
                data_pack_handle(THIS_CAR_TYPE,PRINTF_DATA,(uint8_t*)&printf_buff, printf_buff_len);
                USART3_MYDMA_Enable(printf_buff_len+HEADER_LEN+CRC16_LEN);
                memset(printf_buff, 0, BSP_USART3_PRINTF_BUF_LEN);
                printf_buff_len=0;
            } else if((uploadCount%5) == 0) {//普通显示数据周期 20*5=100ms
                data_pack_handle(THIS_CAR_TYPE,SHOW_DATA, (uint8_t*)&UploadData.ShowData,  sizeof(UploadData.ShowData));
                USART3_MYDMA_Enable(sizeof(UploadData.ShowData)+HEADER_LEN+CRC16_LEN);
            } else {
                data_pack_handle(THIS_CAR_TYPE,WAVE_DATA, (uint8_t*)&UploadData.WaveData,  sizeof(UploadData.WaveData));
                USART3_MYDMA_Enable(sizeof(UploadData.WaveData)+HEADER_LEN+CRC16_LEN);
            }
        }
        uploadCount++;
			//任务延时
    }
}


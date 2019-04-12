/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file      
  * @brief    	��̨������
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     ��ţ��ţ��     1. ���
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  */

#include "GIMBAL_task.h"

pitch_t pitchdata={0,0,0};
yaw_t yawdata={0,0,0};

/**
  * @brief         	��̨����
  * @author         ��ţ��ţ��
  * @param[in]         
  * @retval       
  */
void GIMBAL_task(void *pvParameters)
{
	u32 lastWakeTime = GetSysTickCnt();  //��ȡϵͳʱ��
	while(1)        //��������
	{
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_1000_HZ));
				
		if (RATE_DO_EXECUTE(RATE_1000_HZ, GetSysTickCnt()))  //100hz
			{
				
				
				if(gimabal_main_stat==free_and_preparing)
				{
						Set_Gimbal_Current(0,0); //�ͷ���̨					
				}		
				else if(gimabal_main_stat==control)
				{
						GimbalCtl();   //��̨����				
				}
			}
	}
}

/**
  * @brief         ��̨����
  * @author         ��ţ��ţ��
  * @param[in]      
  * @param[in]     
  * @param[in]      
  * @param[in]     
  * @retval       
  */
void GimbalCtl(void)
{			
		if(gimabal_aux_mode==normal)
		{
			 GimbalNomalCtl();
		}
		if(gimabal_aux_mode==self_aim)
		{
			 GimbalSlefaimCtl();
		}
		if(gimabal_aux_mode==small_god_symbal)
		{
		
		}
		if(gimabal_aux_mode==big_god_symbal)
		{
		
		}		
		if(gimabal_aux_mode==slowing)
		{
			FirstGimbalBack();    //��̨�״ι�λ
			CheckUpFinishi();
		}
		if(gimabal_aux_mode==back_posion)  //���ٻ�λ
		{
		  GimbalQuickBack();
			CheckUpQuickBackFinishi();			
		}
}

/**
  * @brief         	��̨��һ�λ�λ
  * @author         ��ţ��ţ��
  * @param[in]      
  * @param[in]     
  * @param[in]      
  * @param[in]     
  * @retval       
  */
void FirstGimbalBack(void)    
{
			gimbal_back_yaw_pid_p.kp=0.7;
			yawdata.yawtemp=-DJI_PID_Cal(&gimbal_back_yaw_pid_p,(gimbal_motor[0].cal_angle-4096),0,YAWBACKROTATESPEEDMAX);				
			yawdata.yawout=-DJI_PID_Cal(&gimbal_yaw_pid_v,imu_data_access.Gyro.z,yawdata.yawtemp,GIMBAL6623_MOTOR_MAX_CURRENT);		
			pitchdata.pitchexp=0;  //��������
			LimitAngle(&pitchdata.pitchexp);
			
//			pitchdata.pitchtemp=DJI_PID_Cal(&gimbal_pitch_pid_p,gesture_data.Angle.Pitch,0,PITCHBACKROTATESPEEDMAX);
			pitchdata.pitchtemp=DJI_PID_Cal(&gimbal_pitch_pid_p,(float)(gimbal_motor[1].this_angle-4096),pitchdata.pitchexp,PITCHBACKROTATESPEEDMAX);
			pitchdata.pitchout= DJI_PID_Cal(&gimbal_pitch_pid_v,imu_data_access.Gyro.x,pitchdata.pitchtemp,GIMBAL6623_MOTOR_MAX_CURRENT);
	
			Set_Gimbal_Current(-yawdata.yawout,-pitchdata.pitchout);
	
}

/**
  * @brief          ��̨��������
  * @author         ��ţ��ţ��
  * @param[in]           
  * @retval       
  */
void GimbalNomalCtl(void)   //��̨����
{
			if(control_mode==remote)//ң��ģʽ
			{							
				
				yawdata.yawexp+=ProcessGimbalexp((cal_ch.ch2-1024)*REMOTEYAWMOVERATE,MAXANGLEOFFSET);  //��������	
				pitchdata.pitchexp+=((cal_ch.ch3-1024)*REMOTEPITCHMOVERATE);  //��������
				
				GimbalPitchAngleLimit(PITCH_MAX,PITCHMIN);//����
//				ProtectGimbal();//��λ����				
			}
				if(control_mode==keyboard)//������
			{	

				yawdata.yawexp+=ProcessGimbalexp((RC_Ctl.mouse.x)*MOUSEYAWMOVERATE,MAXANGLEOFFSET);  //��������	
				pitchdata.pitchexp-=((RC_Ctl.mouse.y)*MOUSEPITCHMOVERATE);  //��������
				
				GimbalPitchAngleLimit(PITCH_MAX,PITCHMIN);//����
//				ProtectGimbal();//��λ����				
			}
		
			yawdata.yawtemp=DJI_PID_Cal(&gimbal_yaw_pid_p,extend_angle.ecd_value,yawdata.yawexp,YAW_ROTATE_SPEED_MAX);				
			yawdata.yawout=-DJI_PID_Cal(&gimbal_yaw_pid_v,imu_data_access.Gyro.z,yawdata.yawtemp,GIMBAL6623_MOTOR_MAX_CURRENT);		
				
	//		pitchdata.pitchtemp=DJI_PID_Cal(&gimbal_pitch_pid_p,gesture_data.Angle.Pitch,pitchdata.pitchexp,PITCHROTATESPEEDMAX);
			pitchdata.pitchtemp=DJI_PID_Cal(&gimbal_pitch_pid_p,(float)(gimbal_motor[1].this_angle-4096),pitchdata.pitchexp,PITCHROTATESPEEDMAX);
			pitchdata.pitchout= DJI_PID_Cal(&gimbal_pitch_pid_v,imu_data_access.Gyro.x,pitchdata.pitchtemp,GIMBAL6623_MOTOR_MAX_CURRENT);
	
			Set_Gimbal_Current(-yawdata.yawout,-pitchdata.pitchout);
}


/**
  * @brief         ���Ƹ����Ƕ�
  * @author         ��ţ��ţ��
  * @param[in]      ��Ҫ���Ƶ����ݵĵ�ַ   
  * @retval       
  */

void LimitAngle(float* exppitch)
{
		if(*exppitch>PITCH_MAX)
		{
			 *exppitch=PITCH_MAX;
		}
		if(*exppitch<PITCHMIN)
		{
			 *exppitch=PITCHMIN;
		}
}
/**
  * @brief     ����һ�θ�λ���     
  * @author    ��ţ��ţ��
  * @param[in]          
  * @retval       
  */
void CheckUpFinishi(void)
{
		u32 nowtime = GetSysTickCnt();  //��ȡϵͳʱ��
		if(nowtime-last_back_time>SLOWLING_BACK_TIME)
		{
			yawdata.yawexp=extend_angle.ecd_value; //����ԭ��
		//	pitchdata.pitchexp=gesture_data.Angle.Pitch; //����ԭ��
			KeepPitchexp();
			first_order_filter_init(&chassis_slow_set_vx,0.0025f,CHASSIS_ACCEL_X_NUM);    //һ�׵�ͨ�˲�����б����Ϊ�����ٶ�����
			first_order_filter_init(&chassis_slow_set_vy,0.0025f,CHASSIS_ACCEL_Y_NUM);    
			gimabal_aux_mode=normal;  //�ص�����ģʽ
			chasis_follow_stat=follow_gimabal;//���̸���		
		}
}


/**
  * @brief    ��̨��λ���� ����ң�������������в��ң�Yaw������������ƫ���м�ֵ����
							�ı���̨������    
  * @author   ��ţ��ţ��
  * @param[in]         
  * @retval       
  */
void ProtectGimbal(void)  //��̨����
{
		if(cal_ch.ch2==1024&&fabs(gimbal_motor[0].cal_angle-4096)>YAWPROTECTECONDERDETA)
		{
				yawdata.yawexp=extend_angle.ecd_value; 
		}
}


/**
  * @brief    YAW������	    
  * @author   ��ţ��ţ��
  * @param[in] �������������ֵ
  * @param[in] ң����ͨ��ֵ����
  * @retval       
  */
void GimbalYawAngleLimit(float max_angle_offset)
{	
		
		if(gimbal_motor[0].cal_angle-4096>max_angle_offset||gimbal_motor[0].cal_angle-4096<-max_angle_offset)
			{			
				yawdata.yawexp=extend_angle.ecd_value;  //���¸�ֵ		
			}		
}

/**
  * @brief    PITCH���������������	    
  * @author   ��ţ��ţ��
  * @param[in] ���PITCH
	* @param[in] ��СPITCH
  * @param[in] ң����ͨ��ֵ����
  * @retval       
  */
void GimbalPitchAngleLimit(float max_pitch,float min_pitch)
{
		pitchdata.pitchexp=fp32_constrain(pitchdata.pitchexp,min_pitch,max_pitch);
}

/**
  * @brief   ��̨���ٹ�λ	    
  * @author   ��ţ��ţ��
  * @param[in] 
	* @param[in] 
  * @param[in] 
  * @retval       
  */
void GimbalQuickBack(void)    
{
		
			gimbal_back_yaw_pid_p.kp=0.7;
			yawdata.yawtemp=-DJI_PID_Cal(&gimbal_back_yaw_pid_p,(gimbal_motor[0].cal_angle-4096),0,YAWQUICKBACKROTATESPEEDMAX);				
			yawdata.yawout=-DJI_PID_Cal(&gimbal_yaw_pid_v,imu_data_access.Gyro.z,yawdata.yawtemp,GIMBAL6623_MOTOR_MAX_CURRENT);		
			pitchdata.pitchexp=0;  
			LimitAngle(&pitchdata.pitchexp);

//			pitchdata.pitchtemp=DJI_PID_Cal(&gimbal_pitch_pid_p,gesture_data.Angle.Pitch,0,PITCHQUICKBACKROTATESPEEDMAX);
	
			pitchdata.pitchtemp=DJI_PID_Cal(&gimbal_pitch_pid_p,(float)(gimbal_motor[1].this_angle-4096),0,PITCHQUICKBACKROTATESPEEDMAX);
			pitchdata.pitchout= DJI_PID_Cal(&gimbal_pitch_pid_v,imu_data_access.Gyro.x,pitchdata.pitchtemp,GIMBAL6623_MOTOR_MAX_CURRENT);
	
			Set_Gimbal_Current(-yawdata.yawout,-pitchdata.pitchout);
}


/**
  * @brief   ��̨���ٹ�λ���	    
  * @author   ��ţ��ţ��
  * @param[in] 
	* @param[in] 
  * @param[in] 
  * @retval       
  */
void CheckUpQuickBackFinishi(void)
{
		u32 nowtime = GetSysTickCnt();  //��ȡϵͳʱ��
		if(nowtime-last_back_time>QUICKBACKTIME)
		{
			yawdata.yawexp=extend_angle.ecd_value; //����ԭ��
//			pitchdata.pitchexp=gesture_data.Angle.Pitch; //����ԭ��
			KeepPitchexp();
			first_order_filter_init(&chassis_slow_set_vx,0.0025f,CHASSIS_ACCEL_X_NUM);    //һ�׵�ͨ�˲�����б����Ϊ�����ٶ�����
			first_order_filter_init(&chassis_slow_set_vy,0.0025f,CHASSIS_ACCEL_Y_NUM);    
			gimabal_aux_mode=normal;  //�ص�����ģʽ
			chasis_follow_stat=follow_gimabal;//���̸���		
		}
}

/**
  * @brief   ��̨����ģʽ   
  * @author   ��ţ��ţ��
  * @param[in] 
	* @param[in] 
  * @param[in] 
  * @retval       
  */

void GimbalSlefaimCtl(void)
{				
			
				if (RATE_DO_EXECUTE(RATE_100_HZ, GetSysTickCnt()))  //100hz
			{		
				pitchdata.pitchtemp=DJI_PID_Cal(&gimbal_pitch_slef_aim_pid,manifold.y,0,PITCHROTATESPEEDMAX);
				yawdata.yawtemp=DJI_PID_Cal(&gimbal_yaw_slef_aim_pid,manifold.x,0,YAW_ROTATE_SPEED_MAX);				
			}
						
			yawdata.yawout=-DJI_PID_Cal(&gimbal_yaw_pid_v,imu_data_access.Gyro.z,yawdata.yawtemp,GIMBAL6623_MOTOR_MAX_CURRENT);					
			pitchdata.pitchout= DJI_PID_Cal(&gimbal_pitch_pid_v,imu_data_access.Gyro.x,pitchdata.pitchtemp,GIMBAL6623_MOTOR_MAX_CURRENT);
	
			Set_Gimbal_Current(-yawdata.yawout,-pitchdata.pitchout);
			
}

void KeepPitchexp(void)
{
		pitchdata.pitchexp=(float)(gimbal_motor[1].this_angle-4096);

}

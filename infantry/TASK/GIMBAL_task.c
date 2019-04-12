/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file      
  * @brief    	云台任务函数
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     蜗牛蜗牛跑     1. 完成
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
  * @brief         	云台任务
  * @author         蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
void GIMBAL_task(void *pvParameters)
{
	u32 lastWakeTime = GetSysTickCnt();  //获取系统时间
	while(1)        //调试内容
	{
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_1000_HZ));
				
		if (RATE_DO_EXECUTE(RATE_1000_HZ, GetSysTickCnt()))  //100hz
			{
				
				
				if(gimabal_main_stat==free_and_preparing)
				{
						Set_Gimbal_Current(0,0); //释放云台					
				}		
				else if(gimabal_main_stat==control)
				{
						GimbalCtl();   //云台控制				
				}
			}
	}
}

/**
  * @brief         云台控制
  * @author         蜗牛蜗牛跑
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
			FirstGimbalBack();    //云台首次归位
			CheckUpFinishi();
		}
		if(gimabal_aux_mode==back_posion)  //快速回位
		{
		  GimbalQuickBack();
			CheckUpQuickBackFinishi();			
		}
}

/**
  * @brief         	云台第一次回位
  * @author         蜗牛蜗牛跑
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
			pitchdata.pitchexp=0;  //计算期望
			LimitAngle(&pitchdata.pitchexp);
			
//			pitchdata.pitchtemp=DJI_PID_Cal(&gimbal_pitch_pid_p,gesture_data.Angle.Pitch,0,PITCHBACKROTATESPEEDMAX);
			pitchdata.pitchtemp=DJI_PID_Cal(&gimbal_pitch_pid_p,(float)(gimbal_motor[1].this_angle-4096),pitchdata.pitchexp,PITCHBACKROTATESPEEDMAX);
			pitchdata.pitchout= DJI_PID_Cal(&gimbal_pitch_pid_v,imu_data_access.Gyro.x,pitchdata.pitchtemp,GIMBAL6623_MOTOR_MAX_CURRENT);
	
			Set_Gimbal_Current(-yawdata.yawout,-pitchdata.pitchout);
	
}

/**
  * @brief          云台正常控制
  * @author         蜗牛蜗牛跑
  * @param[in]           
  * @retval       
  */
void GimbalNomalCtl(void)   //云台控制
{
			if(control_mode==remote)//遥控模式
			{							
				
				yawdata.yawexp+=ProcessGimbalexp((cal_ch.ch2-1024)*REMOTEYAWMOVERATE,MAXANGLEOFFSET);  //计算期望	
				pitchdata.pitchexp+=((cal_ch.ch3-1024)*REMOTEPITCHMOVERATE);  //计算期望
				
				GimbalPitchAngleLimit(PITCH_MAX,PITCHMIN);//限制
//				ProtectGimbal();//移位保护				
			}
				if(control_mode==keyboard)//鼠标键盘
			{	

				yawdata.yawexp+=ProcessGimbalexp((RC_Ctl.mouse.x)*MOUSEYAWMOVERATE,MAXANGLEOFFSET);  //计算期望	
				pitchdata.pitchexp-=((RC_Ctl.mouse.y)*MOUSEPITCHMOVERATE);  //计算期望
				
				GimbalPitchAngleLimit(PITCH_MAX,PITCHMIN);//限制
//				ProtectGimbal();//移位保护				
			}
		
			yawdata.yawtemp=DJI_PID_Cal(&gimbal_yaw_pid_p,extend_angle.ecd_value,yawdata.yawexp,YAW_ROTATE_SPEED_MAX);				
			yawdata.yawout=-DJI_PID_Cal(&gimbal_yaw_pid_v,imu_data_access.Gyro.z,yawdata.yawtemp,GIMBAL6623_MOTOR_MAX_CURRENT);		
				
	//		pitchdata.pitchtemp=DJI_PID_Cal(&gimbal_pitch_pid_p,gesture_data.Angle.Pitch,pitchdata.pitchexp,PITCHROTATESPEEDMAX);
			pitchdata.pitchtemp=DJI_PID_Cal(&gimbal_pitch_pid_p,(float)(gimbal_motor[1].this_angle-4096),pitchdata.pitchexp,PITCHROTATESPEEDMAX);
			pitchdata.pitchout= DJI_PID_Cal(&gimbal_pitch_pid_v,imu_data_access.Gyro.x,pitchdata.pitchtemp,GIMBAL6623_MOTOR_MAX_CURRENT);
	
			Set_Gimbal_Current(-yawdata.yawout,-pitchdata.pitchout);
}


/**
  * @brief         限制俯仰角度
  * @author         蜗牛蜗牛跑
  * @param[in]      需要限制的数据的地址   
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
  * @brief     检测第一次复位完成     
  * @author    蜗牛蜗牛跑
  * @param[in]          
  * @retval       
  */
void CheckUpFinishi(void)
{
		u32 nowtime = GetSysTickCnt();  //获取系统时间
		if(nowtime-last_back_time>SLOWLING_BACK_TIME)
		{
			yawdata.yawexp=extend_angle.ecd_value; //保持原地
		//	pitchdata.pitchexp=gesture_data.Angle.Pitch; //保持原地
			KeepPitchexp();
			first_order_filter_init(&chassis_slow_set_vx,0.0025f,CHASSIS_ACCEL_X_NUM);    //一阶低通滤波代替斜波作为底盘速度输入
			first_order_filter_init(&chassis_slow_set_vy,0.0025f,CHASSIS_ACCEL_Y_NUM);    
			gimabal_aux_mode=normal;  //回到正常模式
			chasis_follow_stat=follow_gimabal;//底盘跟随		
		}
}


/**
  * @brief    云台移位保护 （当遥控期望在死区中并且，Yaw电机编码器电机偏离中间值过大，
							改变云台期望）    
  * @author   蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
void ProtectGimbal(void)  //云台保护
{
		if(cal_ch.ch2==1024&&fabs(gimbal_motor[0].cal_angle-4096)>YAWPROTECTECONDERDETA)
		{
				yawdata.yawexp=extend_angle.ecd_value; 
		}
}


/**
  * @brief    YAW轴限制	    
  * @author   蜗牛蜗牛跑
  * @param[in] 允许编码器最大差值
  * @param[in] 遥控器通道值输入
  * @retval       
  */
void GimbalYawAngleLimit(float max_angle_offset)
{	
		
		if(gimbal_motor[0].cal_angle-4096>max_angle_offset||gimbal_motor[0].cal_angle-4096<-max_angle_offset)
			{			
				yawdata.yawexp=extend_angle.ecd_value;  //重新给值		
			}		
}

/**
  * @brief    PITCH轴期望计算和限制	    
  * @author   蜗牛蜗牛跑
  * @param[in] 最大PITCH
	* @param[in] 最小PITCH
  * @param[in] 遥控器通道值输入
  * @retval       
  */
void GimbalPitchAngleLimit(float max_pitch,float min_pitch)
{
		pitchdata.pitchexp=fp32_constrain(pitchdata.pitchexp,min_pitch,max_pitch);
}

/**
  * @brief   云台快速归位	    
  * @author   蜗牛蜗牛跑
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
  * @brief   云台快速归位检查	    
  * @author   蜗牛蜗牛跑
  * @param[in] 
	* @param[in] 
  * @param[in] 
  * @retval       
  */
void CheckUpQuickBackFinishi(void)
{
		u32 nowtime = GetSysTickCnt();  //获取系统时间
		if(nowtime-last_back_time>QUICKBACKTIME)
		{
			yawdata.yawexp=extend_angle.ecd_value; //保持原地
//			pitchdata.pitchexp=gesture_data.Angle.Pitch; //保持原地
			KeepPitchexp();
			first_order_filter_init(&chassis_slow_set_vx,0.0025f,CHASSIS_ACCEL_X_NUM);    //一阶低通滤波代替斜波作为底盘速度输入
			first_order_filter_init(&chassis_slow_set_vy,0.0025f,CHASSIS_ACCEL_Y_NUM);    
			gimabal_aux_mode=normal;  //回到正常模式
			chasis_follow_stat=follow_gimabal;//底盘跟随		
		}
}

/**
  * @brief   云台自瞄模式   
  * @author   蜗牛蜗牛跑
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

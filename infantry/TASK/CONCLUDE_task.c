/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file      CONCLUDE_task.c
  * @brief     总结底盘在x轴y轴和z轴方向上的速度期望
							 并进行PID计算和功率控制
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

#include "CONCLUDE_task.h"
#include "math.h"
cm_motor_t cm_motor;


/**
  * @brief         	总结任务函数
  * @author         蜗牛蜗牛跑
  * @param[in]      
  * @param[in]     
  * @param[in]      
  * @param[in]     
  * @retval       
  */
void CONCLUDE_task(void *pvParameters)
{		
	u32 lastWakeTime = GetSysTickCnt();  //获取系统时间
	while(1)        //调试内容
	{
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_1000_HZ));

		if (RATE_DO_EXECUTE(RATE_250_HZ, GetSysTickCnt()))  //250hz
			{
					if(system_stat!=preparing)   //不处于准备状态				
				{
					CalChasisAndSend();
				}
			}

	}
			
}


/**
  * @brief         底盘电机速度计算和发送
  * @author        蜗牛蜗牛跑
  * @param[in]          
  * @retval       
  */
void CalChasisAndSend(void)  
{
		
	 chassis_set_contorl(&chasis_control.vx,&chasis_control.vy,gimbal_motor[0].cal_angle);
	
	 chassis_vector_to_mecanum_wheel_speed(chasis_control.vx,chasis_control.vy,
																					chasis_control.rotate,cm_motor.cm_temp);//速度合成
			
		for(int i =0;i<4;i++)
	{
		  cm_motor.cm_out[i]=	PID_Calc(&chassis_motor[i],chasis_motor[i].speed,cm_motor.cm_temp[i],MAX_M3508_CURRENT);
	}	

		
	  Set_CM_Speed(cm_motor.cm_out[0],cm_motor.cm_out[1],cm_motor.cm_out[2],cm_motor.cm_out[3]);//发送电机电流
}

/**
  * @brief         电机速度分析和限制
  * @author        蜗牛蜗牛跑
  * @param[in]     4个电机期望转速变量地址    
  * @retval       
  */		
void MotorSpeedAnalyze(float *wheel_speed)
{
		    //计算轮子控制最大速度，并限制其最大速度
	 fp32  temp=0.0f,max_vector = 0.0f, vector_rate = 0.0f;
	
    for (int i = 0; i < 4; i++)
    {				
        temp = fabs(wheel_speed[i]);
        if (max_vector < temp)
        {
            max_vector = temp;
        }
    }

    if (max_vector > MAX_WHEEL_SPEED)
    {
        vector_rate = MAX_WHEEL_SPEED / max_vector;
        for (int i = 0; i < 4; i++)
        {
            wheel_speed[i] *= vector_rate;
        }
    }

		
		
				
}
/**
  * @brief         功率限制
  * @author        蜗牛蜗牛跑
  * @param[in]        
  * @retval       
  */		


void CurrentControl(void)
{
		if(judge_rece_mesg.power_heatdata.chassisPower>MAX_CHASIS_POWER)  //当功率超过最大功率时
	 {
//	    cm_motor.current_offset=DJI_PID_Cal(&chasis_motor_power_pid,judge_rece_mesg.power_heatdata.chassisPower,
//																								MAX_CHASIS_POWER,7000);
	
		 if(cm_motor.current_offset>0)
				 cm_motor.current_offset=0;
		  
	 }
	 else
	 {
			 cm_motor.current_offset=0; //偏差清零
	 }
	 
	 	first_order_filter_cali(&chasis_power_control, cm_motor.current_offset);
}



/**
  * @brief    麦克纳姆轮速度分解  
  * @author   蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */


static void chassis_vector_to_mecanum_wheel_speed(const fp32 vx_set, const fp32 vy_set, const fp32 wz_set, fp32 * wheel_speed)
{
    //旋转的时候， 由于云台靠前，所以是前面两轮 0 ，1 旋转的速度变慢， 后面两轮 2,3 旋转的速度变快
    wheel_speed[0] = vx_set + vy_set + (CHASSIS_ROATE_SET_SCALE - 1.0f)* MOTOR_DISTANCE_TO_CENTER * wz_set;
    wheel_speed[1] = vx_set - vy_set + (CHASSIS_ROATE_SET_SCALE - 1.0f) * MOTOR_DISTANCE_TO_CENTER * wz_set;
    wheel_speed[2] = -vx_set + vy_set +(-CHASSIS_ROATE_SET_SCALE - 1.0f) * MOTOR_DISTANCE_TO_CENTER * wz_set;
    wheel_speed[3] = -vx_set - vy_set +(-CHASSIS_ROATE_SET_SCALE - 1.0f) * MOTOR_DISTANCE_TO_CENTER * wz_set;
}


/**
  * @brief    弧度转换 
  * @author   小技师
  * @param[in]         
  * @retval       
  */
static float rad(float input)
{
	return input*PI/180.0f;
}

/**
  * @brief    坐标系转换 
  * @author   小技师
  * @param[in]         
  * @retval       
  */
void chassis_set_contorl(float *exp_vx,float *exp_vy,float econder_angle)
{
		float x = *exp_vx;
		float y = *exp_vy;

			*exp_vx = y * sin(rad(econder_angle)) + x * cos(rad(econder_angle));
	    *exp_vy = y * cos(rad(econder_angle)) - x * sin(rad(econder_angle));
}


/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file      CHASIS_task.c
  * @brief     
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
#include "CHASIS_task.h"


//chasis_expspeed_t chasis_expspeed={0};//速度平面期望
//chasis_expspeed_t key_expspeed={0};//速度平面期望
chasis_control_t chasis_control;
chasis_control_t chasis_control_temp;


float chasis_follow_ecd_exp=0;


/**
  * @brief    底盘任务函数     
  * @author   蜗牛蜗牛跑
  * @param[in]      
  * @param[in]     
  * @param[in]      
  * @param[in]     
  * @retval       
  */

void CHASIS_task(void *pvParameters)
{
	u32 lastWakeTime = GetSysTickCnt();  //获取系统时间
	while(1)        //调试内容
	{
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_1000_HZ));
			if(confirmSystemStat(running))  //运行模式
			{	
					if (RATE_DO_EXECUTE(RATE_250_HZ, GetSysTickCnt()))  //250hz
				{
						ChasisControl();  //底盘控制
				}			
			}
			else if(confirmSystemStat(self_stabilization)) //稳定模式
			{
				LockChasis();   //底盘锁定
			}
			
	#if COMPETETION		  //如果在比赛出故障也要继续运行
			else if(system_stat==breaking)
			{
				ChasisControl();  //底盘控制 
			}
	#else
			else if(confirmSystemStat(breaking))
			{
				Set_CM_Speed(0,0,0,0);
				BeepON();	
			}			
	#endif			
			else
			{
				Set_CM_Speed(0,0,0,0);  //释放电机			
			}
		
	}
}


/**
  * @brief    底盘控制实现函数     
  * @author   蜗牛蜗牛跑
  * @param[in]           
  * @retval       
  */

void ChasisControl(void)
{
		if(chasis_follow_stat==follow_gimabal)  //如果处于云台跟随状态
		{
				ChasisRoteCal();
		}		
		if(chasis_follow_stat==self_move)    //云台不跟随
		{
			chasis_control.rotate=0;  //无旋转量
		}
		if(control_mode==remote)//遥控模式
		{
			
																	/*斜坡*/			
					  chasis_control.vx=first_order_filter_cali(&chassis_slow_set_vx,((cal_ch.ch0-1024)/660)*RC_CHASSIS_SPEED_X);  //水平速度期望  
					  chasis_control.vy=first_order_filter_cali(&chassis_slow_set_vy,((cal_ch.ch1-1024)/660)*RC_CHASSIS_SPEED_Y); //竖直速度期望
					 //停止信号，不需要缓慢加速，直接减速到零
				#if SPEED_REDUCE_SLPOP
								if ((cal_ch.ch0-1024)==0)
									{
										chasis_control.vx = 0.0f;
									}
											if ((cal_ch.ch1-1024)==0)
									{
										chasis_control.vy = 0.0f;
									}
				#endif				
		}	
		
				if(control_mode==keyboard)//键盘模式
		{				
				if(key_boad.key_W)chasis_control_temp.vy=KEY_CHASSIS_SPEED_Y;
						else if(key_boad.key_S)chasis_control_temp.vy=-KEY_CHASSIS_SPEED_Y;
							else chasis_control_temp.vy=0;
				if(key_boad.key_A)chasis_control_temp.vx=-KEY_CHASSIS_SPEED_X;
						else if(key_boad.key_D)chasis_control_temp.vx=KEY_CHASSIS_SPEED_X;
								else chasis_control_temp.vx=0;			
				if(key_boad.key_SHIFT&&key_boad.key_W)chasis_control_temp.vy=SHIFT_CHASSIS_SPEED_Y;  //shift加速
					
			  chasis_control.vx=first_order_filter_cali(&chassis_slow_set_vx,chasis_control_temp.vx);  //水平速度期望  
				chasis_control.vy=first_order_filter_cali(&chassis_slow_set_vy,chasis_control_temp.vy); //竖直速度期望
					 //停止信号，不需要缓慢加速，直接减速到零
				#if SPEED_REDUCE_SLPOP
								if ((!key_boad.key_A)&&(!key_boad.key_D))
									{
										  chasis_control.vx = 0.0f;
									}
											if ((!key_boad.key_W)&&(!key_boad.key_S))
									{
											chasis_control.vy = 0.0f;
									}
				#endif				
		}	
		
}

/**
  * @brief    锁定底盘     
  * @author   蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
void LockChasis(void) 
{
		chasis_control.vx=0;
		chasis_control.vy=0;
		chasis_control.rotate=0;      
}



/**
  * @brief    底盘旋转量计算     
  * @author   蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */
void ChasisRoteCal(void)
{
	if(key_boad.key_Q==PRESS&&key_boad.key_E==DISSPRESS)
		{
			chasis_follow_ecd_exp=-MAX_CHASIS_FOLLOW_ECD;
			first_order_filter_cali(&chassis_follow_QE,chasis_follow_ecd_exp);	
//			chasis_expspeed.rotate=DJI_PID_Cal(&chasis_follow_pid_p,(gimbal_motor[0].cal_angle-4096),chassis_follow_QE.out,QE_ROATE_SPEED_MAX);  //电机中间值
		}
		else if(key_boad.key_Q==DISSPRESS&&key_boad.key_E==PRESS)
		{
			chasis_follow_ecd_exp=MAX_CHASIS_FOLLOW_ECD;
			first_order_filter_cali(&chassis_follow_QE,chasis_follow_ecd_exp);	
//			chasis_expspeed.rotate=DJI_PID_Cal(&chasis_follow_pid_p,(gimbal_motor[0].cal_angle-4096),chassis_follow_QE.out,QE_ROATE_SPEED_MAX);  //电机中间值			
		}
		else
		{
			chasis_follow_ecd_exp=0;
			first_order_filter_cali(&chassis_follow_QE,chasis_follow_ecd_exp);	
//			chasis_expspeed.rotate=DJI_PID_Cal(&chasis_follow_pid_p,(gimbal_motor[0].cal_angle-4096),chassis_follow_QE.out,CHASIS_FOLLOW_ROATE_SPEED_MAX);  //电机中间值			
		}
		
		chasisSwingcontrol(&chasis_control.swing_angle);	

		
				
}







//摇摆原地不动摇摆最大角度(rad)
#define SWING_NO_MOVE_ANGLE 0.7f
//摇摆过程底盘运动最大角度(rad)
#define SWING_MOVE_ANGLE 0.31415926535897932384626433832795f


/**
  * @brief          底盘跟随云台的行为状态机下，底盘模式是跟随云台角度，底盘旋转速度会根据角度差计算底盘旋转的角速度
  * @author         RM
  * @param[in]      angle_set底盘与云台控制到的相对角度
  * @param[in]      chassis_move_rc_to_vector底盘数据
  * @retval         返回空
  */

void chasisSwingcontrol(fp32 *angle_set)
{
    //摇摆角度是利用sin函数生成，swing_time 是sin函数的输入值
    static fp32 swing_time = 0.0f;
    //swing_time 是计算出来的角度
    static fp32 swing_angle = 0.0f;
    //max_angle 是sin函数的幅值
    static fp32 max_angle = SWING_NO_MOVE_ANGLE;
    //add_time 是摇摆角度改变的快慢，最大越快
    static fp32 const add_time = PI / 250.0f;
    //使能摇摆标志位
    static uint8_t swing_flag = 0;
	
    //判断是否要摇摆
    if (this_key_valu.key_CTRL)  //如果按下CTRL
    {
        if (swing_flag == 0)
        {
            swing_flag = 1;
            swing_time = 0.0f;
        }
    }
    else
    {
        swing_flag = 0;
    }

    //判断键盘输入是不是在控制底盘运动，底盘在运动减小摇摆角度
    if (key_boad.key_W||key_boad.key_A||
        key_boad.key_S||key_boad.key_D)
    {
        max_angle = SWING_MOVE_ANGLE;
    }
    else
    {
        max_angle = SWING_NO_MOVE_ANGLE;
    }
    //sin函数生成控制角度
    if (swing_flag)
    {
        swing_angle = max_angle * arm_sin_f32(swing_time);
        swing_time += add_time;
    }
    else
    {
        swing_angle = 0.0f;
    }
    //sin函数不超过2pi
    if (swing_time > 2 * PI)
    {
        swing_time -= 2 * PI;
    }
    *angle_set = swing_angle;
}


//设置遥控器输入控制量
void chassis_set_contorl()
{
    //设置速度
  fp32 vx_set = 0.0f, vy_set = 0.0f, angle_set = 0.0f; 
	fp32 sin_yaw = 0.0f, cos_yaw = 0.0f;
	//旋转控制底盘速度方向，保证前进方向是云台方向，有利于运动平稳
	sin_yaw = arm_sin_f32(-gimbal_motor[0].cal_angle);
	cos_yaw = arm_cos_f32(-gimbal_motor[0].cal_angle);
	vx_set = cos_yaw * vx_set + sin_yaw * vy_set;
	vy_set = -sin_yaw * vx_set + cos_yaw * vy_set;
//	//设置控制相对云台角度
//	chassis_move_control->chassis_relative_angle_set = rad_format(angle_set);
//	//计算旋转PID角速度
//	chassis_move_control->wz_set = -PID_Calc(&chassis_move_control->chassis_angle_pid, chassis_move_control->chassis_yaw_motor->relative_angle, chassis_move_control->chassis_relative_angle_set);
//	//速度限幅
//	chassis_move_control->vx_set = fp32_constrain(chassis_move_control->vx_set, chassis_move_control->vx_min_speed, chassis_move_control->vx_max_speed);
//	chassis_move_control->vy_set = fp32_constrain(chassis_move_control->vy_set, chassis_move_control->vy_min_speed, chassis_move_control->vy_max_speed);



}

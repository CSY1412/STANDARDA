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

cm_motor_t cm_motor={0};


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
				
			}
			if(system_stat!=preparing)   //不处于准备状态				
			{
				CalChasisAndSend();
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
	
	
		cm_motor.cm_temp1=-chasis_expspeed.y+chasis_expspeed.x+chasis_expspeed.rotate;
		cm_motor.cm_temp2=chasis_expspeed.y+chasis_expspeed.x+chasis_expspeed.rotate;
		cm_motor.cm_temp3=-chasis_expspeed.y-chasis_expspeed.x+chasis_expspeed.rotate;
		cm_motor.cm_temp4=chasis_expspeed.y-chasis_expspeed.x+chasis_expspeed.rotate;
		if (RATE_DO_EXECUTE(RATE_50_HZ, GetSysTickCnt()))  //250hz
			{
						CurrentControl();
			}
		cm_motor.cm_out1=DJI_PID_Cal(&chasis_motor1_pid_v,chasis_motor[0].speed,cm_motor.cm_temp1,CHASIS_MOTOR_MAX_CURRENT+chasis_power_control.out);   //得到底盘
		cm_motor.cm_out2=DJI_PID_Cal(&chasis_motor2_pid_v,chasis_motor[1].speed,cm_motor.cm_temp2,CHASIS_MOTOR_MAX_CURRENT+chasis_power_control.out);
		cm_motor.cm_out3=DJI_PID_Cal(&chasis_motor3_pid_v,chasis_motor[2].speed,cm_motor.cm_temp3,CHASIS_MOTOR_MAX_CURRENT+chasis_power_control.out);
		cm_motor.cm_out4=DJI_PID_Cal(&chasis_motor4_pid_v,chasis_motor[3].speed,cm_motor.cm_temp4,CHASIS_MOTOR_MAX_CURRENT+chasis_power_control.out);
	
		
//		cm_motor.cm_out1=cm_motor.cm_out1-cm_motor.current_offset; //偏差清零
//		cm_motor.cm_out2=cm_motor.cm_out2+cm_motor.current_offset; //偏差清零
//		cm_motor.cm_out3=cm_motor.cm_out3-cm_motor.current_offset; //偏差清零
//		cm_motor.cm_out4=cm_motor.cm_out4+cm_motor.current_offset; //偏差清零
//	
	
		MotorSpeedAnalyze(&cm_motor.cm_out1,&cm_motor.cm_out2,&cm_motor.cm_out3,&cm_motor.cm_out4);//比列调整
	  Set_CM_Speed(cm_motor.cm_out1,cm_motor.cm_out2,cm_motor.cm_out3,cm_motor.cm_out4);//发送电机电流
	

}

/**
  * @brief         电机速度分析和限制
  * @author        蜗牛蜗牛跑
  * @param[in]     4个电机期望转速变量地址    
  * @retval       
  */		
void MotorSpeedAnalyze(float* cm1,float *cm2,float *cm3,float *cm4)
{
		float max_val=GetMax(*cm1,*cm2,*cm3,*cm4);
	
		if(max_val>CHASIS_MOTOR_MAX_SPEED)
		{
			*cm1-=max_val-CHASIS_MOTOR_MAX_SPEED;
			*cm2-=max_val-CHASIS_MOTOR_MAX_SPEED;
			*cm3-=max_val-CHASIS_MOTOR_MAX_SPEED;
			*cm4-=max_val-CHASIS_MOTOR_MAX_SPEED;			
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
	    cm_motor.current_offset=DJI_PID_Cal(&chasis_motor_power_pid,judge_rece_mesg.power_heatdata.chassisPower,
																								MAX_CHASIS_POWER,7000);
	
		 if(cm_motor.current_offset>0)
				 cm_motor.current_offset=0;
		  
	 }
	 else
	 {
			 cm_motor.current_offset=0; //偏差清零
	 }
	 
	 	first_order_filter_cali(&chasis_power_control, cm_motor.current_offset);
}
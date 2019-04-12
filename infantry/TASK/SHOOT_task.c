/**
  ****************************(C) COPYRIGHT 2019 CDTU****************************
  * @file    SHOOT_task.c 
  * @brief    射击任务 
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


#include "SHOOT_task.h"

shoot_t shoot;
//计数型信号量句柄


/**
  * @brief					任务主函数
  * @author         蜗牛蜗牛跑
  * @param[in]         
  * @retval       
  */

#define ONE_BULLET_SHOOT_TIME 220

void SHOOT_task(void *pvParameters)    //状态机任务
{
	u32 lastWakeTime = GetSysTickCnt();  //获取系统时间	

	while(1)        //调试内容
	{
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_1000_HZ));
				
		if (RATE_DO_EXECUTE(RATE_1000_HZ, GetSysTickCnt())) 
			{				
					Judege_Shoot();
			}
	}
}


void Judege_Shoot(void)
{
		 if(shoot_mode==point)  //点射模式
		 {
				if(shoot.exp_bullet>0)
					{
						  FeedMotorON();//
							vTaskDelay(ONE_BULLET_SHOOT_TIME);
							shoot.exp_bullet--;  //减少
					}
					else
					{
							 FeedMotorOFF();					
					}
		 }
		 
		 		 if(shoot_mode==keep)  //点射模式
		 {
						  FeedMotorON(); //持续开火	
		 }
		 
		 
}

